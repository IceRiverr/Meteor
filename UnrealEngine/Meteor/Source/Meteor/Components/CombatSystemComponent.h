// Copyright IceRiver. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Animation/AnimMontage.h"

#include "CombatSystemComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class METEOR_API UCombatSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCombatSystemComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sprint)
	UAnimMontage* SprintForwadMtg;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sprint)
	UAnimMontage* SprintBackwardMtg;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sprint)
	UAnimMontage* SprintRightMtg;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sprint)
	UAnimMontage* SprintLeftMtg;
};
