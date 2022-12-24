// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Enemy/Enemy.h"
#include "KingCrownGiftBox.h"
#include "../../Core/Trigger/SpawnLevel.h"
#include "KingCrown.generated.h"



UCLASS()
class PROJECTHIT_API AKingCrown : public AEnemy
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AKingCrown();

	virtual void GiftPatternStart();

	virtual void Die() override;

	UFUNCTION(BlueprintCallable)
	void NotifyCrash();

	virtual void UnNotifyCrash();
	
	UFUNCTION(BlueprintCallable)
	void OnDeathParticle();

	void PunchParringChangeMat();

	virtual void PunchPatternStart();

	virtual void PunchSpawn();

	virtual void PunchPatternEnd();

	UFUNCTION(BlueprintCallable)
	void PunchDoorEffect();

	UFUNCTION(BlueprintCallable)
	void onInterpRotNotify();

	UFUNCTION(BlueprintCallable)
	void offInterpRotNotify();

	virtual void DeathStart();

	virtual void GroggyPatternStart_120();

	void ClearFoucsSetting();

	UFUNCTION(BlueprintCallable)
	void SetFacialMat(float Value);

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
protected:
	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaTime);

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	virtual void OnConstruction(const FTransform& Transform) override;

	void SetLEDMat(float Idle, float On, float Anger, float HeRong, float Attacked);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Boss | Class", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AKingCrownPunch> PunchClass;

	AKingCrownPunch* Punch;

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Boss | Collision", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* MainCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss | Montage", meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* GroggyMontage_120;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss | Montage", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* KingCrown_GiftFireMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss | Montage", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* KingCrown_HitMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss | Montage", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* KingCrown_BeforePunchAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss | Montage", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* KingCrown_Crain;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss | Montage", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* KingCrown_StartShockWave;

	class UMaterialInstanceDynamic* DynamicZeroMat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss | Materials ", meta = (AllowPrivateAccess = "true", DisplayName = "CrownBox"))
	class UMaterialInstance* ZeroMatInstance;

	UMaterialInstanceDynamic* DynamicOneMat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss | Materials ", meta = (AllowPrivateAccess = "true", DisplayName = "Glass"))
	UMaterialInstance* OneMatInstance;

	UMaterialInstanceDynamic* DynamicTwoMat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss | Materials ", meta = (AllowPrivateAccess = "true", DisplayName = "LED"))
	UMaterialInstance* TwoMatInstance;


	UMaterialInstanceDynamic* DynamicThreeMat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss | Materials ", meta = (AllowPrivateAccess = "true", DisplayName = "KingCrown"))
		UMaterialInstance* ThreeMatInstance;


	UMaterialInstanceDynamic* DynamicFourMat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss | Materials ", meta = (AllowPrivateAccess = "true", DisplayName = "Face"))
		UMaterialInstance* FourMatInstance;




	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss | Particles", meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* PS_GiftFireParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss | Particles", meta = (AllowPrivateAccess = "true"))
	class UParticleSystemComponent* RocketParticle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss | Particles", meta = (AllowPrivateAccess = "true"))
	UParticleSystemComponent* PS_Wind;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss | Particles", meta = (AllowPrivateAccess = "true"))
	class UParticleSystemComponent* PS_KingCrownCrash;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss | Particles", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* PS_Death;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss | Particles", meta = (AllowPrivateAccess = "true"))
	UParticleSystemComponent* PS_PunchFire;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss | Particles", meta = (AllowPrivateAccess = "true"))
	UParticleSystemComponent* PS_PunchDoor;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Boss | FMOD", meta = (AllowPrivateAccess = "true"))
	UFMODEvent* FMOD_PunchEnergy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss | Particles", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* PS_Glass_2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss | Particles", meta = (AllowPrivateAccess = "true"))
	FVector PS_Glass_2Location;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss | Particles", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* PS_Glass_3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss | Particles", meta = (AllowPrivateAccess = "true"))
	FVector PS_Glass_3Location;
private:

	FTimerHandle MatTimer_0;

	FTimerHandle LEDTimer;

private:

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss | Particles", meta = (AllowPrivateAccess = "true"))
	float DeathParticleZ;

	bool bPatternInterpRot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss | Combat", meta = (AllowPrivateAccess = "true"))
	float GlassTwoHp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss | Combat", meta = (AllowPrivateAccess = "true"))
	float GlassThreeHp;

public:

	FORCEINLINE UBoxComponent* GetKingCrownMainCollision() const { return MainCollision; }

	FORCEINLINE class UParticleSystemComponent* GetPS_PunchFire() const { return PS_PunchFire; }
	FORCEINLINE class UParticleSystemComponent* GetPS_PunchDoor() const { return PS_PunchDoor; }
	FORCEINLINE class UParticleSystemComponent* GetPS_Wind() const { return PS_Wind; }
	FORCEINLINE class UParticleSystem* GetPS_GiftFireParticles() const { return PS_GiftFireParticles; }

	FORCEINLINE class UAnimMontage* GetStartShockWaveMontage() const { return KingCrown_StartShockWave; }
	FORCEINLINE class UAnimMontage* GetKingCrownHitMontage() const { return KingCrown_HitMontage; }
	FORCEINLINE class UAnimMontage* GetBeforePunchAnim() const { return KingCrown_BeforePunchAnim; }
	FORCEINLINE class UAnimMontage* GetGroggyMontage_120() const { return GroggyMontage_120; }



};