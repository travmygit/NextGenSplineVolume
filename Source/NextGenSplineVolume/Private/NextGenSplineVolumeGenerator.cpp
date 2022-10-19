// Copyright https://github.com/travmygit. All Rights Reserved.


#include "NextGenSplineVolumeGenerator.h"
#if WITH_EDITOR
#include "Engine/Polys.h"
#include "Components/BrushComponent.h"
#include "Components/SplineComponent.h"
#include "NextGenSplineVolumeBuilder.h"
#endif


ANextGenSplineVolumeGenerator::ANextGenSplineVolumeGenerator(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	USceneComponent* SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent0"));
	RootComponent = SceneComponent;

#if WITH_EDITORONLY_DATA
	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent0"));
	SplineComponent->SetupAttachment(RootComponent);
#endif
}

void ANextGenSplineVolumeGenerator::Generate()
{
#if WITH_EDITOR
	if (GetWorld()->IsGameWorld())
	{
		UE_LOG(LogTemp, Error, TEXT("ANextGenSplineVolumeGenerator: Failed, can only generate in editor!"));
		return;
	}
	
	if (!VolumeClass)
	{
		UE_LOG(LogTemp, Error, TEXT("ANextGenSplineVolumeGenerator: Failed, no 'VolumeClass'!"));
		return;
	}

	if (!SplineVolumeBuilder)
	{
		UE_LOG(LogTemp, Error, TEXT("ANextGenSplineVolumeGenerator: Failed, no 'SplineVolumeBuilder'!"));
		return;
	}

	FNextGenSplineParams SplineParams;
	SplineParams.bClosedLoop = SplineComponent->IsClosedLoop();
	const auto& SplinePoints = SplineComponent->GetSplinePointsPosition().Points;
	SplineParams.Points.Reserve(SplinePoints.Num());
	for (const auto& Point : SplinePoints)
	{
		SplineParams.Points.Add(Point.OutVal);
	}

	if (SplinePoints.Num() < 2)
	{
		UE_LOG(LogTemp, Error, TEXT("ANextGenSplineVolumeGenerator: Failed, number of spline points less than two!"));
		return;
	}

	if (IsValid(Volume) && Volume.GetClass() != VolumeClass)
	{
		Volume->Destroy();
		Volume = nullptr;
	}
	
	if (!IsValid(Volume))
	{
		FActorSpawnParameters ActorSpawnParameters;
		ActorSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		Volume = CastChecked<ABrush>(GetWorld()->SpawnActor(VolumeClass, &SplineComponent->GetComponentTransform(), ActorSpawnParameters));
		EObjectFlags NewObjectFlags = Volume->GetFlags() & (RF_Transient | RF_Transactional);
		Volume->PolyFlags = 0;
		Volume->Brush = NewObject<UModel>(Volume, NAME_None, NewObjectFlags);
		Volume->Brush->Initialize(nullptr, true);
		Volume->Brush->Polys = NewObject<UPolys>(Volume->Brush, NAME_None, NewObjectFlags);
		Volume->GetBrushComponent()->Brush = Volume->Brush;
		Volume->BrushBuilder = nullptr;
	}
	else
	{
		Volume->SetActorTransform(SplineComponent->GetComponentTransform());
	}
	
	Volume->PreEditChange(nullptr);
	
	if (SplineVolumeBuilder->Build(Volume, SplineParams))
	{
		const int32 NumVertices = SplineVolumeBuilder->GetVertices().Num();
		const int32 NumPolys = SplineVolumeBuilder->GetPolys().Num();
		UE_LOG(LogTemp, Log, TEXT("ANextGenSplineVolumeGenerator: Success, %d vertices and %d polygons."), NumVertices, NumPolys);
	}
	
	Volume->PostEditChange();
#else
	UE_LOG(LogTemp, Error, TEXT("ANextGenSplineVolumeGenerator: Failed, can only generate in editor!"));
	return;
#endif
}
