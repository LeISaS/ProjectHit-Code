// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/TimelineComponent.h"
#include "Core/Trigger/EnemySpawner.h"
#include "KingCrownGiftBox.generated.h"

UCLASS()
class PROJECTHIT_API AKingCrownGiftBox : public AEnemySpawner
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AKingCrownGiftBox();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void SpawnStart();

	UFUNCTION(BlueprintCallable)
		void SpawnMonster();

	UFUNCTION(BlueprintCallable)
		void CurveStart();

	UFUNCTION()
		void GiftLine(float Value);

	UFUNCTION()
	void GiftBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void FindPlayer();


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	class UProjectileMovementComponent* ProjectileMovementComponent;

	virtual void OnConstruction(const FTransform& Transform) override;

private:


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Class", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* GiftBoxCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Class", meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* GiftMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Class", meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* HealthBarWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Class", meta = (AllowPrivateAccess = "true"))
	class APlayerCharacter* Player;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Montage", meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* MoveMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particles", meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* DestroyParticle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particles", meta = (AllowPrivateAccess = "true"))
	UParticleSystemComponent* TrailParticle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particles", meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* DeathParticle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particles", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* PS_CharSlash;

	class UMaterialInstanceDynamic* DynamicZeroMat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials ", meta = (AllowPrivateAccess = "true"))
	class UMaterialInstance* ZeroMatInstance;

	FTimeline GiftTimeline;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Curve", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* GiftMatCurve;

	UPROPERTY()
	bool bLaunchStart;

	UPROPERTY()
	bool bAnimStart;

	UPROPERTY()
	bool bCurveStart ;

	FTimerHandle LaunchTimer;

	UPROPERTY()
	float LaunchTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gift", meta = (AllowPrivateAccess = "true"))
	float MinPower;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gift", meta = (AllowPrivateAccess = "true"))
	float MaxPower;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gift", meta = (AllowPrivateAccess = "true"))
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gift", meta = (AllowPrivateAccess = "true"))
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gift", meta = (AllowPrivateAccess = "true"))
	float GiftDamage;

	UPROPERTY()
	float RandomPower;

	FTimerHandle OffDamageTimerHandle;

	FTimerHandle DestoryTimerHandle;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "FMOD", meta = (AllowPrivateAccess = "true"))
	class UFMODEvent* FMOD_Back;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "FMOD", meta = (AllowPrivateAccess = "true"))
	class UFMODEvent* FMOD_Death;
};
