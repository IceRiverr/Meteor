// Copyright IceRiver. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MeteorGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class METEOR_API UMeteorGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UFUNCTION(Exec)
	void Host();

	UFUNCTION(Exec)
	void Join(const FString& Address);

	UFUNCTION(Exec)
	void JoinLocal();

	UFUNCTION(Exec)
	void ExitGame();
	
};
