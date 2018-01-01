// Copyright IceRiver. All Rights Reserved.

#include "InputCommamdComponent.h"
#include "Common/MeteorSingletonLibrary.h"

// Sets default values for this component's properties
UInputCommamdComponent::UInputCommamdComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UInputCommamdComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UInputCommamdComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	/*UMeteorSingleton* Singleton = UMeteorSingletonLibrary::GetMeteorSingleton();
	if (Singleton)
	{
		Singleton->FrameCount++;
		UE_LOG(LogTemp, Warning, TEXT("InputBufferTick: %d"), Singleton->FrameCount);
	}

	if (GetWorld())
	{
		const APlayerController* PlayerControler = GetWorld()->GetFirstPlayerController();
		if (PlayerControler)
		{
			const TArray<FInputActionKeyMapping>& InputActionMappings = PlayerControler->PlayerInput->GetKeysForAction("Up");

			if (InputActionMappings.Num() > 0)
			{
				const FInputActionKeyMapping& Mapping = InputActionMappings[0];
				bool bUpKeyDown = PlayerControler->IsInputKeyDown(Mapping.Key);
				UE_LOG(LogTemp, Warning, TEXT("InputKeyDown: Up, %d"), (int)bUpKeyDown);
			}
		}
	}*/

}

