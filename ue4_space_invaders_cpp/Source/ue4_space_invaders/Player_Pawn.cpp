// Fill out your copyright notice in the Description page of Project Settings.


#include "Player_Pawn.h"

#include "BulletActor.h"
#include "Enemy_Actor.h"

// Sets default values
APlayer_Pawn::APlayer_Pawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void APlayer_Pawn::SetScore(int v)
{
	score += v;
}

void APlayer_Pawn::UpdateScore()
{
	if (ScoreField != NULL)
	{
		ScoreField->SetPropertyValue_InContainer(HUDWidget, score);
	}
}

void APlayer_Pawn::CheckForGamepad()
{
	if (MyPlayerController->IsInputKeyDown(EKeys::Gamepad_FaceButton_Bottom) ||
		MyPlayerController->IsInputKeyDown(EKeys::Gamepad_LeftX) ||
		MyPlayerController->IsInputKeyDown(EKeys::Gamepad_DPad_Left) ||
		MyPlayerController->IsInputKeyDown(EKeys::Gamepad_DPad_Right))
	{
		gamePadIsActive = true;
	}
	else if (MyPlayerController->IsInputKeyDown(EKeys::SpaceBar) || MyPlayerController->IsInputKeyDown(EKeys::A) ||
		MyPlayerController->IsInputKeyDown(EKeys::D) || MyPlayerController->IsInputKeyDown(EKeys::Left) ||
		MyPlayerController->IsInputKeyDown(EKeys::Right))
	{
		gamePadIsActive = false;
	}
}

// Called when the game starts or when spawned
void APlayer_Pawn::BeginPlay()
{
	Super::BeginPlay();

	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("I am da player!"));
	MyPlayerController = GetWorld()->GetFirstPlayerController();
	MyPlayerController->bShowMouseCursor = false;

	if (wHUDWidget)
	{
		HUDWidget = CreateWidget<UUserWidget>(GetWorld(), wHUDWidget);
		if (wHUDWidget)
		{
			HUDWidget->AddToViewport();

			ScoreField = FindFProperty<FIntProperty>(HUDWidget->GetClass(), "Score");
			if (ScoreField != NULL)
			{
				if (ScoreField != NULL)
				{
					UpdateScore();
				}
			}
		}
	}
}

// Called every frame
void APlayer_Pawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckForGamepad();

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemy_Actor::StaticClass(), FoundActors);

	if (FoundActors.Num() == 0)
	{
		if (wGameOver)
		{
			GameOverWidget = CreateWidget<UUserWidget>(GetWorld(), wGameOver);
			if (wGameOver)
			{
				GameOverWidget->AddToViewport();
			}
		}
	}
}

// Called to bind functionality to input
void APlayer_Pawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAxis("Horizontal", this, &APlayer_Pawn::Move_XAxis);
	InputComponent->BindAction("Fire", IE_Pressed, this, &APlayer_Pawn::Fire);
}

void APlayer_Pawn::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	MyPlayerController->bShowMouseCursor = true;
	// in my wisdom I tagged the root component instead of the class. normally this would be just OtherActor->ActorHasTag
	if (OtherActor->GetRootComponent()->ComponentHasTag(FName(TEXT("Pusher"))) == true)
	{
		GLog->Log("Enemy Laser!");
		OtherActor->Destroy();
		GetWorld()->SpawnActor<AActor>(ExplosionBP, GetActorLocation(), GetActorRotation());

		if (gamePadIsActive)
		{
			MyPlayerController->PlayDynamicForceFeedback(1.0f, 1.0f, true, true,
			                                             true, true, EDynamicForceFeedbackAction::Start, 0);
		}
		if (wGameOver)
		{
			GameOverWidget = CreateWidget<UUserWidget>(GetWorld(), wGameOver);
			if (wGameOver)
			{
				GameOverWidget->AddToViewport();
			}
		}
		Destroy();
	}
}

void APlayer_Pawn::Move_XAxis(float AxisValue)
{
	FVector myLocation = GetActorLocation();
	float v = myLocation.X + AxisValue * 500.0f * GetWorld()->DeltaTimeSeconds;
	SetActorLocation(FVector(FMath::Clamp<float>(v, -200.0, 360.0), myLocation.Y, myLocation.Z));
	//GLog->Log("Move_XAxis");
}

void APlayer_Pawn::Fire()
{
	//GLog->Log("Fire");
	if (BulletActor)
	{
		FVector myLocation = GetActorLocation();
		ABulletActor* NewBulletActor = GetWorld()->SpawnActor<ABulletActor>(
			BulletActor, FVector(myLocation.X, myLocation.Y, myLocation.Z + 10.0f), FRotator::ZeroRotator);
	}
}
