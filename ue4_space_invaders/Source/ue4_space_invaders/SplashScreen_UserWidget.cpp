// Fill out your copyright notice in the Description page of Project Settings.

#include "SplashScreen_UserWidget.h"
#include "Kismet/GameplayStatics.h"

USplashScreen_UserWidget::USplashScreen_UserWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void USplashScreen_UserWidget::NativeConstruct()
{
	this->SetRenderOpacity(0);

	// Call the Blueprint "Event Construct" node
	Super::NativeConstruct();
}

void USplashScreen_UserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{	
	// Make sure to call the base class's NativeTick function
	Super::NativeTick(MyGeometry, InDeltaTime);

	if(!MaxOpcaity)
		FadeTimer += 1 * InDeltaTime;
	else
		FadeTimer -= 1 * InDeltaTime;

	this->SetRenderOpacity(FadeTimer);

	if(FadeTimer >= 2.0f && !MaxOpcaity)
	{
		MaxOpcaity = true;
		
	}
	else if(FadeTimer <= -0.5f && MaxOpcaity)
	{
		UGameplayStatics::OpenLevel(this, FName("Game"), true);
	}	
}
