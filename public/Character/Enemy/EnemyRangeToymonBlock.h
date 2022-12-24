// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Interface/Skillinteractive.h"
#include "EnemyRangeToymonBlock.generated.h"

UCLASS()
class PROJECTHIT_API AEnemyRangeToymonBlock : public AActor, public ISkillinteractive
{
	GENERATED_BODY()
	
public:	
	AEnemyRangeToymonBlock();

	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void Tick(float DeltaTime);

	void StartFire(class AActor* Actor ,float Speed,float Arc);

	void HitDetach();

	UFUNCTION()
	void AttackBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void DestroyBlock();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Block | Class", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* AttackBoxComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Block | Class", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* BlockMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Block | Properties", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* BlockPlayerParticle;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Block | Properties", meta = (AllowPrivateAccess = "true"))
	class UFMODEvent* FMOD_BlockPlayerSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Block | Properties", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* BlockGroundParticle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Block | Properties", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* BlockDestroyParticle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss | Particles", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* SpawnBlockParticle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Block | Properties", meta = (AllowPrivateAccess = "true"))
	FVector DestroyBlockPosition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Block | Properties", meta = (AllowPrivateAccess = "true"))
	FVector DestroyBlockScale;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Block | Properties", meta = (AllowPrivateAccess = "true"))
	class UFMODEvent* FMOD_BlockGroundSound;

	class UMaterialInstanceDynamic* DynamicZeroMat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Block | Materials ", meta = (AllowPrivateAccess = "true"))
	class UMaterialInstance* ZeroMatInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Block | Particles", meta = (AllowPrivateAccess = "true"))
	UParticleSystemComponent* TrailParticle;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Block | Properties", meta = (AllowPrivateAccess = "true"))
	float Damage;

	bool bAttackStart;

	FTimerHandle DestoryTimeHandle;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Block | Properties", meta = (AllowPrivateAccess = "true"))
	float BlockDestroyTime;

	UPROPERTY(EditAnywhere,meta = (MakeEditWidget = true))
	FVector ParticleSpawnPosition;

	float Dissolve;

	bool bDeathDissolve;

	float bStartDissolve;
	
	float StartDissolve;

	FTimerHandle StartDissolveTimerHandle;
	
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Block | Properties", meta = (AllowPrivateAccess = "true"))
	float ShieldKnockbackPower;
	
public:
	FORCEINLINE class UStaticMeshComponent* GetBlockMesh() const { return BlockMesh; }
	FORCEINLINE FVector GetParticleSpawnPos() const { return ParticleSpawnPosition; }

	virtual int GetShieldKnockbackPower_Implementation() override { return ShieldKnockbackPower; }

};