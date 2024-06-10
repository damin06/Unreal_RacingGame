// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RacingGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class RACING_API ARacingGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UMG_Game")
    TSubclassOf<class UUserWidget> StartingWidgetClass;

    UPROPERTY()
    TObjectPtr<class UUserWidget> CurrentWidget;
public:
    UFUNCTION(BlueprintCallable, Category = "UMG_Game")
    void ChangeMenuWidget(TSubclassOf<class UUserWidget> NewWidgetClass);
};
