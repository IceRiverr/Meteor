#pragma once

#include "Animation/AnimMontage.h"
#include "GameFramework/Character.h"
#include "Engine/DataTable.h"
#include "Engine/StreamableManager.h"

#include "AttackCharacter.generated.h"

UCLASS()
class METEOR_API AAttackCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* CharacterMesh;

public:
	// Sets default values for this character's properties
	AAttackCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	
	void OnAttack();
	void StopAttack();

	UFUNCTION(BlueprintCallable, Category = "Test|Chui")
	void ResetMontageState();

	UFUNCTION(BlueprintCallable, Category = "Test|Chui")
	void SetBlendOut_MontageName(FString name);

	void MyMontageBlendOut(UAnimMontage* Montage, bool bInterrupted);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* AM_Pose315;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* AM_Pose317;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* AM_Pose321;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	bool bAttack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	bool bPose315BlendOut;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	bool bPose317BlendOut;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	bool bPose321BlendOut;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	TAssetPtr<UDataTable> dataTable;
private:
	FString currentToEndMontageName;

	FTimer timer;

	float transationTime;
	bool bStartTransform;
	FAlphaBlend tmpBlend;
	float Pose315_NextPose_Time;

	FStreamableManager StreamMgr;
};
