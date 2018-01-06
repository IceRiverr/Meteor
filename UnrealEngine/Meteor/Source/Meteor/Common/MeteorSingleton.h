// Copyright IceRiver. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/StreamableManager.h"

#include "MeteorSingleton.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class METEOR_API UMeteorSingleton : public UObject
{
	GENERATED_BODY()
	
public:
	UMeteorSingleton(const FObjectInitializer& ObjectInitializer);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Meteor Global")
	int32 FrameCount;

	FStreamableManager StreamMgr;
};
