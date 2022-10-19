// Copyright https://github.com/travmygit. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Volume.h"
#include "NextGenSplineVolumeBase.generated.h"

UCLASS(Abstract, Blueprintable)
class NEXTGENSPLINEVOLUME_API ANextGenSplineVolumeBase : public AVolume
{
	GENERATED_BODY()

public:
	ANextGenSplineVolumeBase();

#if WITH_EDITOR
	virtual bool ShouldCheckCollisionComponentForErrors() const { return false; }
#endif
};
