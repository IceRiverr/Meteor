// Copyright IceRiver. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NetCharacter.generated.h"

UCLASS()
class METEOR_API ANetCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ANetCharacter();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void StartAttack();

	void StopAttack();

	void PlayAttack();

	UFUNCTION(Server, Reliable, WithValidation)
	void SERVER_Attack(uint32 bAttack);
	void SERVER_Attack_Implementation(uint32 bAttack);
	bool SERVER_Attack_Validate(uint32 bAttack);

public:
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	class UAnimMontage* DaoAttackMtg;

	UPROPERTY(ReplicatedUsing = OnRep_SetAttackFlag)
	uint32 bAttackFlag : 1;

	UFUNCTION()
	void OnRep_SetAttackFlag();
};
