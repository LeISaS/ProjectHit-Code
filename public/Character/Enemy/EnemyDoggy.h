// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Enemy/Enemy.h"
#include "EnemyDoggy.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTHIT_API AEnemyDoggy : public AEnemy
{
	GENERATED_BODY()

public:
	AEnemyDoggy();


	UFUNCTION()
	void AttackCollisionOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void Die() override;

	virtual void Attack() override;
	virtual void AttackEnd() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void Accept(VisitorPtr Visitor) override;

	void RetreatStart();

	UFUNCTION(BlueprintCallable)
	void RetreatEnd();

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION()
	void AttackRangeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void AttackRangeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void RetreatRangeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void RetreatRangeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void KnockDown(APlayerCharacter* Target, int32 index) override;

	virtual void StunStart() override;

	virtual void StunEnd() override;


private:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Doggy | Combat", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* AttackCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Doggy | Combat", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* RetreatCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Doggy | Combat", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* AttackRangeSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Doggy | Combat", meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* HealthBarWidget;

	UPROPERTY()
	class UMaterialInstanceDynamic* DynamicZeroMat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Doggy | Materials ", meta = (AllowPrivateAccess = "true", DisplayName = "Doggy"))
	class UMaterialInstance* ZeroMatInstance;

	FTimerHandle MatTimer_0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Doggy | Montage", meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* KnockDownMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Doggy | Montage", meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* RetreateMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Doggy | Montage", meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* StunMontage;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Doggy", meta = (AllowPrivateAccess = "true"))
	class UParticleSystemComponent* PS_Stun;

private:

	UPROPERTY()
	FName BloodSocket;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Doggy | Montage | PlayRate", meta = (AllowPrivateAccess = "true"))
	float KnockDownMontagePlayRate;

	FTimerHandle RetreatCoolDownTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Doggy", meta = (AllowPrivateAccess = "true"))
	float RetreatCoolDownTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Doggy", meta = (AllowPrivateAccess = "true"))
	bool bRetreat;


public:

	
};
