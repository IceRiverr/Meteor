#pragma once

#include "Animation/AnimMontage.h"
#include "GameFramework/Character.h"
#include "Engine/DataTable.h"
#include "Engine/StreamableManager.h"
#include "FPoseInputTable.h"

#include "AttackCharacter.generated.h"

UCLASS()
class METEOR_API AAttackCharacter : public ACharacter
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;

public:
	enum ATTACK_STATE
	{
		ATK_IDLE,
		ATK_PLAYING,
		ATK_NEXTPOSE,
	};

	// Left Right Up Down Attack
	enum ATTACK_KEY
	{
		KEY_L = 0,
		KEY_R,
		KEY_U,
		KEY_D,
		KEY_A,
		KEY_NUM,
	};

	struct FrameInputKey
	{
		FrameInputKey();
		FrameInputKey(ATTACK_KEY key, float time);
		void SetKeyDown(ATTACK_KEY key);
		bool GetValidChars(TArray<TCHAR>& chars) const;
		bool GetValidStr(FString& str) const;
		void Reset();

		float timeStamp;

		bool L_Down;
		bool R_Down;
		bool U_Down;
		bool D_Down;
		bool A_Down;
	};

	AAttackCharacter();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void OnAttack();

	void StopAttack();

	void OnLeftKeyDown();

	void OnRightKeyDown();

	void OnUpKeyDown();

	void OnDownKeyDown();

	UAnimMontage* GetPoseMontage(int32 pose);

	void GetAnimMetaData(UAnimMontage* montage);

	bool ConsumeInputKey();

	TArray<FString> GetPosiableCombinationKey();

	// 因为每一帧可能同时按下几个键，并且由于可能的组合键有1-4个，所有全部需要生成
	TArray<FString> GetAllCominationKey();

	void PushAttackKey(ATTACK_KEY key, float time);

	int32 GetNextPose(int32 poseIdx, const TArray<FString>& inputCmds, bool bInAir = false);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	TAssetPtr<UDataTable> PoseInfoTable;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	TAssetPtr<UDataTable> Dao_PoseChangeTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	int32 CurrentPoseIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	bool bAcceptInput;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	float NextPoseTime;

private:
	FStreamableManager StreamMgr;

	// attack montage
	TMap<FName, float> SectionRatioCache;
	TMap<FName, bool> SectionRatioHasSet;
	FName NextPoseOut;
	FName NextPoseIn;
	
	FAlphaBlend tmpBlend;

	FTimerHandle NextPoseTransitionTimer;

	bool bAttackKeyDown;

	ATTACK_STATE AttackState;

	float NextPoseTimer;
	UAnimMontage* NextPoseMtg;

	int32 MAX_INPUT_BUFFER;
	TArray<FrameInputKey> InputBuffer;

	TArray<FPoseInputTable*> Dao_AllPoses;

	int32 Dao_FirstAttackPose;
};
