// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "Meteor.h"
#include "Kismet/HeadMountedDisplayFunctionLibrary.h"
#include "MeteorCharacter.h"
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

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

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
}

void AMeteorCharacter::Tick(float dt)
{
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

	TArray<Meteor::EAttackKey> U_U_AttackKeys;
	U_U_AttackKeys.Push(Meteor::EAttackKey::ATTACK_UP);
	U_U_AttackKeys.Push(Meteor::EAttackKey::ATTACK_UP);

	TArray<Meteor::EAttackKey> U_D_AttackKeys;
	U_D_AttackKeys.Push(Meteor::EAttackKey::ATTACK_UP);
	U_D_AttackKeys.Push(Meteor::EAttackKey::ATTACK_DOWN);

	TArray<Meteor::EAttackKey> L_U_R_AttackKeys;
	L_U_R_AttackKeys.Push(Meteor::EAttackKey::ATTACK_LEFT);
	L_U_R_AttackKeys.Push(Meteor::EAttackKey::ATTACK_UP);
	L_U_R_AttackKeys.Push(Meteor::EAttackKey::ATTACK_RIGHT);

	if (KeyClickArray.IsKeysDown(currentTime, 0.5f, U_U_AttackKeys))
	{
		//UE_LOG(LogTemp, Warning, TEXT("Up-Up"));
	}

	if (bIsAttackKeyClicked)
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

	PostTick(dt);
}

void AMeteorCharacter::PostTick(float dt)
{
	CurrentClickInfo.Clear();
	bIsAttackKeyClicked = false;
	bIsJumpKeyClicked = false;
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

void AMeteorCharacter::Turn(float Value)
{
	if (bEnableMove)
	{
		AddControllerYawInput(Value);

		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		SetActorRotation(YawRotation);
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
	bIsAttackKeyClicked = true;
}

void AMeteorCharacter::MoveForward(float Value)
{
	if (bEnableMove)
	{
		if ((Controller != NULL) && (Value != 0.0f))
		{
			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);
			const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
			
			FVector Loc = GetActorLocation();
			Loc += (1.0f) * Direction * Value;
			SetActorLocation(Loc);
		}
	}
}

void AMeteorCharacter::MoveRight(float Value)
{
	if (bEnableMove)
	{
		if ((Controller != NULL) && (Value != 0.0f))
		{
			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);
			const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
			
			FVector Loc = GetActorLocation();
			Loc += (1.0f) * Direction * Value;
			SetActorLocation(Loc);
		}
	}
}
