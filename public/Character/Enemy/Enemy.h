// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Character/Unit.h"
#include <map>
#include "../Data/DTEnemyHit.h"
#include "Core/Visitor/Visitor.h"
#include "Engine/DataTable.h"
#include "FMODBlueprintStatics.h"
#include "Enemy.generated.h"

USTRUCT(BlueprintType)
struct FHitParticle
{
	GENERATED_BODY()


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray <UParticleSystem*> HitParticle;	

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray <FVector> HitLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray <FRotator> HitRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray <FVector> HitScale = { FVector(1.f,1.f,1.f) };

};


USTRUCT(BlueprintType)
struct FMonsterMaster : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString Key;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool MonsterSubType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float BaseDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float ParryingDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float AcceptableRadius;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float RetreatRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float WalkSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool SpawnEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FHitParticle> HitParticles;

};

UENUM(BlueprintType)
enum class EMonsterGuardPoint : uint8
{
	EMGP_ON			UMETA(DisplayName = "GuardPointOn"),
	EMGP_OFF			UMETA(DisplayName = "GuardPointOff"),

	EMGP_MAX				UMETA(DisplayName = "DefaultMax")
};

UENUM(BlueprintType)
enum class EEnemyAutoTarget : uint8
{
	EEAT_AUTOTARGET				UMETA(DisplayName = "AutoTarget"),
	EEAT_NOTARGET					UMETA(DisplayName = "NoTarget"),

	EEAT_DEFAULTMAX				UMETA(DisplayName = "DefaultMax")
};


UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	EES_IDLE								UMETA(DisplayName = "Idle"),
	EES_MOVE								UMETA(DisplayName = "Move"),
	EES_ATTACK							UMETA(DisplayName = "Attack"),
	EES_PARRYING						UMETA(DisplayName = "Parring"),
	EES_KNOCKBACK					UMETA(DisplayName = "KnockBack"),
	EES_KNOCKDOWN				UMETA(DisplayName = "KnockDown"),
	EES_STUN								UMETA(DisplayName = "Stun"),
	EES_DEAD								UMETA(DisplayName = "Dead"),
	
	EES_DEAFULTMAX				UMETA(DisplayName = "DefaultMax")
};

UENUM(BlueprintType)
enum class EMonsterKnockBackState : uint8
{
	EMKS_KNOCKBACK			UMETA(DisplayName = "KnockBack"),
	EMKS_KNOCKDOWN			UMETA(DisplayName = "KnockDown"),
	EMKS_STUN								UMETA(DisplayName = "Stun"),
	EMKS_NONE				UMETA(DisplayName = "None"),

	EKBS_DEFAULTMAX						UMETA(DisplayName = "DeafultMax")
};

UENUM(BlueprintType)
enum class EMonsterGuardable : uint8
{
	EMG_ABLE			UMETA(DisplayName = "Able"),
	EMG_DISABLE UMETA(DisplayName = "DisAble"),

	EMG_DEFAULTMAX UMETA(DisplayName = "DefaultMax")
};

UENUM(BlueprintType)
enum class EMonsterMoveState : uint8
{
	EMMS_IDLE					UMETA(DisplayName = "IDLE"),
	EMMS_FORWARD			UMETA(DisplayName = "Forward"),
	EMMS_LEFT					UMETA(DisplayName = "Left"),
	EMMS_RIGHT					UMETA(DisplayName = "Right"),
	EMMS_BACK					UMETA(DisplayName = "Back"),
	EMMS_FORWARD_2			UMETA(DisplayName = "Forward_2"),

	EMMS_DEFAULTMAX	UMETA(DisplayName = "DefaultMax")
};

UENUM(BlueprintType)
enum class EMonsterType: uint8
{
	EMT_OTHER			UMETA(DisplayName = "Other"),
	EMT_TOYMON			UMETA(DisplayName = "Toymon"),
	EMT_FORKYDOLL			UMETA(DisplayName = "ForkyDoll"),
	EMT_BOMBER			UMETA(DisplayName = "Bomber"),
	EMT_UDER			UMETA(DisplayName = "Uder"),
	EMT_HITABLEBLOCK	UMETA(DisplayName = "HitableBlock"),
	EMT_DOGGY			UMETA(DisplayName = "Doggy"),
	EMT_KINGCROWN_2PHASE	UMETA(DisplayName = "KingCrown_2Phase"),

