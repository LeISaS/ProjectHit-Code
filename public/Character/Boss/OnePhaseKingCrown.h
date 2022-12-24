// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Enemy/Enemy.h"
#include "Character/Boss/KingCrown.h"
#include "OnePhaseKingCrown.generated.h"


UENUM(BlueprintType)
enum class EKingCrownPhase : uint8
{
	EKCP_ONEPHASE					UMETA(DisplayName = "ONEPHASE"),
	EKCP_TWOPHASE					UMETA(DisplayName = "TWOPHASE"),

	EKCP_MAX						UMETA(DisplayName = "DEFAULTMAX")
};


UENUM(BlueprintType)
enum class EKingCrownPattern : uint8
{
	EKCP_IDLE						UMETA(DisplayName = "IDLE"),
	EKCP_CRANE						UMETA(DisplayName = "CRANE"),
	EKCP_GIFT						UMETA(DisplayName = "GIFT"),
	EKCP_PUNCH						UMETA(DisplayName = "PUNCH"),
	EKCP_SHOCKWAVE					UMETA(DisplayName = "ShockWave"),
	EKCP_LOOPCRANE					UMETA(DisplayName = "LoopCrane"),

	EKCP_MAX						UMETA(DisplayName = "DefaultMax")
};


UENUM(BlueprintType)
enum class EKingCrownState : uint8
{
	EKCS_NOGROGGY					UMETA(DisplayName = "Nogroggry"),
	EKCS_GROGGY						UMETA(DisplayName = "Groggry"),
	EKCS_DIE						UMETA(DisplayName = "Die"),
	ECKS_MAX						UMETA(DisplayName = "DefaultMax")
};


UCLASS()
class PROJECTHIT_API AOnePhaseKingCrown : public AKingCrown
{
	GENERATED_BODY()
	
public:

	AOnePhaseKingCrown();

	void Accept(VisitorPtr Visitor) override;

	void SetPatternEnum(EKingCrownPattern Pattern);

	void SetStateEnum(EKingCrownState State);

	UFUNCTION(BlueprintCallable)
	void KingCrownClear();

	UFUNCTION(blueprintCallable)
	void ActivateOnePhaseKingCrownBehaviorTree();

	void CranePatternStart();

	void CraneLoopPatternEnd();

	virtual void PunchPatternStart() override;

	virtual void GiftPatternStart() override;

	virtual void PunchPatternEnd() override; 

	virtual void Die() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime);

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION()
	void WindCollisionBeginOveralp(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void ShockWaveBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	//CranePattern


	UFUNCTION(BlueprintCallable)
	void CraneLoopPatternStart(float AnimPlayRate = 1.0f,int PatternEffectNumber=0);

	UFUNCTION(BlueprintCallable)
	void SetCraneLoopPattern(int Count,int PatternKey, bool bMoveTarget = false);



	//GiftPattern


	UFUNCTION(blueprintCallable)
	void GiftFireFunc();
	
	UFUNCTION(blueprintCallable)
	void GiftFireEndFunc();

	UFUNCTION(blueprintCallable)
	void GiftPatternEnd();

	//PunchPattern


	UFUNCTION(BlueprintCallable)
	virtual void PunchSpawn() override;

	UFUNCTION(BlueprintCallable)
	void PunchPatternEndNotify();

	//Other
	void StartHit();

	UFUNCTION(blueprintCallable)
	void PlayerCameraCollisionOn();

	UFUNCTION(blueprintCallable)
	void PlayerCameraCollisionOFF();

	virtual void GroggyPatternStart_120() override;

	UFUNCTION(BlueprintCallable)
	void EndShockWave();

	UFUNCTION(BlueprintCallable)
	virtual void UnNotifyCrash() override;

	UFUNCTION(BlueprintCallable)
	void CollisionOverlapChange();



	UFUNCTION(BlueprintCallable)
	virtual void DeathStart() override;

private:
	/**Enum Var*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Boss | Enum", meta = (AllowPrivateAccess = "true"))
	EKingCrownPhase KingCrownPhase;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Boss | Enum", meta = (AllowPrivateAccess = "true"))
	EKingCrownPattern KingCrownPattern;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Boss | Enum", meta = (AllowPrivateAccess = "true"))
	EKingCrownState KingCrownState;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Boss | Collision", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* ShockWaveRange;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Boss | Class", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AKingCrownGiftBox> GiftBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Boss | Class", meta = (AllowPrivateAccess = "true"))
	class AKingCrownCrane* KingCrownCrane;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss | Class", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<ASpawnLevel> KingCrownSpawnLevel;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Boss | Collision", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* WindCollision;


private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss | Damage", meta = (AllowPrivateAccess = "true"))
	float KingCrownShiledRatio;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss | Damage", meta = (AllowPrivateAccess = "true"))
	float ShockWaveDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss | Damage", meta = (AllowPrivateAccess = "true"))
	float PatternWindPower;

	UPROPERTY()
	bool bGiftFire;

	UPROPERTY()
	float GroggyWindTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss | Properties", meta = (AllowPrivateAccess = "true"))
	int CranePatternCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss | Properties", meta = (AllowPrivateAccess = "true"))
	float MaxFireTime;

	UPROPERTY()
	float FireTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss | Properties", meta = (AllowPrivateAccess = "true"))
	int MinFireCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss | Properties", meta = (AllowPrivateAccess = "true"))
	int MaxFireCount;

	UPROPERTY()
	int CalcFireCount;

private:
	FTimerHandle BehaviorTimerHandle;

	FTimerHandle FireTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss | Properties", meta = (AllowPrivateAccess = "true"))
	float ActiveBehaviorTime;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Boss | FMOD", meta = (AllowPrivateAccess = "true"))
	class UFMODEvent* FMOD_GiftBox_Spawn;
	


public:
	FORCEINLINE void SetKingCrownPhase(EKingCrownPhase Phase) { KingCrownPhase = Phase; }
	FORCEINLINE EKingCrownPhase GetKingCrownPhase() const { return KingCrownPhase; }

	FORCEINLINE void SetKingCrownPattern(EKingCrownPattern Pattern) { KingCrownPattern = Pattern; }
	FORCEINLINE EKingCrownPattern GetKingCrownPattern() const { return KingCrownPattern; }

	FORCEINLINE void SetKingCrownState(EKingCrownState State) { KingCrownState = State; }
	FORCEINLINE EKingCrownState GetKingCrownState() const { return KingCrownState; }
	FORCEINLINE USphereComponent* GetShockWaveRange() const { return ShockWaveRange; }

	FORCEINLINE int& GetCranePatternCount() { return CranePatternCount; }
	FORCEINLINE void SetCranePatternCount(int Count) { CranePatternCount = Count; }
};

