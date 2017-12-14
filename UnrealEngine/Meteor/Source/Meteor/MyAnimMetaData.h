// Copyright IceRiver. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimMetaData.h"
#include "MyAnimMetaData.generated.h"

/**
 * 
 */
UCLASS()
class METEOR_API UAnimMetaData_SectionInfo : public UAnimMetaData
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MetaData")
	TMap<FName, float> SectionSpeeds;
	
	// 有招式连入时，进入的Section
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MetaData")
	FName NextPoseIn;

	// 有连招连出时，下一招的出口
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MetaData")
	FName NextPoseOut;
};
