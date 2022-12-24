// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyController.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTHIT_API AEnemyController : public AAIController
{
	GENERATED_BODY()
public:
	AEnemyController();

	virtual void OnPossess(APawn* InPawn) override;

protected:
	UPROPERTY(BlueprintReadWrite, Category = "AI Behavior",meta = (AllowPrivateAccess = "true"))
	class UBlackboardComponent* BlackboardComponent;
	
	UPROPERTY(BlueprintReadWrite, Category = "AI Behavior", meta = (AllowPrivateAccess = "true"))
	class UBehaviorTreeComponent* BehaviorTreeComponent;

public:
	FORCEINLINE UBlackboardComponent* GetBlackBoardComponent() const { return BlackboardComponent; }

};
