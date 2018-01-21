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

	InputCommandCP = CreateDefaultSubobject<UInputCommandComponent>(TEXT("InputCommand"));
	AddTickPrerequisiteComponent(InputCommandCP);

	CombatSystemCP = CreateDefaultSubobject<UCombatSystemComponent>(TEXT("CombatSystem"));
	AddTickPrerequisiteComponent(CombatSystemCP);
	CombatSystemCP->SetInputCommandComponent(InputCommandCP);

	/*bau_Head_Test = CreateDefaultSubobject<UBoxComponent>(TEXT("bau_Head_Test"));
	bau_Head_Test->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, "bau_Head");
	bau_Head_Test->SetBoxExtent(FVector(12.0f, 10.0f, 10.0f));
	bau_Head_Test->SetRelativeLocation(FVector(6.0f, 0.0f, 0.0f));
	bau_Head_Test->bGenerateOverlapEvents = true;
	bau_Head_Test->SetCollisionObjectType(HitBoxObjectChannel);
	bau_Head_Test->SetCollisionProfileName("HitBoxCheck");*/
}

void AAttackCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AAttackCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	WeaponTraceV1();
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

void AAttackCharacter::Test_createSection(UAnimMontage* montage)
{
	if (montage)
	{
		/*int frames = montage->GetNumberOfFrames();
		float step = frames / 30.0f;
		montage->AddAnimCompositeSection("Act1", 0.0f);
		montage->AddAnimCompositeSection("Act2", 0.1f * step);
		montage->AddAnimCompositeSection("Act3", 0.5f * step);
		montage->AddAnimCompositeSection("Act4", 0.8f * step);
		montage->DeleteAnimCompositeSection(0);*/

		//UAnimNotify_PlaySound* playSnd = CreateDefaultSubobject<UAnimNotify_PlaySound>(TEXT("playSnd"));
	}
}

void AAttackCharacter::Play_TestMontage()
{
	if (TestMtg)
	{
		GetMesh()->GetAnimInstance()->Montage_Play(TestMtg);
	}
}

void AAttackCharacter::OnOverlapBegin_Implementation(UPrimitiveComponent* Comp, 
	AActor* otherActor, UPrimitiveComponent* otherComp, int otherBodyIndex, 
	bool bFromSweep, const FHitResult& SweepResult)
{
	if (Comp->GetCollisionObjectType() == HitBoxObjectChannel && otherComp->GetCollisionObjectType() == HitBoxObjectChannel &&
		otherActor != this && HitBoxCPs.Contains(otherComp) == false)
	{
		UBoxComponent* HitBoxCom = Cast<UBoxComponent>(Comp);
		int CompIndex = -1;
		if (HitBoxCom && HitBoxCPs.Find(HitBoxCom, CompIndex))
		{
			if (HitBoxActives[CompIndex])
			{
				FString DisplayStr;
				DisplayStr.Append(Comp->GetName()).Append(" Hit: ").Append(otherActor->GetName()).Append("|").Append(otherComp->GetName());
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("%s"), *DisplayStr));
			}
		}
	}
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
	CancleAttackBoxs();

	for (int i = 0; i < HitBoxCPs.Num(); ++i)
	{
		HitBoxCPs[i]->OnComponentBeginOverlap.AddDynamic(this, &AAttackCharacter::OnOverlapBegin);
	}

	int WeaponHitBoxIndex = -1;
	if (HitBoxNames.Find("weapon", WeaponHitBoxIndex))
	{
		WeaponHitBox = HitBoxCPs[WeaponHitBoxIndex];
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, FString::Printf(TEXT("--WEAPON--")));
	}
}

void AAttackCharacter::ActiveAttackBoxs(int PoseAttackIndex)
{
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
	HitBoxActives.SetNum(HitBoxCPs.Num());
	for (int i = 0; i < HitBoxActives.Num(); ++i)
	{
		HitBoxActives[i] = false;
	}
}

void AAttackCharacter::WeaponTraceV1()
{
	if (WeaponHitBox)
	{
		FVector Start = WeaponHitBox->GetComponentLocation();
		FVector Vel = Start - WeaponLastLocation;
		FVector End = Start + Vel * 1.0f; // For Test
		WeaponLastLocation = Start;

		FVector HalfSize = WeaponHitBox->GetScaledBoxExtent();
		FRotator Rotator = WeaponHitBox->GetComponentRotation();
		TArray<FHitResult> HitResults;
		TArray<AActor*> ActorsToIgonre;
		ActorsToIgonre.Add(this);

		if (UKismetSystemLibrary::BoxTraceMultiByProfile(
			this, Start, End, HalfSize, Rotator, "HitBoxCheck", false, ActorsToIgonre, EDrawDebugTrace::ForOneFrame, HitResults, true, FLinearColor::Red, FLinearColor::Green, 0.05f))
		{
			for (int i = 0; i < HitResults.Num(); ++i)
			{
				FHitResult& Hit = HitResults[i];

				//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, FString::Printf(TEXT("BoxTrace: %s"), *(HitResults[0].ToString())));
				DrawDebugSphere(GetWorld(), Hit.ImpactPoint, 10.0f, 4, FColor::Blue);
			}
		}
	}
}

void AAttackCharacter::WeaponTraceV2()
{
	// 上面的方法要是不能精确获取，则通过12根射线去模拟；
	// 然后获取击中位置的中心值；
}
