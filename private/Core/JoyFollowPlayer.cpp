// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/JoyFollowPlayer.h"
#include "Character/Enemy/EnemyController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/PlayerCharacter/PlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

AJoyFollowPlayer::AJoyFollowPlayer():
	JoyState(EJoyState::EJS_NoBattle)
{
	PrimaryActorTick.bCanEverTick = true;

}

void AJoyFollowPlayer::BeginPlay()
{
	Super::BeginPlay();
	
	SpawnDefaultController();

	EnemyController =Cast<AEnemyController>(GetController());

	Player = Cast<APlayerCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());

	if (EnemyController)
	{
		EnemyController->GetBlackBoardComponent()->SetValueAsObject(FName("Target"), Player);
		EnemyController->RunBehaviorTree(BehaviorTree);
	}

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}

void AJoyFollowPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Player)
	{
		FVector CurrentLocation = GetActorLocation();

		FVector OtherActorPosition = Player->GetMesh()->GetSocketLocation(FName("FollowJoySocket"));

		float DistanceToActor = FVector::Dist(OtherActorPosition, CurrentLocation);

		if (GetDistanceTo(Player) <= 200)
			Speed = SpeedMovement;
		else
		{
			Speed = FMath::Lerp(SpeedMovement, SpeedMovement + 300, 0.3f);
		}

		if (DistanceToActor < StopRaidus)
		{
			float SpeedFactor = UKismetMathLibrary::NormalizeToRange(DistanceToActor, MinDistanceActor, StopRaidus);
			Speed *= SpeedFactor;
		}

		FVector DirectionToFollow = OtherActorPosition - CurrentLocation;
		DirectionToFollow = DirectionToFollow.GetSafeNormal();

		FRotator TargetRotation = Player->GetActorRotation();

		FVector NewLocation = GetActorLocation();
		NewLocation += DirectionToFollow * Speed * DeltaTime;

		FRotator CurrentRotation = GetActorRotation();
		CurrentRotation.Pitch = 0.f;
		CurrentRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, 5.0f);

		SetActorLocationAndRotation(NewLocation, CurrentRotation);

		if (Player->GetCurrentState() == EPlayerState::EPS_IDLE)
		{
			SetJoyEnum(EJoyState::EJS_NoBattle);
		}
		else
		{
			SetJoyEnum(EJoyState::EJS_Battle);
		}

		if (GetDistanceTo(Player) >= 700.f)
		{
			TeleportTo(OtherActorPosition, CurrentRotation);
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), RangeParticle,OtherActorPosition);
		}
	}

}

void AJoyFollowPlayer::SetJoyEnum(EJoyState State)
{
	if (EnemyController)
	{
		SetJoyState(State);

		EnemyController->GetBlackBoardComponent()->SetValueAsEnum(FName("EJoyState"), uint8(State));
	}
}


