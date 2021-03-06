
#include "AttackCharacter.h"
#include "Meteor.h"
#include "Animation/AnimInstance.h"
#include "FPoseInputTable.h"
#include "MyAnimMetaData.h"

// Sets default values
AAttackCharacter::AAttackCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	transationTime = 0.0f;
	bStartTransform = false;
	Pose315_NextPose_Time = 2.3f;

	CurrentPoseIndex = 295;

	IsInTranslation = false;

	bAcceptInput = false;

	NextPoseTime = 0.3f;

	AttackState = ATTACK_STATE::ATK_IDLE;
}

// Called when the game starts or when spawned
void AAttackCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (PoseInfoTable)
	{
		FPoseInputTable* PoseInfo = nullptr;

		PoseInfo = PoseInfoTable->FindRow<FPoseInputTable>(TEXT("295"), "");
		StreamMgr.LoadSynchronous(PoseInfo->PoseMontage);

		PoseInfo = PoseInfoTable->FindRow<FPoseInputTable>(TEXT("296"), "");
		StreamMgr.LoadSynchronous(PoseInfo->PoseMontage);

		PoseInfo = PoseInfoTable->FindRow<FPoseInputTable>(TEXT("297"), "");
		StreamMgr.LoadSynchronous(PoseInfo->PoseMontage);
	}

	// 进行循环测试
	TmpPoseTranslationTable.Add(295, 296);
	TmpPoseTranslationTable.Add(296, 297);
}

// Called every frame
void AAttackCharacter::Tick2(float DeltaTime)
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
	FName sectionName = AnimInstance->Montage_GetCurrentSection(currentActiveMontage);

	// 设置播放速率
	bool* hasSet = SectionRatioHasSet.Find(sectionName);
	if (IsInTranslation == false)
	{
		if (hasSet == nullptr || *hasSet == false)
		{
			SectionRatioHasSet.Add(sectionName, true);
			float* ratio = SectionRatioCache.Find(sectionName);
			if (ratio)
			{
				UE_LOG(LogTemp, Warning, TEXT("%s %s: play rate %f"), *(currentActiveMontage->GetName()), *(sectionName.ToString()), *ratio);
				AnimInstance->Montage_SetPlayRate(currentActiveMontage, *ratio);
			}
		}
	}
	
	// 确定是否需要跳转
	if (bAcceptInput == false && bAttack)
	{
		if (sectionName.IsEqual(NextPoseOut))
		{
			int32* nextPoseIndex = TmpPoseTranslationTable.Find(CurrentPoseIndex);
			if (nextPoseIndex)
			{
				CurrentPoseIndex = *nextPoseIndex;
				UAnimMontage* poseMtg = GetPoseMontage(CurrentPoseIndex);
				if (poseMtg)
				{
					GetAnimMetaData(poseMtg);

					// 执行实际的过渡
					tmpBlend = poseMtg->BlendIn;
					poseMtg->BlendIn.SetBlendTime(NextPoseTime);
					AnimInstance->Montage_Play(poseMtg, 0.0f);
					AnimInstance->Montage_JumpToSection(NextPoseIn, poseMtg);

					IsInTranslation = true;
					bAttack = false;

					auto lamda = [&]()->void {
						GWorld->GetTimerManager().ClearTimer(NextPoseTransitionTimer);
						IsInTranslation = false;
					};

					auto dlg = FTimerDelegate::CreateLambda(lamda);
					GWorld->GetTimerManager().SetTimer(NextPoseTransitionTimer, dlg, NextPoseTime, false);
				}
			}
		}
	}

	/*UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	UAnimMontage* currentActiveMontage = AnimInstance->GetCurrentActiveMontage();

	FPoseInputTable* PoseInfo = dataTable->FindRow<FPoseInputTable>(TEXT("297"), "");
	UAnimMontage* montage321 = PoseInfo->PoseMontage.Get();*/

	//if (montage321)
	//{
	//	// 有点作弊
	//	if (currentActiveMontage == AM_Pose315 && currentToEndMontageName == "Pose_315_Montage" && !bStartTransform)
	//	{
	//		UE_LOG(LogTemp, Warning, TEXT("AM_Pose315 Blend Out"));

	//		currentToEndMontageName == "";

	//		// 这种方法是将上一个Montage结束时开始，和跳转的Montage进行blend
	//		//AnimInstance->Montage_Stop(0.0);

	//		tmpBlend = montage321->BlendIn;
	//		montage321->BlendIn.SetBlendTime(Pose315_NextPose_Time);
	//		AnimInstance->Montage_Play(montage321, 0.0f);
	//		AnimInstance->Montage_JumpToSection("Action", montage321);
	//		//AnimInstance->Montage_Pause(montage321);

	//		bStartTransform = true;
	//		transationTime = 0.0f;
	//	}

	//	if (bStartTransform)
	//	{
	//		transationTime += DeltaTime;

	//		if (transationTime > Pose315_NextPose_Time)
	//		{
	//			bStartTransform = false;
	//			montage321->BlendIn.SetBlendTime(tmpBlend.GetBlendTime());
	//			AnimInstance->Montage_SetPlayRate(montage321, 1.0f);
	//			//AnimInstance->Montage_SetPosition(montage321, 0.5);
	//		}
	//	}
	//}
}

void AAttackCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance == nullptr)
		return;

	switch (AttackState)
	{
	case AAttackCharacter::ATK_IDLE:
	{
		if (bAttackKeyDown)
		{
			ConsumeInputKey();
			CurrentPoseIndex = 295;
			UAnimMontage* poseMtg = GetPoseMontage(CurrentPoseIndex);
			if (poseMtg)
			{
				GetAnimMetaData(poseMtg);
				AnimInstance->Montage_Play(poseMtg);
				AttackState = ATK_PLAYING;
			}
		}
		break;
	}
	case AAttackCharacter::ATK_PLAYING:
	{
		UAnimMontage* ActiveMontage = AnimInstance->GetCurrentActiveMontage();
		if (ActiveMontage)
		{
			FName sectionName = AnimInstance->Montage_GetCurrentSection(ActiveMontage);

			// 设置playRate
			float* ratio = SectionRatioCache.Find(sectionName);
			if (ratio)
			{
				//UE_LOG(LogTemp, Warning, TEXT("%s %s: play rate %f"), *(ActiveMontage->GetName()), *(sectionName.ToString()), *ratio);
				AnimInstance->Montage_SetPlayRate(ActiveMontage, *ratio);
			}

			// 检测是否需要跳转
			if (sectionName.IsEqual(NextPoseOut) && bAttackKeyDown)
			{
				ConsumeInputKey();

				int32* nextPoseIndex = TmpPoseTranslationTable.Find(CurrentPoseIndex);
				if (nextPoseIndex)
				{
					CurrentPoseIndex = *nextPoseIndex;
					UAnimMontage* poseMtg = GetPoseMontage(CurrentPoseIndex);
					if (poseMtg)
					{
						GetAnimMetaData(poseMtg);

						// 执行实际的过渡
						tmpBlend = poseMtg->BlendIn;
						poseMtg->BlendIn.SetBlendTime(NextPoseTime);
						AnimInstance->Montage_Play(poseMtg, 0.0f);
						AnimInstance->Montage_JumpToSection(NextPoseIn, poseMtg);

						auto lamda = [&]()->void {
							GWorld->GetTimerManager().ClearTimer(NextPoseTransitionTimer);
							AttackState = ATK_PLAYING;
						};

						auto dlg = FTimerDelegate::CreateLambda(lamda);
						GWorld->GetTimerManager().SetTimer(NextPoseTransitionTimer, dlg, NextPoseTime, false);

						AttackState = ATK_NEXTPOSE;
					}
				}
			}
		}
		else// 如果当前动画已经播完，则进入Idle状态
		{
			AttackState = ATK_IDLE;
		}
		break;
	}
	case AAttackCharacter::ATK_NEXTPOSE:
	{
		break;
	}
	default:
		break;
	}
}

// Called to bind functionality to input
void AAttackCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	//Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &AAttackCharacter::OnAttackV2);
	PlayerInputComponent->BindAction("Attack", IE_Released, this, &AAttackCharacter::StopAttack);
}

