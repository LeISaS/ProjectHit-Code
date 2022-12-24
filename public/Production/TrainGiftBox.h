// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Core/Trigger/EnemySpawner.h"
#include "Components/TimelineComponent.h"
#include "TrainGiftBox.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTHIT_API ATrainGiftBox : public AEnemySpawner
{
	GENERATED_BODY()
public:
	// Sets default values for this actor's properties
	ATrainGiftBox();

public:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void OnConstruction(const FTransform& Transform) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	class UDontStopProjectileMovement* ProjectileMovementComponent;

	UFUNCTION()
	void SwordOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable)
		void SpawnMonster();

	void OffBallon();

	UFUNCTION(BlueprintCallable)
		void CurveStart();

	UFUNCTION()
		void GiftLine(float Value);

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Gift", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* GiftBoxCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Gift", meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* GiftMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Gift", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* BalloonMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gift", meta = (AllowPrivateAccess = "true"))
		class UMaterialInstanceDynamic* DynamicBallonMatInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gift", meta = (AllowPrivateAccess = "true"))
		class UMaterialInstance* BallonMatInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particles", meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* DestroyParticle;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particles", meta = (AllowPrivateAccess = "true"))
	//UParticleSystemComponent* TrailParticle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particles", meta = (AllowPrivateAccess = "true"))
		UParticleSystem* BoomParticle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<AActor> Train;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AActor> TrainClass;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Gift", meta = (AllowPrivateAccess = "true"))
		class UAnimMontage* MoveMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Materials ", meta = (AllowPrivateAccess = "true"))
	class UMaterialInstanceDynamic* DynamicZeroMat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials ", meta = (AllowPrivateAccess = "true"))
	class UMaterialInstance* ZeroMatInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "class", meta = (AllowPrivateAccess = "true"))
		class UWidgetComponent* HealthBarWidget;
	FTimeline GiftTimeline;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Curve", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* GiftMatCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particles", meta = (AllowPrivateAccess = "true"))
		UParticleSystem* PS_CharSlash;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particles", meta = (AllowPrivateAccess = "true"))
		UParticleSystem* DeathParticle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties", meta = (AllowPrivateAccess = "true"))
	float MaxHp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties", meta = (AllowPrivateAccess = "true"))
	float NowHp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties", meta = (AllowPrivateAccess = "true"))
	float FlyingSpeed;

	bool bFalling;
	bool bGround;

	float LerpScale;
	FVector DirectionVector;

	UPROPERTY()
	FVector LerpLocation;

	UPROPERTY()
	FVector FlyingLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties", meta = (AllowPrivateAccess = "true"))
	TArray<FVector> GiftEndOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties", meta = (AllowPrivateAccess = "true"))
	float SpwanTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties", meta = (AllowPrivateAccess = "true"))
	float Duration;

	float DurationRatio;

	float StartZOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties", meta = (AllowPrivateAccess = "true"))
	float DirectionDistance;

	FVector EndOffset;

	float StartYaw;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties", meta = (AllowPrivateAccess = "true"))
	float FlyingZLength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties", meta = (AllowPrivateAccess = "true"))
	float FlyingZSpeed;

	float ZCosOffSet;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "FMOD", meta = (AllowPrivateAccess = "true"))
	class UFMODEvent* FMOD_Balloon;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "FMOD", meta = (AllowPrivateAccess = "true"))
		 UFMODEvent* FMOD_Move;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "FMOD", meta = (AllowPrivateAccess = "true"))
		UFMODEvent* FMOD_Back;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "FMOD", meta = (AllowPrivateAccess = "true"))
		class UFMODEvent* FMOD_Death;

private:
	FTimerHandle SpawnTimerHandle;
	FTimerHandle DestoryTimerHandle;
	FTimerHandle OffDamageTimerHandle;
};
