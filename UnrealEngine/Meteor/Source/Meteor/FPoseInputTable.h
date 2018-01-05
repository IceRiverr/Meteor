#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Animation/AnimMontage.h"

#include "FPoseInputTable.generated.h"

USTRUCT(BlueprintType)
struct  METEOR_API FPoseStateInfo : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StateDef")
	int32 PoseID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StateDef")
	FString Group;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StateDef")
	FString PoseName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StateDef")
	FString InputCommand;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StateDef")
	bool bDefenceBreak;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StateDef")
	int32 Anger;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StateDef")
	bool bInAir;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StateDef")
	int32 Priority;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StateDef")
	TAssetPtr<UAnimMontage> PoseMontage;
};


USTRUCT(BlueprintType)
struct  METEOR_API FPoseChangeTable : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pose")
	int32 PoseID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pose")
	TArray<int32> NextPoses;
};

USTRUCT(BlueprintType)
struct  METEOR_API FPoseAttackTable : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pose")
	int32 AttackType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pose")
	int32 CheckFriend;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pose")
	float DefenseValue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pose")
	int32 DefenseMove;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pose")
	float TargetValue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pose")
	int32 TargetMove;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pose")
	int32 TargetPose;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pose")
	int32 TargetPoseFront;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pose")
	int32 TargetPoseBack;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pose")
	int32 TargetPoseLeft;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pose")
	int32 TargetPoseRight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pose")
	TArray<FString> Bone;
};