	EMT_DEFAULTMAX			UMETA(DisplayName = "Max"),
};

UENUM(BlueprintType)
enum class EPullType :uint8
{
	EPT_ALLOW		UMETA(DisplayName = "Allow"),
	EPT_DISALLOW	UMETA(DisplayName = "DisAllow"),
	EPT_DEFAULTMAX	UMETA(DisplayName = "Max")
};

DECLARE_MULTICAST_DELEGATE_OneParam(FDieDelegate, AActor*);



UCLASS()
class PROJECTHIT_API AEnemy : public AUnit
{
	GENERATED_BODY()
public:
	// Sets default values for this character's properties
	AEnemy();

	UFUNCTION()
	void AgroSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void Attack();

	FDieDelegate EnemyDie_Del;
protected:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	//virtual
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCallable)
	virtual void Die();

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void Skill_HitParrying_Implementation(AActor* _Actor) override;

	virtual void EndTimer();

	/**Show HealthBar*/
	UFUNCTION(BlueprintNativeEvent)
	void ShowHealthBar();

	void ShowHealthBar_Implementation();

	UFUNCTION(BlueprintImplementableEvent)
	void HideHealthBar();

	void SpawnBlood(FName SocketName, UParticleSystem* Particle, USkeletalMeshComponent* PlayerMesh,float Damage);

	UFUNCTION(BlueprintCallable)
	void FinishDeath();

	void DestroyEnemy();

	UFUNCTION(BlueprintCallable)
	void MoveToTarget(APlayerCharacter* Target);

	FVector GetPlayerLocation();

	FRotator GetLookAtRotationYaw(FVector Target);

	float PlayHighPriorityMontage(UAnimMontage* Montage, FName StartSectionName, float InPlayRate);
	
	void DissolveChange(UMaterialInstanceDynamic* Mat, FName Name,float DeltaTime);

	void SetIsTrueHit();

	UFUNCTION(BlueprintCallable)
	virtual void SetIsFalseHit();
	
	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd();

	bool RetreatCheck();

	void OnDamageColorMat(UMaterialInstanceDynamic* Mat,FName Name, FTimerHandle TimerHandle);

	void FindPlayerCharacter();

	void PlayFModEventSound(class UFMODEvent* Event);

	void Parrying();

	void SetEnemyState(EEnemyState State);

	virtual void KnockDown(APlayerCharacter* Target, int32 index);

	UFUNCTION(BlueprintCallable)
	void KnockDownEnd();

	UFUNCTION(BlueprintCallable)
	void OnGaurdAble();

	UFUNCTION(BlueprintCallable)
	void OffGaurdAble();

	void boundaryCheck(int dir,float Dest,float MoveSpeed);

	virtual void StunStart();

	UFUNCTION(BlueprintCallable)
	virtual void StunEnd();

	UFUNCTION(BlueprintCallable)
	void DieDelNotify();

public:

	void ResetTakeHitDamage(float AttackTiming);

	void Knockback(APlayerCharacter* Target,int32 index);

	UFUNCTION(BlueprintCallable)
	void ExecuteVisitor(FString key);

	void StartAttackCameraShake(); // AttackCameraReact

	UFUNCTION(BlueprintCallable)
	void SetFocusBeh();

