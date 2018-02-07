// Copyright IceRiver. All Rights Reserved.

#include "MeteorGameInstance.h"

#include "Engine/World.h"
#include "Engine/Engine.h"

void UMeteorGameInstance::Host()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(0, 2, FColor::Green, TEXT("Hosting..."));
	}

	UWorld* World = GetWorld();
	if (World)
	{
		World->ServerTravel("/Game/Test/AttackTest/SN05_2?listen");
	}
}

void UMeteorGameInstance::Join(const FString& Address)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(0, 10.0f, FColor::Green, FString::Printf(TEXT("Join %s..."), *Address));
	}

	APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (PlayerController)
	{
		PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
	}
}

void UMeteorGameInstance::JoinLocal()
{
	Join("127.0.0.1");
}

void UMeteorGameInstance::ExitGame()
{
	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (PlayerController)
		{
			PlayerController->ConsoleCommand("quit");
		}
	}
}
