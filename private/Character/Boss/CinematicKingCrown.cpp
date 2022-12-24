// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Boss/CinematicKingCrown.h"
#include "../../../Public/Character/Enemy/EnemyController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/PlayerCharacter/PlayerCharacter.h"
#include "Character/Boss/KingCrownPunch.h"

ACinematicKingCrown::ACinematicKingCrown():
	CinematicKingCrown(ECinematicKingCrown::ECKC_MAX)
{
	PrimaryActorTick.bCanEverTick = true;

	Visitors.insert(std::make_pair("Punch", std::make_shared<KingCrownPunchVisitor>()));

}

void ACinematicKingCrown::Accept(VisitorPtr Visitor)
{
	Visitor->Visit(this);
}

void ACinematicKingCrown::SetBoolPunchStart()
{
	if (EnemyController)
	{
		EnemyController->GetBlackBoardComponent()->SetValueAsBool("bIsStart", false);
	}
}

void ACinematicKingCrown::BeginPlay()
{
	Super::BeginPlay();

	SetPullType(EPullType::EPT_DISALLOW);

	if (EnemyController)
	{
		EnemyController->GetBlackBoardComponent()->SetValueAsBool("bIsStart", true);
	}
}

void ACinematicKingCrown::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

float ACinematicKingCrown::ReturnPunchTimer()
{
	float PlayerDistance = GetDistanceTo(Player) - GetDistanceTo(Punch);
	float PunchVel = Punch->GetVelocity().Size();

	//UE_LOG(LogTemp, Warning, TEXT("PunchTimer : %f"), PlayerDistance / PunchVel);

	return PlayerDistance / PunchVel;
}

float ACinematicKingCrown::ReturnPunchDistance()
{
	float PlayerDistance = GetDistanceTo(Player) - GetDistanceTo(Punch);

	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("PlayerDistance : % f"), PlayerDistance));

	return PlayerDistance;
}

void ACinematicKingCrown::PunchPatternStart()
{
	auto bStart = EnemyController->GetBlackBoardComponent()->GetValueAsBool("bIsStart");

	if (bStart)
		return;

	EnemyController->GetBlackBoardComponent()->SetValueAsBool("bIsStart", true);

	SetLEDMat(1.f, 1.f, 1.f, 0.f, 0.f);
	SetFacialMat(1);
	SetCinematicPatternEnum(ECinematicKingCrown::ECKC_IDLE);
	PlayHighPriorityMontage(CinematicPunch, FName("Start"), 1.0f);
}

void ACinematicKingCrown::PunchSpawn()
{
	Super::PunchSpawn();
	SetCinematicPatternEnum(ECinematicKingCrown::ECKC_PUNCH);

	SetLEDMat(1.f, 1.f, 1.f, 0.f, 0.f);

}

void ACinematicKingCrown::PunchPatternEnd()
{
	if (CinematicKingCrown == ECinematicKingCrown::ECKC_HITTING)
		return;
	//SetFacialMat(0);

	SetCinematicPatternEnum(ECinematicKingCrown::ECKC_HITTING);

}

void ACinematicKingCrown::GroggyPatternStart_120()
{
	Super::GroggyPatternStart_120();

	PlayHighPriorityMontage(CinematicCrash, FName("Start"), 1.0f);
	SetFacialMat(2);
}

void ACinematicKingCrown::SetCinematicPatternEnum(ECinematicKingCrown Pattern)
{
	if (EnemyController)
	{
		SetCinematicKingCrown(Pattern);
		EnemyController->GetBlackBoardComponent()->SetValueAsEnum(FName("ECinematicKingCrown"), uint8(Pattern));
	}
}
