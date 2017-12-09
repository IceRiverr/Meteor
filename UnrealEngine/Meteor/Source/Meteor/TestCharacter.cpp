// Fill out your copyright notice in the Description page of Project Settings.

#include "TestCharacter.h"
#include "Meteor.h"
#include "Animation/AnimInstance.h"
#include "FPoseInputTable.h"

// Sets default values
ATestCharacter::ATestCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ResetMontageState();
	
	transationTime = 0.0f;
	bStartTransform = false;
	Pose315_NextPose_Time = 2.3f;


}

// Called when the game starts or when spawned
void ATestCharacter::BeginPlay()
{
	Super::BeginPlay();

	FPoseInputTable* PoseInfo = dataTable->FindRow<FPoseInputTable>(TEXT("295"), "");
	StreamMgr.LoadSynchronous(PoseInfo->PoseMontage);

	PoseInfo = dataTable->FindRow<FPoseInputTable>(TEXT("297"), "");
	StreamMgr.LoadSynchronous(PoseInfo->PoseMontage);

	//if (dataTable)
	//{
	//	//TArray<FStringAssetReference> MontageToLoad;

	//	for (auto i : dataTable->RowMap)
	//	{
	//		FPoseInputTable* PoseInfo = (FPoseInputTable*)(i.Value);
	//		UE_LOG(LogTemp, Warning, TEXT("Row %i %s %s"), PoseInfo->PoseID, *(PoseInfo->PoseName), *(PoseInfo->PoseInputKey));

	//		if (PoseInfo->PoseMontage)
	//		{
	//			test_montage = StreamMgr.LoadSynchronous(PoseInfo->PoseMontage);
	//		}

	//		//if (PoseInfo->PoseMontage)
	//		//{
	//			//MontageToLoad.AddUnique(PoseInfo->PoseMontage.ToStringReference());
	//		//}
	//	}
	//}

	//	//StreamMgr.RequestAsyncLoad(MontageToLoad);
	//}
}

// Called every frame
void ATestCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	static int counter = 0;

	if (bAttack)
	{
		counter++;
		FString tt = "Attack";
		//UE_LOG(LogTemp, Warning, TEXT("%s: %i %f"), *tt, counter, DeltaTime);
	}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	UAnimMontage* currentActiveMontage = AnimInstance->GetCurrentActiveMontage();

	FPoseInputTable* PoseInfo = dataTable->FindRow<FPoseInputTable>(TEXT("297"), "");
	UAnimMontage* montage321 = PoseInfo->PoseMontage.Get();

	if (montage321)
	{
		// 有点作弊
		if (currentActiveMontage == AM_Pose315 && currentToEndMontageName == "Pose_315_Montage" && !bStartTransform)
		{
			UE_LOG(LogTemp, Warning, TEXT("AM_Pose315 Blend Out"));

			currentToEndMontageName == "";

			// 这种方法是将上一个Montage结束时开始，和跳转的Montage进行blend
			//AnimInstance->Montage_Stop(0.0);

			tmpBlend = montage321->BlendIn;
			montage321->BlendIn.SetBlendTime(Pose315_NextPose_Time);
			AnimInstance->Montage_Play(montage321, 0.0f);
			AnimInstance->Montage_JumpToSection("Action", montage321);
			//AnimInstance->Montage_Pause(montage321);

			bStartTransform = true;
			transationTime = 0.0f;
		}

		if (bStartTransform)
		{
			transationTime += DeltaTime;

			if (transationTime > Pose315_NextPose_Time)
			{
				bStartTransform = false;
				montage321->BlendIn.SetBlendTime(tmpBlend.GetBlendTime());
				AnimInstance->Montage_SetPlayRate(montage321, 1.0f);
				//AnimInstance->Montage_SetPosition(montage321, 0.5);
			}
		}
	}
}

// Called to bind functionality to input
void ATestCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	//Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &ATestCharacter::OnAttack);
	PlayerInputComponent->BindAction("Attack", IE_Released, this, &ATestCharacter::StopAttack);
}

void ATestCharacter::OnAttack()
{
	bAttack = true;
	//if (AM_Pose315 != NULL)
	//{
	//	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	//	if (AnimInstance != NULL)
	//	{
	//		UAnimMontage* currentActiveMontage = AnimInstance->GetCurrentActiveMontage();
	//		//AnimInstance->Montage_Stop(0.0, AM_Pose321);
	//		AnimInstance->Montage_Play(AM_Pose315);
	//	}
	//}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		FPoseInputTable* PoseInfo = dataTable->FindRow<FPoseInputTable>(TEXT("295"), "");
		if (PoseInfo)
		{
			UAnimMontage* mtg295 = PoseInfo->PoseMontage.Get();
			if (mtg295)
			{
				AnimInstance->Montage_Play(mtg295);
			}
		}
	}
}

void ATestCharacter::StopAttack()
{
	bAttack = false;
}

void ATestCharacter::ResetMontageState()
{
	bPose315BlendOut = false;
	bPose317BlendOut = false;
	bPose321BlendOut = false;
}

void ATestCharacter::SetBlendOut_MontageName(FString name)
{
	currentToEndMontageName = name;
}

void ATestCharacter::MyMontageBlendOut(UAnimMontage* Montage, bool bInterrupted)
{

}
