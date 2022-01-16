// Fill out your copyright notice in the Description page of Project Settings.


#include "SplashScreen_Actor.h"

// Sets default values
ASplashScreen_Actor::ASplashScreen_Actor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASplashScreen_Actor::BeginPlay()
{
	Super::BeginPlay();
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Some debug message!"));

	//FCommandLine::Set(TEXT("r.SetRes 640x480w"));
	GEngine->GameUserSettings->SetFullscreenMode(EWindowMode::Windowed);
	GEngine->GameUserSettings->SetScreenResolution(FIntPoint(640, 480));
	GEngine->GameUserSettings->ApplySettings(true);
	
	if (wSplashScreenWidget)
	{
		SplashScreenWidget = CreateWidget<UUserWidget>(GetWorld(), wSplashScreenWidget);
		if (wSplashScreenWidget)
		{
			SplashScreenWidget->AddToViewport();
		}
	}	
}

// Called every frame
void ASplashScreen_Actor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

