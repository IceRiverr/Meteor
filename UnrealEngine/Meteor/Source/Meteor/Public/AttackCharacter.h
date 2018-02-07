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

// ��ʱӦ�ò���һ������ TakeDamage();
// ���ֽ��ܹ�����Ȼ�������Լ��Ĳ��������ܵ�����ʱ��ֱ�Ӻ��Ե���MUGEʹ�õ�JudgePoint�����������������
// Ӧ���в����������ã��Ƿ����Ⱥ��������Ӧ�ÿ���Ⱥ�������絶����ʽ

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
