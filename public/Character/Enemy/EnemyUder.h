// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Enemy/Enemy.h"
#include "../../Core/Trigger/SpawnLevel.h"
#include "EnemyUder.generated.h"

UENUM(BlueprintType)
enum class EUderAttackState : uint8
{
	EUAS_NOATTACKING		UMETA(DisplayName = "NOATTACKING"),
	EUAS_ATTACKING			UMETA(DisplayName = "ATTACKING"),

	EUAS_MAX				UMETA(DisplayName = "DefaultMax")
};

UENUM(BlueprintType)
enum class EUderActivity : uint8
{
	EUA_TOGETHER			UMETA(DisplayName = "Together"),
	EUA_LEFT						UMETA(DisplayName = "Left"),
	EUA_RIGHT						UMETA(DisplayName = "Right"),

	EUA_MAX						UMETA(DisplayName = "DefaultMax")
};

UENUM(BlueprintType)
enum class EUderNumbering : uint8
{
	EUN_NONE		UMETA(DisplayName = "None	"),
	EUN_ONE		UMETA(DisplayName = "One	"),
	EUN_TWO			UMETA(DisplayName = "Two"),
	EUN_THREE		UMETA(DisplayName = "Three"),
	EUN_FOUR			UMETA(DisplayName = "Four"),
	EUN_LAST		UMETA(DisplayName = "Last"),
	EUN_LASTTOGETHER		UMETA(DisplayName = "LASTTOGETHER"),

	EUN_MAX			UMETA(DisplayName = "MAX")
};

UENUM(BlueprintType)
enum class EUderInvincibility : uint8
{
	EUI_NONE		UMETA(DisplayName = "None"),
	EUI_INVINCIBILITY		UMETA(DisplayName = "Invincibility"),
	EUI_MAX		UMETA(DisplayName = "Max")
};

/**
 * 
 */
UCLASS()
class PROJECTHIT_API AEnemyUder : public AEnemy
{
	GENERATED_BODY()
	
public:

	AEnemyUder();

	virtual void Tick(float DeltaTime) override;
	
	virtual void BeginPlay() override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	void Accept(VisitorPtr Visitor) override;

	virtual void Die() override;

	UFUNCTION(BlueprintCallable)
	void LoopDeath();
	
	virtual void RushAttack();

	UFUNCTION(BlueprintCallable)
	virtual void RushEnd();

	virtual void Attack() override;
	virtual void AttackEnd() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCallable)
	void EndVisibility();

	UFUNCTION(BlueprintCallable)
	void DetachArm();

	void ShieldFunc();

	UFUNCTION(BlueprintCallable)
	virtual void BehHealthCheck();

	UFUNCTION(BlueprintCallable)
	void UderNumberChange(EUderNumbering Numbering);

	UFUNCTION(BlueprintCallable)
		void InvinRetreatStart();

	UFUNCTION(BlueprintCallable)
		void InvinRetreatEnd();

	virtual void StunStart() override;

	virtual void EndTimer() override;

	UFUNCTION(BlueprintCallable)
	void SpawnInviParticle();

protected:
	UFUNCTION()
	void BoomRangeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void OnConstruction(const FTransform& Transform) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Uder | Class", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<ASpawnLevel> UderSpawnLevel;

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Uder | Class", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* RightHandCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Uder | Class", meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* HealthBarWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Uder | Class", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* AttackRangeSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Uder | Class", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* RushRangeSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Uder | Class", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* RushCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Uder | Montage", meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* RushMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Uder | Montage", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* ParringMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Uder | Class", meta = (AllowPrivateAccess = "true"))
	USphereComponent* BoomRangeSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Uder | Enum", meta = (AllowPrivateAccess = "true"))
	EUderActivity UderActivity;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Uder | Enum", meta = (AllowPrivateAccess = "true"))
	EUderAttackState UderAttackState;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Uder | Enum", meta = (AllowPrivateAccess = "true"))
	EUderNumbering UderNumbering;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Uder | Enum", meta = (AllowPrivateAccess = "true"))
	EUderInvincibility UderInvincibility;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Uder | Particles", meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* BoomParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Uder | Particles", meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* BoomSignParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Uder | Particles", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* RushParticles;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Uder | Class", meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* RightArm;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Uder | Particles", meta = (AllowPrivateAccess = "true"))
	class UParticleSystemComponent* PS_Shiled_Blue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Uder | Particles", meta = (AllowPrivateAccess = "true"))
	UParticleSystemComponent* PS_Shiled_Red;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Uder | Particles", meta = (AllowPrivateAccess = "true"))
	UParticleSystemComponent* PS_Shiled_Invi;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Uder | Class", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* LeftArm;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Uder | Montage", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* LeftArmParring;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Uder | Montage", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* RightArmParring;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Uder | Montage", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* HitParringMontage;

	class UMaterialInstanceDynamic* DynamicZeroMat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Uder | Materials ", meta = (AllowPrivateAccess = "true", DisplayName = "UderHead"))
	class UMaterialInstance* ZeroMatInstance;

	UMaterialInstanceDynamic* DynamicOneMat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Uder | Materials ", meta = (AllowPrivateAccess = "true", DisplayName = "UderArm"))
	UMaterialInstance* OneMatInstance;

	UMaterialInstanceDynamic* DynamicTwoMat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Uder | Materials ", meta = (AllowPrivateAccess = "true", DisplayName = "UderBody"))
	UMaterialInstance* TwoMatInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Uder | CameraShake ", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UMatineeCameraShake> BoomCameraShake;

	FTimerHandle MatTimer_0;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Uder | FMOD", meta = (AllowPrivateAccess = "true"))
	class UFMODEvent* FMOD_UderBoom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Uder | Montage", meta = (AllowPrivateAccess = "true"))
		UAnimMontage* StunMontage;




