// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "EnemyShark.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTHIT_API AEnemyShark : public AEnemy
{
	GENERATED_BODY()
public:
	AEnemyShark();


	void Accept(VisitorPtr Visitor) override;

	//void Knockback(FVector dir, float knockPower) override;
	
	virtual void Die() override;

protected:
	virtual void BeginPlay() override;


	void Fire();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shark | Particles", meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* FireParticles;
private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class ASharkBullet> SharkBullets;


};
