// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Animation/AnimMontage.h"

#include "FPoseInputTable.generated.h"

USTRUCT(BlueprintType)
struct  METEOR_API FPoseInputTable : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pose InputKeys")
	int32 PoseID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pose InputKeys")
	FString Group;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pose InputKeys")
	FString PoseName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pose InputKeys")
	FString PoseInputKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pose InputKeys")
	bool bDefenceBreak;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pose InputKeys")
	int32 Anger;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pose InputKeys")
	bool bInAir;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pose InputKeys")
	TAssetPtr<UAnimMontage> PoseMontage;
};