private:

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Uder | Combat", meta = (AllowPrivateAccess = "true"))
	float AttackLaunchPower;

	FTimerHandle BoomTimer;

	FTimerHandle DestroyParticleTimer;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Uder | Combat", meta = (AllowPrivateAccess = "true"))
	float BoomTime;

private:
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Uder | Damage", meta = (AllowPrivateAccess = "true"))
	float AttackDamage;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Uder | Damage", meta = (AllowPrivateAccess = "true"))
	float RushDamage;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Uder | Damage", meta = (AllowPrivateAccess = "true"))
	int ShieldCount;
	
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Uder | Damage", meta = (AllowPrivateAccess = "true"))
	float RushEnemyKnockBack;

	UPROPERTY()
	bool bRushRotChar = false;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Uder | Damage", meta = (AllowPrivateAccess = "true"))
	float UderBoomAttackDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Uder | Damage", meta = (AllowPrivateAccess = "true"))
	float ShiledOneRatio;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Uder | Damage", meta = (AllowPrivateAccess = "true"))
	float ShiledTwoRatio;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Uder | Particles", meta = (AllowPrivateAccess = "true"))
	FVector DieBoomScale;

	FTimerHandle StartTimer;
	FTimerHandle RushCoolDownTimer;

protected:
	FORCEINLINE USphereComponent* GetAttackRangeSphere() const { return AttackRangeSphere; }
	FORCEINLINE UBoxComponent* GetRightHandCollision() const { return RightHandCollision; }
	FORCEINLINE USphereComponent* GetRushRangeSphere() const { return RushRangeSphere; }
	FORCEINLINE UBoxComponent* GetRushCollision() const { return RushCollision; }
	
	FORCEINLINE float GetAttackDamage() const { return AttackDamage; }
	FORCEINLINE float GetRushDamage() const { return RushDamage; }

	FORCEINLINE UAnimMontage* GetHitParringMontage() const { return HitParringMontage; }


	static float TotalHp;
	static float LeftHp;
	static float RightHp;
	static bool bHealthCheck;


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Uder | Combat", meta = (AllowPrivateAccess = "true"))
	float TotalMaxHp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Uder | Timer", meta = (AllowPrivateAccess = "true"))
	float RushCoolDownTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Uder | Combat", meta = (AllowPrivateAccess = "true"))
	float UderDistance;
	
	bool bInvi;

public:
	FORCEINLINE void SetEUderActivity(EUderActivity State) { UderActivity = State; }
	FORCEINLINE EUderActivity GetUderActivity() const { return UderActivity; }
	
	void SetEUderActivityEnum(EUderActivity State);

	FORCEINLINE void SetEUderNumbering(EUderNumbering State) { UderNumbering = State; }
	FORCEINLINE EUderNumbering GetUderNumbering() const { return UderNumbering; }

	void SetEUderNumberingEnum(EUderNumbering State);

	FORCEINLINE void SetUderAttackState(EUderAttackState State) { UderAttackState = State; }
	FORCEINLINE EUderAttackState GetUderAttackState() const { return UderAttackState; }

	FORCEINLINE void SetEUderInvincibility(EUderInvincibility State) { UderInvincibility = State; }
	FORCEINLINE EUderInvincibility GetEUderInvincibility() const { return UderInvincibility; }

	FORCEINLINE UParticleSystemComponent* GetInviParticleComponent() const { return PS_Shiled_Invi; }
	FORCEINLINE UParticleSystemComponent* GetPS_BlueParticleComponent() const { return PS_Shiled_Blue; }
	FORCEINLINE UParticleSystemComponent* GetPS_RedParticleComponent() const { return PS_Shiled_Red; }

	void SetEUderInvincibilityEnum(EUderInvincibility State);
	
};
