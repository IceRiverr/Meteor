// Copyright IceRiver. All Rights Reserved.

#include "AttackCharacter.h"

#include "Animation/AnimInstance.h"
#include "Engine.h"
#include "GameFramework/Character.h"
#include "Components/BoxComponent.h"

#include "Meteor.h"
#include "Meteor/Common/MeteorFuncLib.h"
#include "Meteor/Components/InputBufferComponent.h"
#include "Meteor/Components/InputCommandComponent.h"
#include "Meteor/Components/CombatSystemComponent.h"

AAttackCharacter::AAttackCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	MoveFwdSpeedFactor = 1.0f;

	MoveBwdSpeedFactor = 0.3f;

	MoveRightSpeedFactor = 0.3f;

	bAttackBoxActiveFlag = false;
	bHasAttackOne = false;

	InputCommandCP = CreateDefaultSubobject<UInputCommandComponent>(TEXT("InputCommand"));
	AddTickPrerequisiteComponent(InputCommandCP);

	CombatSystemCP = CreateDefaultSubobject<UCombatSystemComponent>(TEXT("CombatSystem"));
	AddTickPrerequisiteComponent(CombatSystemCP);
	CombatSystemCP->SetInputCommandComponent(InputCommandCP);
}

void AAttackCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AAttackCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	TArray<UActorComponent*> Comps = GetComponentsByClass(UBoxComponent::StaticClass());
	for (int i = 0; i < Comps.Num(); ++i)
	{
		UBoxComponent* HitBox = Cast<UBoxComponent>(Comps[i]);
		if (HitBox && HitBox->GetCollisionObjectType() == HitBoxObjectChannel)
		{
			HitBoxCPs.Add(HitBox);
			HitBoxNames.Add(HitBox->GetName());
		}
	}
	HitBoxActives.SetNum(HitBoxCPs.Num());
	HitBoxLastLocations.SetNum(HitBoxCPs.Num());

	CancleAttackBoxs();
}

void AAttackCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bAttackBoxActiveFlag && !bHasAttackOne)
	{
		AttackBoxesTrace();
	}

	for (int i = 0; i < HitBoxCPs.Num(); ++i)
	{
		HitBoxLastLocations[i] = HitBoxCPs[i]->GetComponentLocation();
	}
}

void AAttackCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AAttackCharacter::OnJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AAttackCharacter::StopJump);

	PlayerInputComponent->BindAction("Guard", IE_Pressed, InputCommandCP, &UInputCommandComponent::OnGuard);
	PlayerInputComponent->BindAction("Guard", IE_Released, InputCommandCP, &UInputCommandComponent::StopGuard);

	PlayerInputComponent->BindAxis("MoveForward", this, &AAttackCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AAttackCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &AAttackCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &AAttackCharacter::LookUp);
}

float AAttackCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	FVector Forward = GetActorForwardVector();

	FHitResult HitResult;
	FVector ImpulseDir;
	DamageEvent.GetBestHitInfo(this, DamageCauser, HitResult, ImpulseDir);
	ImpulseDir.Normalize();

	FVector ToHitPoint = HitResult.ImpactPoint - GetActorLocation();
	ToHitPoint = FVector(ToHitPoint.X, ToHitPoint.Y, 0.0f);
	ToHitPoint.Normalize();

	float DotValue = FVector::DotProduct(Forward, ToHitPoint);
	FVector CrossValue = FVector::CrossProduct(Forward, ToHitPoint);
	float UpAngleDot = FVector::DotProduct(CrossValue, GetActorUpVector());
	float LeftRightFactor = (UpAngleDot > 0.0f ? 1.0f : -1.0f) * CrossValue.Size();

	if (DotValue > 0.707107)
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Red, FString::Printf(TEXT("Hit by FWD")));
	}
	else if (DotValue < -0.707107)
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Red, FString::Printf(TEXT("Hit by BWD")));
	}
	else if (LeftRightFactor > 0.707107)
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Red, FString::Printf(TEXT("Hit by RIGHT")));
	}
	else if (LeftRightFactor < -0.707107)
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Red, FString::Printf(TEXT("Hit by LEFT")));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Red, FString::Printf(TEXT("Hit by Undecide Direction")));
	}

	return Damage;
}

