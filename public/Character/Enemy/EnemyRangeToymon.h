// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Enemy/Enemy.h"
#include "EnemyRangeToymon.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTHIT_API AEnemyRangeToymon : public AEnemy
{
	GENERATED_BODY()

public:

	AEnemyRangeToymon();

	virtual void Die() override;

	virtual void Tick(float DeltaTime) override;

	void Accept(VisitorPtr Visitor) override;

	virtual void Attack() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	virtual void BeginPlay() override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION()
		void AttackRangeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void AttackRangeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void KnockDown(APlayerCharacter* Target, int32 index) override;

	virtual void StunStart() override;

	virtual void StunEnd() override;

	UFUNCTION(BlueprintCallable)
	void BlockSpawn();

	UFUNCTION(BlueprintCallable)
	void DeattachBlock();

	UFUNCTION(BlueprintCallable)
	void DeattachHitBlock();

	UFUNCTION(BlueprintCallable)
	void IgnoreHitBlock();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Toymon | Class", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AEnemyRangeToymonBlock> BlockClass_1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Toymon | Class", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AEnemyRangeToymonBlock> BlockClass_2;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Toymon | Class", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AEnemyRangeToymonBlock> BlockClass_3;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Toymon | Class", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AEnemyRangeToymonBlock> BlockClass_4;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Toymon | Class", meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<AEnemyRangeToymonBlock>> ArrBlockClass;


	AEnemyRangeToymonBlock* Block;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Toymon | Combat", meta = (AllowPrivateAccess = "true"))
		class USphereComponent* AttackRangeSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Toymon | Montage", meta = (AllowPrivateAccess = "true"))
		UAnimMontage* KnockDownMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Toymon | Montage", meta = (AllowPrivateAccess = "true"))
		UAnimMontage* StunMontage;


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Toymon | Combat", meta = (AllowPrivateAccess = "true"))
		class UWidgetComponent* HealthBarWidget;

	UPROPERTY()
		class UMaterialInstanceDynamic* DynamicZeroMat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Toymon | Materials ", meta = (AllowPrivateAccess = "true", DisplayName = "RibbonMat"))
		class UMaterialInstance* ZeroMatInstance;

	UPROPERTY()
		UMaterialInstanceDynamic* DynamicOneMat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Toymon | Materials ", meta = (AllowPrivateAccess = "true", DisplayName = "BodyMat"))
		UMaterialInstance* OneMatInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Toymon", meta = (AllowPrivateAccess = "true"))
	class UParticleSystemComponent* PS_Stun;

	FTimerHandle MatTimer_0;

	bool bAttacking;
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Toymon | Montage | PlayRate", meta = (AllowPrivateAccess = "true"))
		float KnockDownMontagePlayRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Toymon", meta = (AllowPrivateAccess = "true"))
		float ThrowSpeed;
private:
	
};
