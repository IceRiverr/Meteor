// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/GameModeBase.h"
#include "MeteorGameMode.generated.h"

UCLASS(minimalapi)
class AMeteorGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMeteorGameMode();

	virtual void BeginPlay() override;
public:
	/** HUD class this game uses. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Meteor")
	TSubclassOf<class UUserWidget> HUDWidgetClass;
};

