// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "EnemyToymon.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTHIT_API AEnemyToymon : public AEnemy
{
	GENERATED_BODY()
public:
	AEnemyToymon();

	UFUNCTION()
	void RightHandOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void Die() override;

	virtual void Tick(float DeltaTime) override;

	void Accept(VisitorPtr Visitor) override;

protected:
	virtual void BeginPlay() override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION()
	void AttackRangeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void AttackRangeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void KnockDown(APlayerCharacter* Target, int32 index) override;


private:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Toymon | Combat", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* RightHandCollision;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Toymon | Combat", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* AttackRangeSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Toymon | Montage", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* KnockDownMontage;

	UPROPERTY()
	FName RightHandBloodSocket;

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

	FTimerHandle MatTimer_0;
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Toymon | Montage | PlayRate", meta = (AllowPrivateAccess = "true"))
	float KnockDownMontagePlayRate;
private:

};
