// Copyright Epic Games, Inc. All Rights Reserved.


#include "RacingGameModeBase.h"
#include "Blueprint/UserWidget.h"

void ARacingGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	ChangeMenuWidget(StartingWidgetClass);
}

void ARacingGameModeBase::ChangeMenuWidget(TSubclassOf<UUserWidget> NewWidgetClass)
{
    if (CurrentWidget != nullptr)
    {
        CurrentWidget->RemoveFromViewport();
        CurrentWidget = nullptr;
    }

    if (NewWidgetClass != nullptr)
    {
        CurrentWidget = CreateWidget(GetWorld(), NewWidgetClass);
        if (CurrentWidget != nullptr)
        {
            CurrentWidget->AddToViewport();
        }
    }
}
