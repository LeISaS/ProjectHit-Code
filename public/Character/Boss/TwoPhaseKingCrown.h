// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Enemy/Enemy.h"
#include "Components/TimelineComponent.h"
#include "TwoPhaseKingCrown.generated.h"


UENUM(BlueprintType)
enum class ETwoBossState : uint8
{
	ETBS_IDLE					UMETA(DisplayName = "Idle"),
	ETBS_SCRATCH				UMETA(DisplayName = "Scratch"),
	ETBS_SCRATCH_2				UMETA(DisplayName = "Scratch_2"),
	ETBS_LEFTTHROW				UMETA(DisplayName = "LeftThrow"),
	ETBS_RIGHTTHROW				UMETA(DisplayName = "RightThrow"),
	ETBS_THROW_1				UMETA(DisplayName = "Throw_1"), //하나씩 던지기
	ETBS_THROW_2				UMETA(DisplayName = "Throw_2"), //5개 던지기 
	ETBS_JUMP					UMETA(DisplayName = "Jump"),	//뒤로 점프

	ETBS_MAX					UMETA(DisplayName = "DefaultMax")
};

UENUM(BlueprintType)
enum class ETwoBossAttackState : uint8
{
	ETBAS_NONE				UMETA(DisplayName = "NoAttack"),
	ETBAS_ATTACKING			UMETA(DisplayName = "Attacking"),

	ETBAS_MAX				UMETA(DisplayName = "DefaultMax")
};


/**
 * 
 */
UCLASS()
class PROJECTHIT_API ATwoPhaseKingCrown : public AEnemy
{
	GENERATED_BODY()
	
public:
	ATwoPhaseKingCrown();

	virtual void Attack() override;

	virtual void AttackEnd() override;

	void RightThrowPatternStart();

	void LeftThrowPatternStart();

	void Throw_1PatternStart();
	
	void Throw_2PatternStart();

	void Accept(VisitorPtr Visitor) override;

	virtual void Die() override;

	void JumpPatternStart();
	
	UFUNCTION()
	void JumpTimelineProgress(float Value);

	FVector GetLocationBezier(FVector StartPoint, FVector Anchor, FVector EndPoint, float Time);

	void AttackScratch_2();

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime);

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION()
	void RightHandAttackOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void LeftHandAttackOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


	UFUNCTION(BlueprintCallable)
	void RightThrowBlockSpawn();

	UFUNCTION(BlueprintCallable)
	void LeftThrowBlockSpawn();

	UFUNCTION(BlueprintCallable)
	void DeattachBlock();

	UFUNCTION(BlueprintCallable)
	void RightThrowPatternEnd();

	UFUNCTION(BlueprintCallable)
	void LeftThrowPatternEnd();

	UFUNCTION(BlueprintCallable)
	void Throw_1PatternEnd();
	
	UFUNCTION(BlueprintCallable)
	void Throw_2PatternSpawn();

	UFUNCTION(BlueprintCallable)
	void Throw_2PatternEnd();

	UFUNCTION(BlueprintCallable)
	void Thorw_2DetachBlock();

	void Start_ShortHit();

	void Start_LongHit();

	virtual void StunEnd() override;

	UFUNCTION(BlueprintCallable)
	void JumpingMove();

	UFUNCTION(BlueprintCallable)
	void JumpSpawnParticle();

	UFUNCTION()
	void OnParticleHit(FName EventName, float EmitterTime, int32 ParticleTime, FVector Location, FVector Velocity, FVector Direction, FVector Normal, FName BoneName, UPhysicalMaterial* PhysMat);

	FVector GetRandomPosition(int index);

	UFUNCTION(BlueprintCallable)
	void JumpPatternEnd();



private:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Boss | Enum", meta = (AllowPrivateAccess = "true"))
	ETwoBossState TwoBossState;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Boss | Enum", meta = (AllowPrivateAccess = "true"))
	ETwoBossAttackState TwoBossAttackState;

	class UMaterialInstanceDynamic* DynamicZeroMat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss | Materials ", meta = (AllowPrivateAccess = "true", DisplayName = "KingCrown"))
	class UMaterialInstance* ZeroMatInstance;

	UMaterialInstanceDynamic* DynamicOneMat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss | Materials ", meta = (AllowPrivateAccess = "true", DisplayName = "Facial"))
	UMaterialInstance* OneMatInstance;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Boss | Combat", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* RightHandCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Boss | Combat", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* LeftHandCollision;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Boss | Class", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AEnemyRangeToymonBlock> BlockClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Boss | Class", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AEnemyRangeToymonBlock> Block_2Class;

	AEnemyRangeToymonBlock* Block;
	AEnemyRangeToymonBlock* Block_2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Boss | Montage", meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* RightThrowMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Boss | Montage", meta = (AllowPrivateAccess = "true"))
	float RightThrowAnimTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Boss | Montage", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* LeftThrowMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Boss | Montage", meta = (AllowPrivateAccess = "true"))
	float LeftThrowAnimTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Boss | Montage", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* Throw_1Montage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Boss | Montage", meta = (AllowPrivateAccess = "true"))
	float Throw_1AnimTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Boss | Montage", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* Throw_2Montage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Boss | Montage", meta = (AllowPrivateAccess = "true"))
	float Throw_2AnimTime;	

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Boss | Montage", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* Scrach_2Montage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Boss | Montage", meta = (AllowPrivateAccess = "true"))
	float Scrach_2AnimTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Boss | Montage", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* ShortHitMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Boss | Montage", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* LongHitMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss | Combat", meta = (AllowPrivateAccess = "true"))
		TSoftObjectPtr<class ATwoPhaseJumpActor> JumpActor_1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss | Combat", meta = (AllowPrivateAccess = "true"))
		TSoftObjectPtr<ATwoPhaseJumpActor> JumpActor_2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss | Combat", meta = (AllowPrivateAccess = "true"))
		TSoftObjectPtr<ATwoPhaseJumpActor> JumpActor_3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss | Combat", meta = (AllowPrivateAccess = "true"))
		TSoftObjectPtr<ATwoPhaseJumpActor> JumpActor_4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss | Combat", meta = (AllowPrivateAccess = "true"))
	TArray<TSoftObjectPtr<ATwoPhaseJumpActor>> Arr_JumpActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss | Particle", meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* PS_2PageShockWave;

	FTimeline JumpCurveFTimeline;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss | Combat", meta = (AllowPrivateAccess = "true"))
	class UCurveFloat* JumpCurve;


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Boss | Montage", meta = (AllowPrivateAccess = "true"))
		UAnimMontage* JumpMontage;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "FMOD", meta = (AllowPrivateAccess = "true"))
	class UFMODEvent* FMOD_BlockSpawn;
private:
	FTimerHandle MatTimer_0;
	FTimerHandle RightThrowHandle;
	FTimerHandle LeftThrowHandle;
	FTimerHandle Throw_1Handle;
	FTimerHandle Throw_2Handle;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Boss | Timer", meta = (AllowPrivateAccess = "true"))
	float RightThrowAttackCoolDown;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Boss | Timer", meta = (AllowPrivateAccess = "true"))
	float LeftThrowAttackCoolDown;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Boss | Timer", meta = (AllowPrivateAccess = "true"))
	float Throw_1AttackCoolDown;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Boss | Timer", meta = (AllowPrivateAccess = "true"))
	float Throw_2AttackCoolDown;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Boss | Timer", meta = (AllowPrivateAccess = "true"))
	int Throw_2AttackCount;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Boss | Timer", meta = (AllowPrivateAccess = "true"))
	int Throw_2ThrowCount;

	TArray<AEnemyRangeToymonBlock*> Throw_2BlockInfo;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Boss | Speed", meta = (AllowPrivateAccess = "true"))
	float Throw_2PatternSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Boss | Speed", meta = (AllowPrivateAccess = "true"))
	float PatternSpeed;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Boss | Combat", meta = (AllowPrivateAccess = "true"))
	bool bAttacking;

	bool bCondition;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Boss | Combat", meta = (AllowPrivateAccess = "true"))
	float TwoPhaseShockWaveParticleDamage;

	FVector JumpStartLoc;
	FVector JumpTargetLoc;
	FVector JumpAnchor;
	FVector JumpResultLoc;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Boss | Speed", meta = (AllowPrivateAccess = "true"))
	float PatternLeftSpeed;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Boss | Speed", meta = (AllowPrivateAccess = "true"))
	float PatternRightSpeed;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Boss | Speed", meta = (AllowPrivateAccess = "true"))
	float PatternThrow_1Speed;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Boss | Speed", meta = (AllowPrivateAccess = "true"))
	float PatternArc;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Boss | Speed", meta = (AllowPrivateAccess = "true"))
	float PatternLeftArc;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Boss | Speed", meta = (AllowPrivateAccess = "true"))
	float PatternRightArc;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Boss | Speed", meta = (AllowPrivateAccess = "true"))
	float PatternThrow_1Arc;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Boss | Speed", meta = (AllowPrivateAccess = "true"))
	float PatternThrow_2Arc;

public:
	FORCEINLINE void SetTwoBossState(ETwoBossState Phase) { TwoBossState = Phase; }
	FORCEINLINE ETwoBossState GetTwoBossState() const { return TwoBossState; }

	void SetTwoBossPattern(ETwoBossState State);

	FORCEINLINE void SetTwoBossAttackState(ETwoBossAttackState Phase) { TwoBossAttackState = Phase; }
	FORCEINLINE ETwoBossAttackState GetTwoBossAttackState() const { return TwoBossAttackState; }

	void SetTwoBossAttackPattern(ETwoBossAttackState State);

	UFUNCTION(BlueprintCallable)
	void SetTwoBossNoAttack();
};