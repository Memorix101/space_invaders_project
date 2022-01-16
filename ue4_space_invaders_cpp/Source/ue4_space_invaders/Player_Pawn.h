// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnrealTypePrivate.h"
#include "GameFramework/Pawn.h"
#include "Runtime/UMG/Public/UMG.h"
#include "Blueprint/UserWidget.h"
#include "Player_Pawn.generated.h"

UCLASS()
class UE4_SPACE_INVADERS_API APlayer_Pawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APlayer_Pawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	void Move_XAxis(float AxisValue);
	void Fire();

	void SetScore(int v);
	void UpdateScore();
	void CheckForGamepad();

	UPROPERTY(EditAnywhere, Category = CPP)
	TSubclassOf<class UUserWidget> wHUDWidget;
	UUserWidget* HUDWidget;

	UPROPERTY(EditAnywhere, Category = CPP)
	TSubclassOf<class UUserWidget> wGameOver;
	UUserWidget* GameOverWidget;

	UPROPERTY(EditAnywhere, Category = CPP)
	TSubclassOf<AActor> ExplosionBP;

	UPROPERTY(EditAnywhere, Category=CPP)
	TSubclassOf<class ABulletActor> BulletActor;

	FIntProperty* ScoreField = nullptr;
	APlayerController* MyPlayerController;
	int score = 0;
	bool gamePadIsActive = false;
};
