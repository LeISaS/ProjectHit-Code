// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Enemy/Enemy.h"
#include "Components/TimelineComponent.h"

#include "EnemyBomber.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTHIT_API AEnemyBomber : public AEnemy
{
	GENERATED_BODY()

public:
	AEnemyBomber();

	virtual void Die() override;

	virtual void Tick(float DeltaTime) override;

	void Accept(VisitorPtr Visitor) override;

	UFUNCTION()
	void BoomLine(float Value);

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Bomber | Combat", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* IgnitionSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Bomber | Combat", meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* HealthBarWidget;

	UFUNCTION()
	void IgnitionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION(BlueprintCallable)
	void Boom();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bomber | Particles", meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* ExplodeParticles;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bomber | Particles", meta = (AllowPrivateAccess = "true"))
	FVector ExplodeVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bomber | Particles", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* BalloonBoomParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bomber | Particles", meta = (AllowPrivateAccess = "true"))
	FVector BoomBallonVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bomber | Particles", meta = (AllowPrivateAccess = "true"))
	FVector DieBallonVector;

	class UMaterialInstanceDynamic* DynamicZeroMat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bomber | Materials ", meta = (AllowPrivateAccess = "true", DisplayName = "Ballon"))
	class UMaterialInstance* ZeroMatInstance;

	UMaterialInstanceDynamic* DynamicOneMat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bomber | Materials ", meta = (AllowPrivateAccess = "true", DisplayName = "Body"))
	UMaterialInstance* OneMatInstance;


	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Bomber | FMOD", meta = (AllowPrivateAccess = "true"))
	class UFMODEvent* FMOD_BalloonBoom;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Bomber | FMOD", meta = (AllowPrivateAccess = "true"))
	UFMODEvent* FMOD_Die;

private:
	FTimerHandle DeathTimer;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bomber | Timer", meta = (AllowPrivateAccess = "true"))
	float DeathTimer_Time;

	FTimerHandle DestoryTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bomber | Speed", meta = (AllowPrivateAccess = "true"))
	float IgnitionSpeed;

	FTimerHandle MatTimer_0;

	FTimeline BoomMatTimeline;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Bomber | Curve", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* BoomMatCurve;
		
	FTimerHandle ChangeStateHandle;

private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bomber | Damage", meta = (AllowPrivateAccess = "true"))
	float BoomDamage;

public:

	
};
