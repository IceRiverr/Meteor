// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "MeteorCharacter.h"
#include "Meteor.h"
//#include "Kismet/HeadMountedDisplayFunctionLibrary.h"
#include "LogMacros.h"
#include <string>

//////////////////////////////////////////////////////////////////////////
// AMeteorCharacter

AMeteorCharacter::AMeteorCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(30.f, 90.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

	bEnableMove = true;

	sprintFwdTime = 0.2f;
	sprintBwdTime = 0.2f;
	sprintRightTime = 0.2f;
	sprintLeftTime = 0.2f;

	moveFwdSpeedModifier = 1.0f;
	moveBwdSpeedModifier = 0.2f;
	moveRightSpeedModifier = 0.3f;

	bIsAttackKeyDown = false;

	bJumpSetFlag = false;

	bCanInputAttackKey = true;
}

void AMeteorCharacter::Tick(float dt)
{
	PreTick(dt);
	Super::Tick(dt);

	float currentTime = GetWorld()->GetTimeSeconds();
	
	//UE_LOG(LogTemp, Warning, TEXT("Time: %f"), time2);

	if (CurrentClickInfo.IsAnyKeyClicked())
	{
		CurrentClickInfo.TimeStamp = currentTime;
		KeyClickArray.PushClickInfo(CurrentClickInfo);
	}

	// Jump
	if (bIsJumpKeyClicked)
	{
		UE_LOG(LogTemp, Warning, TEXT("Jump"));
	}

	TestSprintMovement(dt, currentTime);

	TestFireSkill(dt, currentTime);

	PostTick(dt);
}

void AMeteorCharacter::PreTick(float dt)
{
	bIsAttackKeyDown = false;
	bIsJumpKeyClicked = false;
}

void AMeteorCharacter::PostTick(float dt)
{
	CurrentClickInfo.Clear();
}

//////////////////////////////////////////////////////////////////////////
// Input

void AMeteorCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMeteorCharacter::OnJumpKeyClicked);
	PlayerInputComponent->BindAction("Up", IE_Pressed, this, &AMeteorCharacter::OnUpKeyClicked);
	PlayerInputComponent->BindAction("Down", IE_Pressed, this, &AMeteorCharacter::OnDownKeyClicked);
	PlayerInputComponent->BindAction("Left", IE_Pressed, this, &AMeteorCharacter::OnLeftKeyClicked);
	PlayerInputComponent->BindAction("Right", IE_Pressed, this, &AMeteorCharacter::OnRightKeyClicked);
	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &AMeteorCharacter::OnAttackKeyClicked);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMeteorCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMeteorCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &AMeteorCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &AMeteorCharacter::LookUp);
}

float AMeteorCharacter::GetHP()
{
	return 30;
}

float AMeteorCharacter::GetMP()
{
	return 60;
}

void AMeteorCharacter::AddAttackPoint(int atk)
{
	static int _atk = 0;
	_atk += atk;
	UE_LOG(LogTemp, Warning, TEXT("ATK: %d"), _atk);
}

int AMeteorCharacter::GetMoveSprintDirection()
{
	if (bIsSpringFwd)
		return 2;
	else if (bIsSpringBwd)
		return -2;
	else if (bIsSpringRight)
		return 1;
	else if (bIsSpringLeft)
		return -1;
	else
		return 0;
}

bool AMeteorCharacter::GetIsAttackKeyDown()
{
	return bIsAttackKeyDown;
}

void AMeteorCharacter::SetJumpSetFlag(bool flag)
{
	bJumpSetFlag = flag;
}

bool AMeteorCharacter::GetJumpSetFlag()
{
	return bJumpSetFlag;
}

void AMeteorCharacter::SetCanInputAttackKey(bool attack)
{
	bCanInputAttackKey = attack;
}

bool AMeteorCharacter::GetCanInputAttackKey()
{
	auto com = GetMovementComponent();
	if (com &&  com->IsFalling())
		return false;
	else
		return bCanInputAttackKey;
}

