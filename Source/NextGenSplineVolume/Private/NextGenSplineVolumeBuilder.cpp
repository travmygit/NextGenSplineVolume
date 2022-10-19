// Copyright https://github.com/travmygit. All Rights Reserved.


#include "NextGenSplineVolumeBuilder.h"
#if WITH_EDITOR
#include "BSPOps.h"
#include "Engine/Polys.h"
#endif


bool UNextGenSplineVolumeBuilder::Build(ABrush* Volume, const FNextGenSplineParams& SplineParams)
{
#if WITH_EDITOR
	BeginBuild();
	BuildImpl(SplineParams);
	return EndBuild(Volume);
#else
	return false;
#endif
}

void UNextGenSplineVolumeBuilder::BeginBuild()
{
#if WITH_EDITOR
	Vertices.Empty();
	Polys.Empty();
#endif
}

bool UNextGenSplineVolumeBuilder::EndBuild(ABrush* Volume)
{
#if WITH_EDITOR
	check(Volume);
	Volume->Brush->Modify();
	Volume->Modify();
	Volume->Brush->Polys->Element.Empty();
	
	for (const auto& PolyBuilder : Polys)
	{
		FPoly Poly;
		Poly.Init();
		Poly.ItemName = PolyBuilder.ItemName;
		Poly.Base = (FVector3f)Vertices[PolyBuilder.VertexIndices[0]];
		Poly.PolyFlags = PolyBuilder.PolyFlags;
		Poly.iBrushPoly = INDEX_NONE;
		Poly.Material = nullptr;
		for (int32 i = 0, num = PolyBuilder.VertexIndices.Num(); i < num; i++)
		{
			new(Poly.Vertices) FVector3f(Vertices[PolyBuilder.VertexIndices[i]]);
		}
		if (Poly.Finalize(Volume, 1) == 0)
		{
			new(Volume->Brush->Polys->Element) FPoly(Poly);
		}
	}

	FBSPOps::csgPrepMovingBrush(Volume);
	Volume->ReregisterAllComponents();
	
	return true;
#else
	return false;
#endif
}

int32 UNextGenSplineVolumeBuilder::Vertexv(FVector V)
{
	int32 Result = Vertices.Num();
	new(Vertices)FVector(V);

	return Result;
}

int32 UNextGenSplineVolumeBuilder::Vertex3f(float X, float Y, float Z)
{
	int32 Result = Vertices.Num();
	new(Vertices)FVector(X,Y,Z);
	return Result;
}

void UNextGenSplineVolumeBuilder::Poly3i(int32 Direction, int32 i, int32 j, int32 k, FName ItemName, bool bIsTwoSidedNonSolid )
{
	new(Polys)FBuilderPoly;
	Polys.Last().Direction=Direction;
	Polys.Last().ItemName=ItemName;
	new(Polys.Last().VertexIndices)int32(i);
	new(Polys.Last().VertexIndices)int32(j);
	new(Polys.Last().VertexIndices)int32(k);
	Polys.Last().PolyFlags = PF_DefaultFlags | (bIsTwoSidedNonSolid ? (PF_TwoSided|PF_NotSolid) : 0);
}

void UNextGenSplineVolumeBuilder::Poly4i(int32 Direction, int32 i, int32 j, int32 k, int32 l, FName ItemName, bool bIsTwoSidedNonSolid )
{
	new(Polys)FBuilderPoly;
	Polys.Last().Direction=Direction;
	Polys.Last().ItemName=ItemName;
	new(Polys.Last().VertexIndices)int32(i);
	new(Polys.Last().VertexIndices)int32(j);
	new(Polys.Last().VertexIndices)int32(k);
	new(Polys.Last().VertexIndices)int32(l);
	Polys.Last().PolyFlags = PF_DefaultFlags | (bIsTwoSidedNonSolid ? (PF_TwoSided|PF_NotSolid) : 0);
}

void UNextGenSplineVolumeBuilder::PolyBegin(int32 Direction, FName ItemName)
{
	new(Polys)FBuilderPoly;
	Polys.Last().ItemName=ItemName;
	Polys.Last().Direction = Direction;
	Polys.Last().PolyFlags = PF_DefaultFlags;
}

void UNextGenSplineVolumeBuilder::Polyi(int32 i)
{
	new(Polys.Last().VertexIndices)int32(i);
}

void UNextGenSplineVolumeBuilder::PolyEnd()
{
}