protected:
	/**class**/
	UPROPERTY(EditAnywhere, Category = "Behavior Tree", meta = (AllowPrivateAccess = "true"))
	class UBehaviorTree* BehaviorTree;

	class AEnemyController* EnemyController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy | Combat", meta = (AllowPrivateAccess = "true"))
	APlayerCharacter* Player;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enemy | Enum", meta = (AllowPrivateAccess = "true"))
	EMonsterGuardPoint MonsterGuardPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy | Enum", meta = (AllowPrivateAccess = "true"))
	EEnemyState EnemyState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy | Enum", meta = (AllowPrivateAccess = "true"))
	EEnemyAutoTarget EnemyAutoTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy | Enum", meta = (AllowPrivateAccess = "true"))
	EMonsterKnockBackState MonsterKnockBackState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy | Enum", meta = (AllowPrivateAccess = "true"))
	EMonsterGuardable MonsterGuardable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy | Enum", meta = (AllowPrivateAccess = "true"))
	EMonsterMoveState MonsterMoveState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy | Enum", meta = (AllowPrivateAccess = "true"))
	EMonsterType MonsterType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy | Enum", meta = (AllowPrivateAccess = "true"))
	EPullType PullType;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enemy | Combat", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* AgroSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enemy | Combat", meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* TargetWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy | Montage", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DeathMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy | Montage ", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* HitMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy | Montage", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* AttackMontage;

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy | Particles", meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* HitParticles;*/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy | Particles", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* SpawnParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy | Particles", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* BloodParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy | CameraShake", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UMatineeCameraShake> AttackCameraShake;// AttackCameraReact

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy | Properties ", meta = (AllowPrivateAccess = "true"))
	UDataTable* DTEnemyName; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy | Montage ", meta = (AllowPrivateAccess = "true"))
	UFMODEvent* FMOD_KnockBackHitEvent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy | Montage ", meta = (AllowPrivateAccess = "true"))
	UFMODEvent* FMOD_KnockDownHitEvent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy | Montage ", meta = (AllowPrivateAccess = "true"))
	UFMODEvent* FMOD_StunHitEvent;

protected:
	/*Enemy Properties Section*/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy | Properties | DB", meta = (AllowPrivateAccess = "true"))
	float BaseDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy | Properties | DB", meta = (AllowPrivateAccess = "true"))
	float ParryingDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy | Properties | DB", meta = (AllowPrivateAccess = "true"))
	float AcceptableRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy | Properties | DB", meta = (AllowPrivateAccess = "true"))
	float DissolveTimeRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy | Properties | DB", meta = (AllowPrivateAccess = "true"))
	float Dissolve;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enemy | Properties | DB", meta = (AllowPrivateAccess = "true"))
	float HitMontagePlayRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enemy | Properties | DB", meta = (AllowPrivateAccess = "true"))
	float AttackMontagePlayRate;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Enemy | Properties | DB", meta = (AllowPrivateAccess = "true"))
	float RetreatRadius;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Enemy | Properties | DB", meta = (AllowPrivateAccess = "true"))
	float OnDamageMatTime;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Enemy | Properties | DB", meta = (AllowPrivateAccess = "true"))
	float MaxGP;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Enemy | Properties | DB", meta = (AllowPrivateAccess = "true"))
	float GP;

protected:
	/*Bool Section*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enemy | Bool", meta = (AllowPrivateAccess = "true"))
	bool bHitDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy | Bool | DB", meta = (AllowPrivateAccess = "true"))
	bool bAgroInit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy | Bool | DB", meta = (AllowPrivateAccess = "true"))
	bool bSpawnEffect;

	UPROPERTY()
	bool bAttackRotChar = false;

protected:
	/*Other Section*/
		
/**Timer Section***/
private:
	//Character Attack -> bHitDamage Timer
	FTimerHandle CharacterComboTimer;

	void ComboTimerReset();

	//HealthBar Timer
	FTimerHandle HealthBarTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy | Timer", meta = (AllowPrivateAccess = "true"))
	float HealthBarDisplayTime;

	//DeathTimer
	FTimerHandle DeathTimer;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category ="Enemy | Timer", meta = (AllowPrivateAccess = "true"))
	float DeathTime;


	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<class AActor>> DropAssets;

	UPROPERTY(EditAnywhere)
	FVector4 DropOffset;
	
protected:
	FTimerHandle HitStopTimer;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enemy | Properties | DB", meta = (AllowPrivateAccess = "true"))
	float CurShiledRatio;