ECombooKey::Type AMeteorCharacter::GetCombooKey()
{
	ECombooKey::Type comKey = ECombooKey::COM_INVALID;

	float currentTime = GetWorld()->GetTimeSeconds();

	TArray<Meteor::EAttackKey> U_A_Keys;
	U_A_Keys.Push(Meteor::EAttackKey::ATTACK_UP);

	TArray<Meteor::EAttackKey> D_A_Keys;
	D_A_Keys.Push(Meteor::EAttackKey::ATTACK_DOWN);

	TArray<Meteor::EAttackKey> L_A_Keys;
	L_A_Keys.Push(Meteor::EAttackKey::ATTACK_LEFT);

	TArray<Meteor::EAttackKey> R_A_Keys;
	R_A_Keys.Push(Meteor::EAttackKey::ATTACK_RIGHT);

	float interval = 0.5f;

	if (bIsAttackKeyDown)
	{
		if (KeyClickArray.IsKeysDown(currentTime, interval, U_A_Keys))
			comKey = ECombooKey::COM_U_A;
		else if (KeyClickArray.IsKeysDown(currentTime, interval, D_A_Keys))
			comKey = ECombooKey::COM_D_A;
		else if(KeyClickArray.IsKeysDown(currentTime, interval, L_A_Keys))
			comKey = ECombooKey::COM_L_A;
		else if(KeyClickArray.IsKeysDown(currentTime, interval, R_A_Keys))
			comKey = ECombooKey::COM_R_A;
	}
	return comKey;
}

void AMeteorCharacter::Turn(float Value)
{
	if (bEnableMove)
	{
		AddControllerYawInput(Value);
	}
}

void AMeteorCharacter::LookUp(float Value)
{
	if (bEnableMove)
	{
		AddControllerPitchInput(Value);
	}
}

void AMeteorCharacter::OnJumpKeyClicked()
{
	bIsJumpKeyClicked = true;
	ACharacter::Jump();
}

void AMeteorCharacter::OnUpKeyClicked()
{
	CurrentClickInfo.bUpKeyClick = true;
}

void AMeteorCharacter::OnDownKeyClicked()
{
	CurrentClickInfo.bDownKeyClick = true;
}

void AMeteorCharacter::OnLeftKeyClicked()
{
	CurrentClickInfo.bLeftKeyClick = true;
}

void AMeteorCharacter::OnRightKeyClicked()
{
	CurrentClickInfo.bRightKeyClick = true;
}

void AMeteorCharacter::OnAttackKeyClicked()
{
	bIsAttackKeyDown = true;
}

