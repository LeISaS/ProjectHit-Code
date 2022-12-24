// Fill out your copyright notice in the Description page of Project Settings.


#include "../../../Public/Character/Enemy/EnemyController.h"
#include "../../../Public/Character/Enemy/Enemy.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Core/JoyFollowPlayer.h"

AEnemyController::AEnemyController()
{
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
	check(BlackboardComponent);

	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
	check(BehaviorTreeComponent);
}

void AEnemyController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (InPawn == nullptr) return;

	AEnemy* Enemy = Cast<AEnemy>(InPawn);
	AJoyFollowPlayer* Joy = Cast<AJoyFollowPlayer>(InPawn);

	if (Enemy)
	{
		if (Enemy->GetBehaviorTree())
		{
			BlackboardComponent->InitializeBlackboard(*(Enemy->GetBehaviorTree()->BlackboardAsset));
		}	
	}
	if (Joy)
	{
		if (Joy->GetBehaviorTree())
		{
			BlackboardComponent->InitializeBlackboard(*(Joy->GetBehaviorTree()->BlackboardAsset));
		}
	}


}
