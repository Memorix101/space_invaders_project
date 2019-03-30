// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Image.h"

#include "SplashScreen_UserWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE4_SPACE_INVADERS_API USplashScreen_UserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	USplashScreen_UserWidget(const FObjectInitializer& ObjectInitializer);

	// Optionally override the Blueprint "Event Construct" event
	virtual void NativeConstruct() override;

	// Optionally override the tick event
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

private:
	bool MaxOpcaity = false;
	float FadeTimer = 0.0f;
};