bool AMeteorCharacter::TestSprintMovement(float dt, float currentTime)
{
	// Sprint Fwd
	TArray<Meteor::EAttackKey> U_U_AttackKeys;
	U_U_AttackKeys.Push(Meteor::EAttackKey::ATTACK_UP);
	U_U_AttackKeys.Push(Meteor::EAttackKey::ATTACK_UP);
	bIsSpringFwd = false;
	if (KeyClickArray.IsKeysDown(currentTime, 0.5f, U_U_AttackKeys))
	{
		if (currentTime - sprintFwdStampTime > sprintFwdTime)
		{
			UE_LOG(LogTemp, Warning, TEXT("Up-Up"));
			bIsSpringFwd = true;
			sprintFwdStampTime = currentTime;
		}
	}

	// Sprint Bwd
	TArray<Meteor::EAttackKey> D_D_AttackKeys;
	D_D_AttackKeys.Push(Meteor::EAttackKey::ATTACK_DOWN);
	D_D_AttackKeys.Push(Meteor::EAttackKey::ATTACK_DOWN);
	bIsSpringBwd = false;
	if (KeyClickArray.IsKeysDown(currentTime, 0.5f, D_D_AttackKeys))
	{
		if (currentTime - sprintBwdStampTime > sprintBwdTime)
		{
			UE_LOG(LogTemp, Warning, TEXT("D-D"));
			bIsSpringBwd = true;
			sprintBwdStampTime = currentTime;
		}
	}

	// Sprint Right
	TArray<Meteor::EAttackKey> R_R_AttackKeys;
	R_R_AttackKeys.Push(Meteor::EAttackKey::ATTACK_RIGHT);
	R_R_AttackKeys.Push(Meteor::EAttackKey::ATTACK_RIGHT);
	bIsSpringRight = false;
	if (KeyClickArray.IsKeysDown(currentTime, 0.5f, R_R_AttackKeys))
	{
		if (currentTime - sprintRightStampTime > sprintRightTime)
		{
			UE_LOG(LogTemp, Warning, TEXT("R-R"));
			bIsSpringRight = true;
			sprintRightStampTime = currentTime;
		}
	}

	// sprint left
	TArray<Meteor::EAttackKey> L_L_AttackKeys;
	L_L_AttackKeys.Push(Meteor::EAttackKey::ATTACK_LEFT);
	L_L_AttackKeys.Push(Meteor::EAttackKey::ATTACK_LEFT);
	bIsSpringLeft = false;
	if (KeyClickArray.IsKeysDown(currentTime, 0.5f, L_L_AttackKeys))
	{
		if (currentTime - sprintLeftStampTime > sprintLeftTime)
		{
			UE_LOG(LogTemp, Warning, TEXT("L-L"));
			bIsSpringLeft = true;
			sprintLeftStampTime = currentTime;
		}
	}

	if (bIsSpringFwd || bIsSpringBwd || bIsSpringRight || sprintLeftStampTime)
		return true;
	
	return false;
}

bool AMeteorCharacter::TestFireSkill(float dt, float currentTime)
{
	TArray<Meteor::EAttackKey> U_D_AttackKeys;
	U_D_AttackKeys.Push(Meteor::EAttackKey::ATTACK_UP);
	U_D_AttackKeys.Push(Meteor::EAttackKey::ATTACK_DOWN);

	TArray<Meteor::EAttackKey> L_U_R_AttackKeys;
	L_U_R_AttackKeys.Push(Meteor::EAttackKey::ATTACK_LEFT);
	L_U_R_AttackKeys.Push(Meteor::EAttackKey::ATTACK_UP);
	L_U_R_AttackKeys.Push(Meteor::EAttackKey::ATTACK_RIGHT);

	//FKey attackKey = FKey("Attack");
	if (Controller && Controller->IsLocalPlayerController())
	{
		APlayerController* const PC = CastChecked<APlayerController>(Controller);
		bool bIsCanAttack = PC->WasInputKeyJustPressed(EKeys::LeftMouseButton);
		if (bIsCanAttack)
		{
			UE_LOG(LogTemp, Warning, TEXT("Attack"));
		}
	}

	if (bIsAttackKeyDown)
	{
		if (KeyClickArray.IsKeysDown(currentTime, 0.5f, U_D_AttackKeys))
		{
			UE_LOG(LogTemp, Warning, TEXT("Up-Down-Attack"));
		}

		if (KeyClickArray.IsKeysDown(currentTime, 1.0f, L_U_R_AttackKeys))
		{
			UE_LOG(LogTemp, Warning, TEXT("Left-Right-Up-Attack"));

		}
	}

	return false;
}

void AMeteorCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		auto moveCom = GetMovementComponent();
		if (moveCom && !moveCom->IsFalling())
		{
			float inputSpeed = 0.0f;
			if (Value > 0.0f)
			{
				inputSpeed = Value * moveFwdSpeedModifier;
			}
			else if (Value < 0.0f)
			{
				inputSpeed = Value * moveBwdSpeedModifier;
			}
			AddMovementInput(GetActorForwardVector(), inputSpeed);
		}
	}
}

void AMeteorCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		auto moveCom = GetMovementComponent();
		if (moveCom && !moveCom->IsFalling())
		{
			float inputSpeed = Value * moveRightSpeedModifier;
			AddMovementInput(GetActorRightVector(), inputSpeed);
		}
	}
}
