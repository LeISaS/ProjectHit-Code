// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SharkBullet.generated.h"

UCLASS()
class PROJECTHIT_API ASharkBullet : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASharkBullet();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category = "Movement")
	class UProjectileMovementComponent* ProjectileMovementComponent;

	UFUNCTION()
	void ExplositionSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void SuggestFire();

	TArray<FHitResult> PlayerParrying();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bullet Properties", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* BulletMesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bullet Properties", meta = (AllowPrivateAccess = "true"))
	float Speed;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bullet Properties", meta = (AllowPrivateAccess = "true"))
	float ProjectileSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bullet Properties", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* ExplositionCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bullet Properties", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* BoomDamageCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Particles", meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* ExplositionParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Properties", meta = (AllowPrivateAccess = "true"))
	float BaseDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Properties", meta = (AllowPrivateAccess = "true"))
	float ParryingSplashDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Properties", meta = (AllowPrivateAccess = "true"))
	float AttackRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Properties", meta = (AllowPrivateAccess = "true"))
	AController* PlayerController;

	FVector SharkLocation;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	FORCEINLINE float GetParryingSplashDamage() { return ParryingSplashDamage; }
	FORCEINLINE void SetSharkLocation(FVector NowSharkLocation) { SharkLocation = NowSharkLocation; }
	FORCEINLINE FVector GetSharkLocation() { return SharkLocation; }
};
