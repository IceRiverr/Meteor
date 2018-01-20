// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "AttackKeyClickInfo.h"
#include "GameFramework/Character.h"
#include "MeteorCharacter.generated.h"

UCLASS(config=Game)
class AMeteorCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
public:
	AMeteorCharacter();

	virtual void Tick(float dt) override;
	void PreTick(float dt);
	void PostTick(float dt);


	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Control)
	float bEnableMove;

protected:
	void MoveForward(float Value);

	void MoveRight(float Value);

	void Turn(float Value);

	void LookUp(float Value);

	void OnJumpKeyClicked();

	void OnUpKeyClicked();

	void OnDownKeyClicked();

	void OnLeftKeyClicked();

	void OnRightKeyClicked();

	void OnAttackKeyClicked();

	bool TestSprintMovement(float dt, float currentTime);

	bool TestFireSkill(float dt, float currentTime);

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
public:
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	// 下面的信息最好实现为借口，并且单独写一个模块
	UFUNCTION(BlueprintCallable, Category = "Meteor")
	float GetHP();

	UFUNCTION(BlueprintCallable, Category = "Meteor")
	float GetMP();

	UFUNCTION(BlueprintCallable, Category = "Meteor")
	void AddAttackPoint(int atk);

	// 2 fwd -2 bwd 1 right -1 left
	UFUNCTION(BlueprintCallable, Category = "Meteor")
	int GetMoveSprintDirection(); 

	UFUNCTION(BlueprintCallable, Category = "Meteor")
	bool GetIsAttackKeyDown();

	UFUNCTION(BlueprintCallable, Category = "Meteor")
	void SetJumpSetFlag(bool flag);

	UFUNCTION(BlueprintCallable, Category = "Meteor")
	bool GetJumpSetFlag();

	UFUNCTION(BlueprintCallable, Category = "Meteor")
	void SetCanInputAttackKey(bool attack);

	UFUNCTION(BlueprintCallable, Category = "Meteor")
	bool GetCanInputAttackKey();

	UFUNCTION(BlueprintCallable, Category = "Meteor")
	ECombooKey::Type GetCombooKey();

private:
	AttackKeyClickInfo CurrentClickInfo;
	AttackKeyClickArray KeyClickArray;

	bool bIsJumpKeyClicked;
	
	bool bIsSpringFwd;
	float sprintFwdTime;
	float sprintFwdStampTime;

	bool bIsSpringBwd;
	float sprintBwdTime;
	float sprintBwdStampTime;

	bool bIsSpringRight;
	float sprintRightTime;
	float sprintRightStampTime;

	bool bIsSpringLeft;
	float sprintLeftTime;
	float sprintLeftStampTime;

	float moveFwdSpeedModifier;
	float moveBwdSpeedModifier;
	float moveRightSpeedModifier;

	bool bIsAttackKeyDown;

	bool bJumpSetFlag;

	bool bCanInputAttackKey;
};
