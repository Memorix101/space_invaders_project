// Fill out your copyright notice in the Description page of Project Settings.


#include "ExplosionActor.h"

// Sets default values
AExplosionActor::AExplosionActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AExplosionActor::BeginPlay()
{
	Super::BeginPlay();
	
	TArray<UPaperFlipbookComponent*> Components;
    this->GetComponents<UPaperFlipbookComponent>(Components);
    Me = Components[0];

	if(Me)
	{
		Me->SetLooping(false);
	}
}

// Called every frame
void AExplosionActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	timer += DeltaTime;

	if(timer > 1.0f)
	{
		Destroy();
	}
}

