// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "JoyFollowPlayer.generated.h"

UENUM(BlueprintType)
enum class EJoyState : uint8
{
	EJS_NoBattle			UMETA(DisplayName = "NoBattle"),
	EJS_Battle				UMETA(DisplayName = "Battle"),

	EJS_DefaultMax		UMETA(DisplayName = "DefaultMax")
};


UCLASS()
class PROJECTHIT_API AJoyFollowPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	AJoyFollowPlayer();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere, Category = "Behavior Tree", meta = (AllowPrivateAccess = "true"))
	class UBehaviorTree* BehaviorTree;

	class AEnemyController* EnemyController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	class  APlayerCharacter* Player;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	EJoyState JoyState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* RangeParticle;

private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float SpeedMovement = 600.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float MinDistanceActor = 130.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float StopRaidus = 0.f;

	float Speed;

public:
	FORCEINLINE EJoyState GetJoyState() const { return JoyState; }
	FORCEINLINE void SetJoyState(EJoyState State) { JoyState = State; }
	FORCEINLINE UBehaviorTree* GetBehaviorTree() const { return BehaviorTree; }

	void SetJoyEnum(EJoyState State);

};