void AAttackCharacter::MoveForward(float Value)
{
	if (CombatSystemCP->GetMoveType() == MOVE_TYPE::MOVE_IDLE || CombatSystemCP->GetMoveType() == MOVE_TYPE::MOVE_JUMP)
	{
		if (Value > 0.0f)
			Value *= MoveFwdSpeedFactor;
		else
			Value *= MoveBwdSpeedFactor;

		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AAttackCharacter::MoveRight(float Value)
{
	if (CombatSystemCP->GetMoveType() == MOVE_TYPE::MOVE_IDLE || CombatSystemCP->GetMoveType() == MOVE_TYPE::MOVE_JUMP)
	{
		AddMovementInput(GetActorRightVector(), Value * MoveRightSpeedFactor);
	}
}

void AAttackCharacter::Turn(float Value)
{
	if (CombatSystemCP->GetMoveType() == MOVE_TYPE::MOVE_IDLE || CombatSystemCP->GetMoveType() == MOVE_TYPE::MOVE_JUMP)
	{
		AddControllerYawInput(Value);
	}
}

void AAttackCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

void AAttackCharacter::OnJump()
{
	if (CombatSystemCP->GetMoveType() == MOVE_TYPE::MOVE_IDLE)
	{
		InputCommandCP->bJumpKeyDown = true;
		ACharacter::Jump();
	}
}

void AAttackCharacter::StopJump()
{
	InputCommandCP->bJumpKeyDown = false;
	ACharacter::StopJumping();
}

void AAttackCharacter::ActiveAttackBoxs(int PoseAttackIndex)
{
	bAttackBoxActiveFlag = true;
	bHasAttackOne = false;
	HitBoxActives.SetNum(HitBoxCPs.Num());
	for (int i = 0; i < HitBoxActives.Num(); ++i)
	{
		HitBoxActives[i] = false;
	}
	
	if (CombatSystemCP->CurrentActionState && CombatSystemCP->CurrentActionState->GetMoveType() == MOVE_TYPE::MOVE_ATTACK && PoseAttackIndex != -1)
	{
		TArray<int> AttackBoxs = CombatSystemCP->GetPoseAttackBones(CombatSystemCP->CurrentActionState->GetPoseIndex(), PoseAttackIndex);
		for (int i = 0; i < AttackBoxs.Num(); ++i)
		{
			HitBoxActives[AttackBoxs[i]] = true;
		}
	}

	// For Debug
	FString AttackBonesDisplay;
	AttackBonesDisplay.Append("AttackBones: ").Append(FString::FromInt(CombatSystemCP->CurrentActionState->GetPoseIndex())).Append(".").Append(FString::FromInt(PoseAttackIndex)).Append("|");
	for (int i = 0; i < HitBoxActives.Num(); ++i)
	{
		if (HitBoxActives[i])
		{
			AttackBonesDisplay.Append(HitBoxNames[i]);
		}
	}
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, FString::Printf(TEXT("%s"), *AttackBonesDisplay));
}

void AAttackCharacter::CancleAttackBoxs()
{
	bAttackBoxActiveFlag = false;
	bHasAttackOne = false;
	HitBoxActives.SetNum(HitBoxCPs.Num());
	for (int i = 0; i < HitBoxActives.Num(); ++i)
	{
		HitBoxActives[i] = false;
	}
}

void AAttackCharacter::AttackBoxesTrace()
{
	for (int i = 0; i < HitBoxActives.Num(); ++i)
	{
		if (HitBoxActives[i])
		{
			UBoxComponent* AttackBox = HitBoxCPs[i];
			FHitResult HitResult;
			
			if (AttackBox)
			{
				FVector Start = AttackBox->GetComponentLocation();
				FVector Vel = Start - HitBoxLastLocations[i];
				FVector End = Start + Vel;

				FVector HalfSize = AttackBox->GetScaledBoxExtent();
				FRotator Rotator = AttackBox->GetComponentRotation();
				TArray<AActor*> ActorsToIgonre;
				ActorsToIgonre.Add(this);

				if (UKismetSystemLibrary::BoxTraceSingleByProfile(
					this, Start, End, HalfSize, Rotator, "HitBoxCheck", false, ActorsToIgonre, EDrawDebugTrace::ForOneFrame,
					HitResult, true, FLinearColor::Red, FLinearColor::Green, 0.05f))
				{
					GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, FString::Printf(TEXT("%s Hit %s"), *AttackBox->GetName(), *HitResult.GetActor()->GetName()));
					//DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 10.0f, 4, FColor::Blue);
					bHasAttackOne = true;
					
					UGameplayStatics::ApplyPointDamage(HitResult.GetActor(), 10.0f, HitResult.ImpactNormal, HitResult, nullptr, this, nullptr);
					
					return;
				}
			}
		}
	}
}
