// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "KingCrownCraneAIComponent.h"
#include "../Enemy/Enemy.h"
#include "KingCrownCrane.generated.h"

UENUM(BlueprintType)
enum class ECraneStampState : uint8
{
	ECSS_IDLE			UMETA(DisplayName = "IDLE"),
	ECSS_ATTACKREADY	UMETA(DisplayName = "ATTACKREADY"),
	ECSS_ATTACK			UMETA(DisplayName = "ATTACK"),
	ECSS_ATTACKKEEP		UMETA(DisplayName = "ATTACKKEEP"),
	ECSS_ATTACKKEEPEND	UMETA(DisplayName = "ATTACKKEEPEND"),
	ECSS_DEATH			UMETA(DisplayName = "DEATH"),

	ECCS_MAX			UMETA(DisplayName = "DEFAULTMAX")
};

UCLASS()
class PROJECTHIT_API AKingCrownCrane : public AEnemy
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AKingCrownCrane();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void CraneAttackOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void Tick(float DeltaTime) override;

public:	
	void CraneAttackReady(class AOnePhaseKingCrown* kingcrown);

	void CraneLoopingAttackStart(AOnePhaseKingCrown* kingcrown,float AnimPlayRate =1.0f, int PatternEffectNumber = 0);
	
	void CraneLoopingAttackEnd();

	void SetPlayerMoveTarget(bool bMoveTarget = false);
private:
	void CraneAttackStart();

	void CraneAttacking();

	void CraneAttackEnd();

	UFUNCTION(BlueprintCallable)
	void CraneAttackEndNotify();

	UFUNCTION(BlueprintCallable)
	void CraneAttackStartNotify();

private:
	/**Enum var*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Crane | Enum", meta = (AllowPrivateAccess = "true"))
	ECraneStampState CraneStampState;

	/**Collision Var*/
	class UKingCrownCraneAIComponent* KingCrownCraneAIComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Crane | Collision", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* CraneAttackCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Crane | Collision", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* CraneRangeCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Crane | Class", meta = (AllowPrivateAccess = "true"))
	class AOnePhaseKingCrown* KingCrown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crane | Particles", meta = (AllowPrivateAccess = "true"))
	class UParticleSystemComponent* SignParticle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crane | Particles", meta = (AllowPrivateAccess = "true"))
	UParticleSystemComponent* SignParticle_1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crane | Particles", meta = (AllowPrivateAccess = "true"))
	UParticleSystemComponent* SignParticle_3;
	
	class UMaterialInstanceDynamic* DynamicZeroMat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crane | Materials ", meta = (AllowPrivateAccess = "true",DisplayName = "Crane"))
	class UMaterialInstance* ZeroMatInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crane | Montage ", meta = (AllowPrivateAccess = "true",DisplayName = "Crane"))
	class UAnimMontage* LoppingAttackMontage;

	UMaterialInstanceDynamic* DynamicOneMat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crane | Materials ", meta = (AllowPrivateAccess = "true", DisplayName = "Crane"))
	UMaterialInstance* OneMatInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crane | Particles", meta = (AllowPrivateAccess = "true"))
	UParticleSystemComponent* PS_GroogyParticles;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crane | FMOD", meta = (AllowPrivateAccess = "true"))
	class UFMODEvent* FMOD_Sign;


	FTimerHandle LoopingHandle;

	float PatternEffectTime;

private:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crane | AnimRate", meta = (AllowPrivateAccess = "true"))
	float AttackStartPlayRate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly , Category = "Crane | AnimRate", meta = (AllowPrivateAccess = "true"))
	float AttackEndPlayRate;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly , Category = "Crane | Damage", meta = (AllowPrivateAccess = "true"))
	float CraneDamage;

	//Timer 
private:
	FTimerHandle AttackStartTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crane | Timer", meta = (AllowPrivateAccess = "true"))
	float AttackStartTime;

	FTimerHandle AttackKeepTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crane | Timer", meta = (AllowPrivateAccess = "true"))
	float AttackKeepTime;

	FTimerHandle AttackKeepEndTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crane | Timer", meta = (AllowPrivateAccess = "true"))
	float AttackKeepEndTime;

	FTimerHandle AttackIdleTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crane | Timer", meta = (AllowPrivateAccess = "true"))
	float AttackIdleTime;





public:
	FORCEINLINE void SetCraneStampState(ECraneStampState State) { CraneStampState = State; }
	FORCEINLINE ECraneStampState GetCraneStampState() const { return CraneStampState; }



};
