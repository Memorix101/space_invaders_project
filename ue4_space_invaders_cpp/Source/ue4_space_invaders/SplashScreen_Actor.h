// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Runtime/UMG/Public/UMG.h"
#include "Blueprint/UserWidget.h"
#include "SplashScreen_Actor.generated.h"

UCLASS()
class UE4_SPACE_INVADERS_API ASplashScreen_Actor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASplashScreen_Actor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = CPP)
	TSubclassOf<class UUserWidget> wSplashScreenWidget;
	UUserWidget* SplashScreenWidget;
};
