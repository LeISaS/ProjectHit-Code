// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Enemy/Enemy.h"
#include "EnemyForkyDoll.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTHIT_API AEnemyForkyDoll : public AEnemy
{
	GENERATED_BODY()

public:
	AEnemyForkyDoll();

	void Accept(VisitorPtr Visitor) override;
	
	UFUNCTION()
	void RightHandOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void Die() override;
	
	virtual void Attack() override ;

	virtual void Tick(float DeltaTime) override;

	void KnockDown(APlayerCharacter* Target, int32 index) override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
protected:
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION()
	void AttackRangeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void AttackRangeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void AttackLaunch();

	void AttackRushEnd();
	
	void AttackingCancelCheck();

	virtual void StunStart() override;

	virtual void StunEnd() override;

	virtual void EndTimer() override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "ForkyDoll | Combat", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* RightHandCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ForkyDoll | Montage", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* KnockDownMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ForkyDoll | Montage", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* StunMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "ForkyDoll | Combat", meta = (AllowPrivateAccess = "true"))
	USphereComponent* AttackRangeSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "ForkyDoll | Combat", meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* HealthBarWidget;

	UPROPERTY()
	class UMaterialInstanceDynamic* DynamicZeroMat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ForkyDoll | Materials ", meta = (AllowPrivateAccess = "true", DisplayName = "Fork"))
	class UMaterialInstance* ZeroMatInstance;

	UPROPERTY()
	UMaterialInstanceDynamic* DynamicOneMat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ForkyDoll | Materials ", meta = (AllowPrivateAccess = "true", DisplayName = "Cloth"))
	UMaterialInstance* OneMatInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ForkyDoll", meta = (AllowPrivateAccess = "true"))
		class UParticleSystemComponent* PS_Stun;

private:
	
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "ForkyDoll | Properties", meta = (AllowPrivateAccess = "true"))
	float AttackRushPower;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "ForkyDoll | Montage| PlayRate", meta = (AllowPrivateAccess = "true"))
	float KnockDownMontagePlayRate;

	UPROPERTY()
	bool bAttackCancel;

private:


private:
	FTimerHandle RushTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ForkyDoll | Timer", meta = (AllowPrivateAccess = "true"))
	float RushTime;

	FTimerHandle RushEndTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ForkyDoll | Timer", meta = (AllowPrivateAccess = "true"))
	float RushEndTime;

	FTimerHandle MatTimer_0;

public:


};
