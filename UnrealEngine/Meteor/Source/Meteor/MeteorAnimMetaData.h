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
	
	// ����ʽ����ʱ�������Section
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MetaData")
	FName NextPoseIn;

	// ����������ʱ����һ�еĳ���
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MetaData")
	FName NextPoseOut;

	// ������һ����ʹ�õ�ʱ��
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
