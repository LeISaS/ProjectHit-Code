// Fill out your copyright notice in the Description page of Project Settings.


#include "../../../Public/Character/Enemy/EnemyAnimInstance.h"
#include "../../../Public/Character/Enemy/Enemy.h"
#include "../../../Public/Character/Boss/KingCrownCrane.h"
#include "../../../Public/Character/Boss/KingCrown.h"
#include "../../../Public/Character/Enemy/EnemyUderRight.h"
#include "../../../Public/Character/Enemy/EnemyUderLeft.h"
#include "Character/Boss/TwoPhaseKingCrown.h"
#include "GameFramework/CharacterMovementComponent.h"


void UEnemyAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Enemy = Cast<AEnemy>(TryGetPawnOwner());
	CrownCrane = Cast<AKingCrownCrane>(TryGetPawnOwner());
	KingCrown = Cast<AKingCrown>(TryGetPawnOwner());
	UderRight = Cast<AEnemyUderRight>(TryGetPawnOwner());
	UderLeft = Cast<AEnemyUderLeft>(TryGetPawnOwner());
	TwoPhaseKingCrown = Cast<ATwoPhaseKingCrown>(TryGetPawnOwner());
	
}

void UEnemyAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (Enemy)
	{
		FVector Velocity = Enemy->GetVelocity();
		Velocity.Z = 0.f;

		Speed = Velocity.Size();
		
		Direction = CalculateDirection(Velocity, Enemy->GetActorRotation());
		EnemyMove = uint8(Enemy->GetMonsterMoveState());
	}

	if (CrownCrane)
	{
		CraneState = uint8(CrownCrane->GetCraneStampState());
	}

	if (UderRight)
	{
		UderRightState = uint8(UderRight->GetEUderAnimRight());
	}

	if (UderLeft)
	{
		UderLeftState = uint8(UderLeft->GetEUderAnimLeft());
	}

	if (TwoPhaseKingCrown)
	{
		bIsInAir = TwoPhaseKingCrown->GetMovementComponent()->IsFalling();
	}
}