void AAttackCharacter::OnAttack()
{
	if (CanAttack())
	{
		bAttack = true;
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			UAnimMontage* currentActiveMontage = AnimInstance->GetCurrentActiveMontage();
			if (currentActiveMontage)
			{
				// 如果当前的输入在BlendOutSection中则直接跳转
				FName sectionName = AnimInstance->Montage_GetCurrentSection(currentActiveMontage);
				if (sectionName.IsEqual(NextPoseOut))
				{
					int32* nextPoseIndex = TmpPoseTranslationTable.Find(CurrentPoseIndex);
					if (nextPoseIndex)
					{
						CurrentPoseIndex = *nextPoseIndex;
						UAnimMontage* poseMtg = GetPoseMontage(CurrentPoseIndex);
						if (poseMtg)
						{
							GetAnimMetaData(poseMtg);

							// 执行实际的过渡
							tmpBlend = poseMtg->BlendIn;
							poseMtg->BlendIn.SetBlendTime(NextPoseTime);
							AnimInstance->Montage_Play(poseMtg, 0.0f);
							AnimInstance->Montage_JumpToSection(NextPoseIn, poseMtg);

							IsInTranslation = true;
							bAttack = false;

							auto lamda = [&]()->void {
								GWorld->GetTimerManager().ClearTimer(NextPoseTransitionTimer);
								IsInTranslation = false;
							};

							auto dlg = FTimerDelegate::CreateLambda(lamda);
							GWorld->GetTimerManager().SetTimer(NextPoseTransitionTimer, dlg, NextPoseTime, false);
						}
					}
				}
			}
			else
			{
				CurrentPoseIndex = 295;
				UAnimMontage* poseMtg = GetPoseMontage(CurrentPoseIndex);
				if (poseMtg)
				{
					GetAnimMetaData(poseMtg);
					AnimInstance->Montage_Play(poseMtg);
				}
			}
		}
	}
}

void AAttackCharacter::StopAttack()
{
	//bAttack = false;
}

void AAttackCharacter::OnAttackV2()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		UAnimMontage* activeMontage = AnimInstance->GetCurrentActiveMontage();
		if (activeMontage && bAcceptInput)
		{
			bAttackKeyDown = true;
		}
		else if (activeMontage == nullptr)
		{
			bAttackKeyDown = true;
		}
	}
}

UAnimMontage* AAttackCharacter::GetPoseMontage(int32 poseIndex)
{
	FString poseStr = FString::FromInt(poseIndex);

	if (PoseInfoTable)
	{
		FPoseInputTable* PoseInfo = PoseInfoTable->FindRow<FPoseInputTable>(FName(*poseStr), "");
		if (PoseInfo)
		{
			UAnimMontage* poseMtg = PoseInfo->PoseMontage.Get();
			if (poseMtg)
			{
				return poseMtg;
				UE_LOG(LogTemp, Warning, TEXT("Get %s Succeed!"), *(poseMtg->GetName()));
			}
		}
	}
	return nullptr;
}

float AAttackCharacter::GetSectionPlayRate(UAnimMontage* montage, FName sectionName)
{
	if (montage)
	{
		const TArray<UAnimMetaData*> MetaDatas = montage->GetMetaData();
		for (int i = 0; i < MetaDatas.Num(); ++i)
		{
			UAnimMetaData_SectionInfo* sectionInfo = Cast<UAnimMetaData_SectionInfo>(MetaDatas[i]);
			if (sectionInfo)
			{
				float* speed = sectionInfo->SectionSpeeds.Find(sectionName);
				
				UE_LOG(LogTemp, Warning, TEXT("%s %s: play rate %f"), *(montage->GetName()), *(sectionName.ToString()), *speed);
				
				return *speed;
			}
		}
	}
	return 1.0f;
}

bool AAttackCharacter::CanAttack()
{
	if (bAcceptInput)
		return true;
	
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		UAnimMontage* ActiveMontage = AnimInstance->GetCurrentActiveMontage();
		if (ActiveMontage && bAcceptInput)
		{
			UE_LOG(LogTemp, Warning, TEXT("Can't Attack Now"));
			return true;
		}
	}
	return true;
}

void AAttackCharacter::GetAnimMetaData(UAnimMontage* montage)
{
	if (montage)
	{
		SectionRatioCache.Empty(4);
		SectionRatioHasSet.Empty(4);
		const TArray<UAnimMetaData*> MetaDatas = montage->GetMetaData();
		for (int i = 0; i < MetaDatas.Num(); ++i)
		{
			UAnimMetaData_SectionInfo* sectionInfo = Cast<UAnimMetaData_SectionInfo>(MetaDatas[i]);
			if (sectionInfo)
			{
				SectionRatioCache = sectionInfo->SectionSpeeds;
				NextPoseIn = sectionInfo->NextPoseIn;
				NextPoseOut = sectionInfo->NextPoseOut;
				break;
			}
		}
	}
}

bool AAttackCharacter::ConsumeInputKey()
{
	bool lastInput = bAttackKeyDown;
	bAttackKeyDown = false;
	return lastInput;
}
