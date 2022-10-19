// Copyright https://github.com/travmygit. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NextGenSplineVolumeGenerator.generated.h"

class ABrush;
class USplineComponent;
class UNextGenSplineVolumeBuilder;

UCLASS()
class NEXTGENSPLINEVOLUME_API ANextGenSplineVolumeGenerator : public AActor
{
	GENERATED_BODY()

public:
	ANextGenSplineVolumeGenerator(const FObjectInitializer& ObjectInitializer);

	/** Generate volume with the configured data. */
	UFUNCTION(CallInEditor, Category = SplineVolume)
	virtual void Generate();
	
#if WITH_EDITORONLY_DATA
protected:
	/** Spline component for volume generation. */
	UPROPERTY(VisibleAnywhere)
	USplineComponent* SplineComponent;
	
	/** Spline volume class for volume generation. */
	UPROPERTY(EditAnywhere, Category = SplineVolume)
	TSubclassOf<ABrush> VolumeClass;
	
	/** Spline volume builder for volume generation. */
	UPROPERTY(EditAnywhere, Instanced, Category = SplineVolume)
	TObjectPtr<UNextGenSplineVolumeBuilder> SplineVolumeBuilder;
	
	/** Cached the generated volume actor. */
	UPROPERTY()
	TObjectPtr<ABrush> Volume;
#endif
};
