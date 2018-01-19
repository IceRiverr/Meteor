// Copyright IceRiver. All Rights Reserved.

#pragma once

#include "Animation/AnimMontage.h"
#include "GameFramework/Character.h"
#include "Engine/DataTable.h"
#include "Engine/StreamableManager.h"

#include "MeteorDataTable.h"
#include "MeteorAnimMetaData.h"
#include "Meteor/Common/MeteorDef.h"

#include "AttackCharacter.generated.h"

using namespace Meteor;
class UInputBufferComponent;
class UInputCommandComponent;
class UCombatSystemComponent;
class UBoxComponent;

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

	enum class SPRINT_DIRECTION
	{
		SPRINT_IDLE = 0,
		SPRINT_FWD,
		SPRINT_BWD,
		SPRINT_RIGHT,
		SPRINT_LEFT,
	};

	struct CombatAction
	{
		FName InputCmd;
		int32 PoseIndex;
	};

	AAttackCharacter();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MoveForward(float Value);

	void MoveRight(float Value);

	void Turn(float Value);

	void LookUp(float Value);

	void OnJump();

	void StopJump();

	void Test_createSection(UAnimMontage* montage);

	UFUNCTION(BlueprintCallable, Category = "Test")
	void Play_TestMontage();

	UInputCommandComponent* GetInputCommandComponent() { return InputCommandCP; }

	UCombatSystemComponent* GetCombatSystemComponent() { return CombatSystemCP; }

	UFUNCTION(BlueprintNativeEvent, Category = Collision)
	void OnOverlapBegin(UPrimitiveComponent* Comp, AActor* otherActor, UPrimitiveComponent* otherComp,
		int otherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void PostInitializeComponents() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInputCommandComponent* InputCommandCP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCombatSystemComponent* CombatSystemCP;

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* bau_Head_Test;*/

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* bau_Head;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* bau_Spine;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* bad_L_Thigh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* bad_L_Calf;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* bad_L_Foot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* bau_L_UpperArm;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* bau_L_Forearm;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* bau_L_Hand;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* bau_Spine1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* bau_Neck;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* bad_R_Thigh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* bad_R_Calf;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* bad_R_Foot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* bau_R_UpperArm;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* bau_R_Forearm;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* bau_R_Hand;*/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float MoveFwdSpeedFactor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float MoveBwdSpeedFactor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float MoveRightSpeedFactor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat)
	UAnimMontage* TestMtg;

	TArray<UBoxComponent*> HitBoxCPs;
	TArray<FString> HitBoxNames;
private:
};
