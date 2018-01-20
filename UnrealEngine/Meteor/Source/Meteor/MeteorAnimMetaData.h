// Copyright IceRiver. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimMetaData.h"

#include "MeteorAnimMetaData.generated.h"

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

	// 连到下一招所使用的时间
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MetaData")
	float NextPoseTime;
};

UENUM()
enum class EAnimFlag : uint8
{
	Unknow = 0,
	Attack,
	Jump,
	Sprint,
	Block,
	BeHit,
	Death,
};

UCLASS()
class UAnimMetaData_Flag : public UAnimMetaData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MetaData")
	EAnimFlag AnimFlag;
};
