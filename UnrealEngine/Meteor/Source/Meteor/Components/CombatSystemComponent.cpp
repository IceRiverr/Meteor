// Copyright IceRiver. All Rights Reserved.

#include "CombatSystemComponent.h"

UCombatSystemComponent::UCombatSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UCombatSystemComponent::BeginPlay()
{
	Super::BeginPlay();

	
}


void UCombatSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

