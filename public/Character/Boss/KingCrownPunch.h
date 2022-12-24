// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Interface/Skillinteractive.h"
#include "Components/TimelineComponent.h"
#include "KingCrownPunch.generated.h"

UENUM(BlueprintType)
enum class EPunchState : uint8
{
	EPS_IDLE		UMETA(DisplayName = "IDLE"),
	EPS_ATTACK		UMETA(DisplayName = "ATTACK"),
	EPS_NOHIT		UMETA(DisplayName = "NOHIT"),
	EPS_HIT			UMETA(DisplayName = "HIT"),
	EPS_Reverse		UMETA(DisplayName = "REVERSE"),
	EPS_PARRING		UMETA(DisplayName = "PARRING"),
	EPS_DESTROY		UMETA(DisplayName = "Destroy"),

	EPS_MAX			UMETA(DisplayName = "DEFAULTMAX")
};

UCLASS()
class PROJECTHIT_API AKingCrownPunch : public AActor, public ISkillinteractive
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AKingCrownPunch();

	UFUNCTION()
		void MovementLine(float Value);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void FindPlayer();

	void FindKingCrown();

	void UpdateTarget(EPunchState State);

	void DelayLogic(float DeltaTime);

	UFUNCTION()
	void CompBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void CurveTimerStart();

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Punch | Class", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* CollisionComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Punch | Class", meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* PunchMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Punch | Class", meta = (AllowPrivateAccess = "true"))
	class UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Punch | Class", meta = (AllowPrivateAccess = "true"))
	class APlayerCharacter* Player;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Punch | Enum", meta = (AllowPrivateAccess = "true"))
	EPunchState PunchState;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Punch | Class", meta = (AllowPrivateAccess = "true"))
	class AKingCrown* kingCrown;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Punch | Class", meta = (AllowPrivateAccess = "true"))
	class USplineComponent* SplineComponent;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Punch | Class", meta = (AllowPrivateAccess = "true"))
	class UStaticMesh* Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Punch | Class", meta = (AllowPrivateAccess = "true"))
	class AActor* Target;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Punch | Class", meta = (AllowPrivateAccess = "true"))
	TArray<class USplineMeshComponent*> MySpline;

	FTimeline MovementTimeline;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Punch | Curve", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* MovementCurve;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Punch | Class", meta = (AllowPrivateAccess = "true"))
	class USplineMeshComponent* SplineMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Punch | Particles", meta = (AllowPrivateAccess = "true"))
	UParticleSystemComponent* PS_WindParticle;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Punch | Particles", meta = (AllowPrivateAccess = "true"))
	//UParticleSystem* PS_SuccessParring;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Punch | Particles", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* PS_PunchHit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Punch | Particles", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* PS_PunchHitBoss;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Punch | Particles", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* PS_PunchBroken;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Punch | FMOD", meta = (AllowPrivateAccess = "true"))
	class UFMODEvent* FMOD_Start;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Punch | FMOD", meta = (AllowPrivateAccess = "true"))
	UFMODEvent* FMOD_GroundHit;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Punch | FMOD", meta = (AllowPrivateAccess = "true"))
	UFMODEvent* FMOD_KingCrownHit;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Punch | FMOD", meta = (AllowPrivateAccess = "true"))
	UFMODEvent* FMOD_PlayerHit;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Punch | FMOD", meta = (AllowPrivateAccess = "true"))
	UFMODEvent* FMOD_Broken;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Punch | FMOD", meta = (AllowPrivateAccess = "true"))
	UFMODEvent* FMOD_PlayerParrying;

private:
	UPROPERTY()
	bool bNoTarget;

	UPROPERTY()
	float DelayTimer;

	UPROPERTY()
	bool bhasFinishedDelay;

	UPROPERTY()
	bool bSpline;

	UPROPERTY()
	int PunchSplineValue;

	UPROPERTY()
	int SplineParringValue=0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Punch | Properties", meta = (AllowPrivateAccess = "true"))
	float PunchDamage;

private:

	FTimerHandle PlayerHandle;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Punch | Timer", meta = (AllowPrivateAccess = "true"))
	float playerTime;

	FTimerHandle GroundHandle;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Punch | Timer", meta = (AllowPrivateAccess = "true"))
	float GroundTime;

	FTimerHandle SplineHandle;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Punch | Timer", meta = (AllowPrivateAccess = "true"))
	float SplineTime;
	
	FTimerHandle HitStopTimer; 
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Punch | Camera", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UMatineeCameraShake> AttackCameraShake;
	
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Punch | Properties", meta = (AllowPrivateAccess = "true"))
	float ShieldKnockbackPower;
	
public:	
	virtual void Tick(float DeltaTime) override;

	virtual int GetShieldKnockbackPower_Implementation() override { return ShieldKnockbackPower;}
	
	FORCEINLINE void SetPunchState(EPunchState State) { PunchState = State; }
	FORCEINLINE EPunchState GetPunchState() const { return PunchState; }
};
