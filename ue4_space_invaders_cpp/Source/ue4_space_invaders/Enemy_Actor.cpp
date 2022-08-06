// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy_Actor.h"

#include "BulletActor.h"
#include "Player_Pawn.h"

// Sets default values
AEnemy_Actor::AEnemy_Actor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEnemy_Actor::BeginPlay()
{
	Super::BeginPlay();
	shootTimer = FMath::RandRange(3, 30);
	startPos = GetActorLocation();
}

// Called every frame
void AEnemy_Actor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	timer +=  1.0f * DeltaTime;

	if(timer >= shootTimer)
	{
		timer = 0.0f;
		GetWorld()->SpawnActor<ABulletActor>(PusherBP, GetActorLocation(), GetActorRotation());
	}

	if(GetActorLocation().X > startPos.X + 80.0f && goLeft == false)
	{
		speed *= -1;
		goLeft = true;
	}
	else if(GetActorLocation().X < startPos.X && goLeft == true)
	{
		speed *= -1;
		goLeft = false;
	}

	moveDir = GetActorLocation().X + speed * DeltaTime;
	
	SetActorLocation(FVector(moveDir, GetActorLocation().Y, GetActorLocation().Z));	
}

void AEnemy_Actor::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	
	// in my wisdom I tagged the root component instead of the class. normally this would be just OtherActor->ActorHasTag
	if(OtherActor->GetRootComponent()->ComponentHasTag(FName(TEXT("Laser"))) == true)
	{
		GLog->Log("Enemy Laser!");

		// theres probably a better way to access the player ... however, it works.
		for (TActorIterator<APlayer_Pawn> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			APlayer_Pawn* Player = *ActorItr;
			Player->SetScore(100);	
			Player->UpdateScore();	
		}
		
		OtherActor->Destroy();
		GetWorld()->SpawnActor<AActor>(ExplosionBP, GetActorLocation(), GetActorRotation());
		Destroy();
	}
}