void UNextGenSplineVolumeBuilder_SimpleBorder::BuildImpl(const FNextGenSplineParams& SplineParams)
{
	check(SplineParams.Points.Num() > 1);
	const int32 PointsSize = SplineParams.Points.Num();
	const bool bLoop = SplineParams.bClosedLoop && PointsSize > 2;
	for (int i = 0; i < PointsSize; ++i)
	{
		if (i == 0)
		{
			const FVector V = (SplineParams.Points[i + 1] - SplineParams.Points[i]).GetSafeNormal();
			FVector N;
			if (bLoop)
			{
				const FVector U = (SplineParams.Points[0] - SplineParams.Points[PointsSize - 1]);
				N = ((V ^ FVector::UpVector).GetSafeNormal() + (U ^ FVector::UpVector).GetSafeNormal()).GetSafeNormal();
			}
			else
			{
				N = (V ^ FVector::UpVector).GetSafeNormal();
			}
			const float Offset = HalfBoundsWidth / FMath::Abs((V ^ N).Size());
			Vertexv(SplineParams.Points[i] + N * Offset + FVector(0, 0, -LowerBoundsHeight));
			Vertexv(SplineParams.Points[i] + N * Offset + FVector(0, 0, UpperBoundsHeight));
			Vertexv(SplineParams.Points[i] - N * Offset + FVector(0, 0, -LowerBoundsHeight));
			Vertexv(SplineParams.Points[i] - N * Offset + FVector(0, 0, UpperBoundsHeight));
		}
		else if (i == (PointsSize - 1))
		{
			const FVector V = (SplineParams.Points[i] - SplineParams.Points[i - 1]).GetSafeNormal();
			FVector N;
			if (bLoop)
			{
				const FVector U = (SplineParams.Points[0] - SplineParams.Points[i]);
				N = ((V ^ FVector::UpVector).GetSafeNormal() + (U ^ FVector::UpVector).GetSafeNormal()).GetSafeNormal();
			}
			else
			{
				N = (V ^ FVector::UpVector).GetSafeNormal();
			}
			const float Offset = HalfBoundsWidth / FMath::Abs((V ^ N).Size());
			Vertexv(SplineParams.Points[i] + N * Offset + FVector(0, 0, -LowerBoundsHeight));
			Vertexv(SplineParams.Points[i] + N * Offset + FVector(0, 0, UpperBoundsHeight));
			Vertexv(SplineParams.Points[i] - N * Offset + FVector(0, 0, -LowerBoundsHeight));
			Vertexv(SplineParams.Points[i] - N * Offset + FVector(0, 0, UpperBoundsHeight));
		}
		else
		{
			const FVector V = (SplineParams.Points[i + 1] - SplineParams.Points[i]).GetSafeNormal();
			const FVector U = (SplineParams.Points[i] - SplineParams.Points[i - 1]);
			const FVector N = ((V ^ FVector::UpVector).GetSafeNormal() + (U ^ FVector::UpVector).GetSafeNormal()).GetSafeNormal();
			const float Offset = HalfBoundsWidth / FMath::Abs((V ^ N).Size());
			Vertexv(SplineParams.Points[i] + N * Offset + FVector(0, 0, -LowerBoundsHeight));
			Vertexv(SplineParams.Points[i] + N * Offset + FVector(0, 0, UpperBoundsHeight));
			Vertexv(SplineParams.Points[i] - N * Offset + FVector(0, 0, -LowerBoundsHeight));
			Vertexv(SplineParams.Points[i] - N * Offset + FVector(0, 0, UpperBoundsHeight));
		}

		if (i == 0)
		{
			if (bLoop == false)
			{
				Poly4i(1, 4 * i + 0, 4 * i + 1, 4 * i + 3, 4 * i + 2);
			}
		}
		else
		{
			Poly4i(1, 4 * i + 0, 4 * i + 1, 4 * i - 3, 4 * i - 4);
			Poly4i(1, 4 * i + 1, 4 * i + 3, 4 * i - 1, 4 * i - 3);
			Poly4i(1, 4 * i + 3, 4 * i + 2, 4 * i - 2, 4 * i - 1);
			Poly4i(1, 4 * i + 2, 4 * i + 0, 4 * i - 4, 4 * i - 2);
			if (i == (PointsSize - 1))
			{
				if (bLoop)
				{
					Poly4i(1, 0, 1, 4 * i + 1, 4 * i + 0);
					Poly4i(1, 1, 3, 4 * i + 3, 4 * i + 1);
					Poly4i(1, 3, 2, 4 * i + 2, 4 * i + 3);
					Poly4i(1, 2, 0, 4 * i + 0, 4 * i + 2);
				}
				else
				{
					Poly4i(1, 4 * i + 2, 4 * i + 3, 4 * i + 1, 4 * i + 0);
				}
			}
		}
	}
}

bool UNextGenSplineVolumeBuilder_SimplePolygon::Build(ABrush* Volume, const FNextGenSplineParams& SplineParams)
{
#if WITH_EDITOR
	if (SplineParams.Points.Num() > 2)
	{
		BeginBuild();
		BuildImpl(SplineParams);
		return EndBuild(Volume);
	}
	UE_LOG(LogTemp, Error, TEXT("ANextGenSplineVolumeGenerator: Failed, number of spline points less than three!"));
	return false;
#else
	return false;
#endif
}

void UNextGenSplineVolumeBuilder_SimplePolygon::BuildImpl(const FNextGenSplineParams& SplineParams)
{
	check(SplineParams.Points.Num() > 2);
	const int32 Direction = (((SplineParams.Points[1] - SplineParams.Points[0]) ^ (SplineParams.Points[2] - SplineParams.Points[0])) | FVector::UpVector) > 0 ? 1 : -1;
	const int32 PointsSize = SplineParams.Points.Num();
	for (int i = 0; i < PointsSize; ++i)
	{
		if (Direction > 0)
		{
			Vertexv(SplineParams.Points[i] + FVector(0, 0, -LowerBoundsHeight));
			Vertexv(SplineParams.Points[i] + FVector(0, 0, UpperBoundsHeight));
		}
		else
		{
			Vertexv(SplineParams.Points[i] + FVector(0, 0, UpperBoundsHeight));
			Vertexv(SplineParams.Points[i] + FVector(0, 0, -LowerBoundsHeight));
		}

		if (i > 0)
		{
			Poly4i(1, 2 * i + 0, 2 * i + 1, 2 * i - 1, 2 * i - 2);
			if (i == (PointsSize - 1))
			{
				Poly4i(1, 0, 1, 2 * i + 1, 2 * i + 0);
			}
		}
	}

	PolyBegin(1);
	for (int i = PointsSize - 1; i >= 0; --i)
	{
		Polyi(2 * i);
	}
	PolyEnd();

	PolyBegin(1);
	for (int i = 0; i < PointsSize; ++i)
	{
		Polyi(2 * i + 1);
	}
	PolyEnd();
}
