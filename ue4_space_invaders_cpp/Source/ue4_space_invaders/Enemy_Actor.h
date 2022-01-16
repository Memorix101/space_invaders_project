// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BulletActor.h"
#include "GameFramework/Actor.h"
#include "Enemy_Actor.generated.h"

UCLASS()
class UE4_SPACE_INVADERS_API AEnemy_Actor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemy_Actor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	UPROPERTY(EditAnywhere, Category = CPP)
	TSubclassOf<AActor> ExplosionBP;

	UPROPERTY(EditAnywhere, Category = CPP)
	TSubclassOf<ABulletActor> PusherBP;
	
	float moveDir = 0.0f;
	int shootTimer = 0.0f;
	float timer = 0.0f;
	bool goLeft = false;
	FVector startPos;
	float speed = 100.0f;
};
