// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "Meteor.h"
#include "MeteorGameMode.h"
#include "MeteorCharacter.h"
#include "../UMG/Public/Blueprint/UserWidget.h"

AMeteorGameMode::AMeteorGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Meteor/BluePrints/BP_Meteor_Character"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void AMeteorGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (HUDWidgetClass != nullptr)
	{
		UUserWidget* HUDWidget = CreateWidget<UUserWidget>(GetWorld(), HUDWidgetClass);
		if (HUDWidget != nullptr)
		{
			HUDWidget->AddToViewport();
		}
	}
}
