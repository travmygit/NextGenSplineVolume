// Copyright https://github.com/travmygit. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/BrushBuilder.h"
#include "NextGenSplineVolumeBuilder.generated.h"

USTRUCT()
struct FNextGenSplineParams
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FVector> Points;

	UPROPERTY()
	bool bClosedLoop;
};

UCLASS(Abstract, MinimalAPI, HideCategories = (Object))
class UNextGenSplineVolumeBuilder : public UObject
{
	GENERATED_BODY()
	
public:
	virtual bool Build(ABrush* Volume, const FNextGenSplineParams& SplineParams);

	const TArray<FVector>& GetVertices() const { return Vertices; }
	const TArray<struct FBuilderPoly>& GetPolys() const { return Polys; }
	
protected:
	virtual void BeginBuild();
	virtual bool EndBuild(ABrush* Volume);
	virtual void BuildImpl(const FNextGenSplineParams& SplineParams) PURE_VIRTUAL(UNextGenSplineVolumeBuilder::BuildImpl,);
	int32 Vertexv( FVector v );
	int32 Vertex3f( float X, float Y, float Z );
	void Poly3i( int32 Direction, int32 i, int32 j, int32 k, FName ItemName = NAME_None, bool bIsTwoSidedNonSolid = false );
	void Poly4i( int32 Direction, int32 i, int32 j, int32 k, int32 l, FName ItemName = NAME_None, bool bIsTwoSidedNonSolid = false );
	void PolyBegin( int32 Direction, FName ItemName = NAME_None );
	void Polyi( int32 i );
	void PolyEnd();

protected:
	UPROPERTY()
	TArray<FVector> Vertices;

	UPROPERTY()
	TArray<struct FBuilderPoly> Polys;
};

UCLASS(MinimalAPI, EditInlineNew, HideCategories = (Object), meta = (DisplayName = "SimpleBorder"))
class UNextGenSplineVolumeBuilder_SimpleBorder : public UNextGenSplineVolumeBuilder
{
	GENERATED_BODY()

protected:
	virtual void BuildImpl(const FNextGenSplineParams& SplineParams) override;
	
private:
	UPROPERTY(EditAnywhere, Category = "SimpleBorder", meta = (ClampMin = "0"))
	float HalfBoundsWidth = 50.f;

	UPROPERTY(EditAnywhere, Category = "SimpleBorder", meta = (ClampMin = "0"))
	float LowerBoundsHeight = 0.f;

	UPROPERTY(EditAnywhere, Category = "SimpleBorder", meta = (ClampMin = "0"))
	float UpperBoundsHeight = 100.f;
};

UCLASS(MinimalAPI, EditInlineNew, HideCategories = (Object), meta = (DisplayName = "SimplePolygon"))
class UNextGenSplineVolumeBuilder_SimplePolygon : public UNextGenSplineVolumeBuilder
{
	GENERATED_BODY()

public:
	virtual bool Build(ABrush* Volume, const FNextGenSplineParams& SplineParams) override;
	
protected:
	virtual void BuildImpl(const FNextGenSplineParams& SplineParams) override;
	
private:
	UPROPERTY(EditAnywhere, Category = "SimplePolygon", meta = (ClampMin = "0"))
	float LowerBoundsHeight = 0.f;

	UPROPERTY(EditAnywhere, Category = "SimplePolygon", meta = (ClampMin = "0"))
	float UpperBoundsHeight = 100.f;
};
