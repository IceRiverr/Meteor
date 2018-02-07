// Copyright IceRiver. All Rights Reserved.

#pragma once

#include "Animation/AnimMontage.h"
#include "GameFramework/Character.h"
#include "Engine/DataTable.h"
#include "Engine/StreamableManager.h"

#include "MeteorDataTable.h"
#include "MeteorAnimMetaData.h"
#include "Common/MeteorDef.h"

#include "AttackCharacter.generated.h"

using namespace Meteor;
class UInputBufferComponent;
class UInputCommandComponent;
class UCombatSystemComponent;
class UBoxComponent;

// 此时应该产生一个攻击 TakeDamage();
// 对手接受攻击，然后设置自己的参数，再受到攻击时，直接忽略掉；MUGE使用的JudgePoint，减少死连的情况；
// 应该有操作，来设置，是否可以群攻，正常应该可以群攻，比如刀的招式

UCLASS()
class METEOR_API AAttackCharacter : public ACharacter
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;

public:
	AAttackCharacter();

	virtual void PostInitializeComponents() override;

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	void MoveForward(float Value);

	void MoveRight(float Value);

	void Turn(float Value);

	void LookUp(float Value);

	void OnJump();

	void StopJump();

	UInputCommandComponent* GetInputCommandComponent() { return InputCommandCP; }

	UCombatSystemComponent* GetCombatSystemComponent() { return CombatSystemCP; }

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ActiveAttackBoxs(int PoseAttackIndex);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void CancleAttackBoxs();

	void AttackBoxesTrace();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInputCommandComponent* InputCommandCP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCombatSystemComponent* CombatSystemCP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float MoveFwdSpeedFactor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float MoveBwdSpeedFactor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float MoveRightSpeedFactor;

	TArray<UBoxComponent*> HitBoxCPs;
	TArray<FString> HitBoxNames;
	TArray<bool> HitBoxActives;
	TArray<FVector> HitBoxLastLocations;
private:
	bool bAttackBoxActiveFlag;
	bool bHasAttackOne;
};
