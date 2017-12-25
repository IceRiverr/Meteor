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
	
	// ����ʽ����ʱ�������Section
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MetaData")
	FName NextPoseIn;

	// ����������ʱ����һ�еĳ���
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MetaData")
	FName NextPoseOut;
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