private:
	FTimerHandle MatTimer;

	float HitStopTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy | Properties ", meta = (AllowPrivateAccess = "true"))
	UDataTable* MonsterMasterTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy | Properties ", meta = (AllowPrivateAccess = "true"))
	FString TableKey;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Enemy | Properties | DB", meta = (AllowPrivateAccess = "true"))
	float MoveLeftSpeed;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Enemy | Properties | DB", meta = (AllowPrivateAccess = "true"))
	float MoveRightSpeed;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Enemy | Properties | DB", meta = (AllowPrivateAccess = "true"))
	float MoveBackSpeed;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Enemy | Properties | DB", meta = (AllowPrivateAccess = "true"))
	TArray<FHitParticle> HitParticles;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Enemy | Properties | DB", meta = (AllowPrivateAccess = "true"))
	TArray<FEnemyHitInfo> EnemyInfo;

protected:
	FTimerHandle AttackCoolDownTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy | Timer", meta = (AllowPrivateAccess = "true"))
		float AttackCoolDownTime;

	std::map<FString,VisitorPtr> Visitors;
public:
	virtual void Accept(VisitorPtr Visitor) {};
	
public:
	FORCEINLINE bool GetHitDamage() const { return bHitDamage; }
	FORCEINLINE UBehaviorTree* GetBehaviorTree() const { return BehaviorTree; }
	FORCEINLINE float GetParryingDamge() const { return ParryingDamage; }
	
	
	//FORCEINLINE UParticleSystem* GetHitParticles() const { return HitParticles; }
	FORCEINLINE void SetHitDamage(bool bHit) { bHitDamage = bHit; }
	FORCEINLINE FString GetTableKey() const { return TableKey; }
	FORCEINLINE void SetTableKey(FString tablekey) {TableKey = tablekey;}

	FORCEINLINE void SetMonsterGuardPoint(EMonsterGuardPoint State) { MonsterGuardPoint = State; }
	FORCEINLINE EMonsterGuardPoint GetMonsterGuardPoint() const { return MonsterGuardPoint; }	

	FORCEINLINE void SetPullType(EPullType State) { PullType = State; }
	FORCEINLINE EPullType GetPullType() const { return PullType; }

	FORCEINLINE float GetShileDamageRatio() const { return CurShiledRatio; }

	FORCEINLINE EEnemyState GetEnemyState() const { return EnemyState; }

	FORCEINLINE EEnemyAutoTarget GetEnemyAutoTarget() const { return EnemyAutoTarget; }
	FORCEINLINE void SetEnemyAutoTarget(EEnemyAutoTarget State) { EnemyAutoTarget = State; }

	FORCEINLINE EMonsterKnockBackState GetMonsterKnockBackState() const { return MonsterKnockBackState; }
	FORCEINLINE void SetMonsterKnockBackState(EMonsterKnockBackState State) { MonsterKnockBackState = State; }

	FORCEINLINE float GetMaxGP() const { return MaxGP; }
	FORCEINLINE void SetMaxGP(float _MaxGp) { MaxGP = _MaxGp; }

	FORCEINLINE float GetGP() const { return GP; }
	FORCEINLINE void SetGP(float _GP) { GP = _GP; }

	FORCEINLINE void SetMonsterGuardable(EMonsterGuardable State) { MonsterGuardable = State; }
	FORCEINLINE EMonsterGuardable GetMonsterGuardAble()const { return MonsterGuardable; }

	FORCEINLINE void SetMonsterMoveState(EMonsterMoveState State) { MonsterMoveState = State; }
	FORCEINLINE EMonsterMoveState GetMonsterMoveState() const { return MonsterMoveState; }

	FORCEINLINE FHitParticle GetHitParticles(int32 index) { return HitParticles[index]; }
	FORCEINLINE bool IsHitParticlesValid(int32 index) { return HitParticles.IsValidIndex(index); }

	FORCEINLINE void SetMonsterType(EMonsterType State) { MonsterType = State; }
	FORCEINLINE EMonsterType GetMonsterType() const { return MonsterType; }

	FORCEINLINE float GetMonsterbackSpeed() const { return MoveBackSpeed; }

	void TargetWidgetSetVisible();
	void TargetWidgetSetHidden();

};
