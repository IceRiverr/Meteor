#pragma once

#include "Animation/AnimMontage.h"
#include "GameFramework/Character.h"
#include "Engine/DataTable.h"
#include "Engine/StreamableManager.h"

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

	AAttackCharacter();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void OnAttackV2();

	void StopAttack();

	UAnimMontage* GetPoseMontage(int32 pose);

	void GetAnimMetaData(UAnimMontage* montage);

	bool ConsumeInputKey();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	TAssetPtr<UDataTable> PoseInfoTable;

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

	TMap<int32, int32> TmpPoseTranslationTable;
	
	FTimerHandle NextPoseTransitionTimer;

	bool bAttackKeyDown;

	ATTACK_STATE AttackState;

	float NextPoseTimer;
	UAnimMontage* NextPoseMtg;
};
