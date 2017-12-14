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
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	enum ATTACK_STATE
	{
		ATK_IDLE,
		ATK_PLAYING,
		ATK_NEXTPOSE,
	};

	// Sets default values for this character's properties
	AAttackCharacter();

	// Called every frame
	void Tick2(float DeltaTime);

	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void OnAttack();

	void StopAttack();

	void OnAttackV2();

	UAnimMontage* GetPoseMontage(int32 pose);

	float GetSectionPlayRate(UAnimMontage* montage, FName sectionName);

	bool CanAttack();

	void GetAnimMetaData(UAnimMontage* montage);

	bool ConsumeInputKey();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	bool bAttack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	TAssetPtr<UDataTable> PoseInfoTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	int32 CurrentPoseIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	bool bAcceptInput;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	float NextPoseTime;

private:
	FString currentToEndMontageName;

	FTimer timer;

	float transationTime;
	bool bStartTransform;
	FAlphaBlend tmpBlend;
	float Pose315_NextPose_Time;

	FStreamableManager StreamMgr;

	TMap<FName, float> SectionRatioCache;
	TMap<FName, bool> SectionRatioHasSet;
	FName NextPoseOut;
	FName NextPoseIn;

	// ½øÐÐ¼ò»¯
	TMap<int32, int32> TmpPoseTranslationTable;
	bool IsInTranslation;

	FTimerHandle NextPoseTransitionTimer;

	bool bAttackKeyDown;

	ATTACK_STATE AttackState;
};
