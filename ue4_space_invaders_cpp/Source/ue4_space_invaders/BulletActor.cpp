// Fill out your copyright notice in the Description page of Project Settings.


#include "BulletActor.h"

// Sets default values
ABulletActor::ABulletActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABulletActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABulletActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector myLocation = GetActorLocation();
	float v = myLocation.Z+speed*150.0f*GetWorld()->DeltaTimeSeconds;
	SetActorLocation(FVector(myLocation.X, myLocation.Y, v));

	if(myLocation.Z >= 300.0f)
	{
		Destroy();
	}
	else if(myLocation.Z <= -300.0f)
	{
		Destroy();
	}
}

