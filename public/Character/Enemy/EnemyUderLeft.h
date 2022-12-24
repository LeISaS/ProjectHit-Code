// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Enemy/EnemyUder.h"
#include "EnemyUderLeft.generated.h"

UENUM(BlueprintType)
enum class EUderStateLeft :uint8
{
	EUSL_IDLE			 UMETA(DisplayName = "Idle"),
	EUSL_ATTACK	 UMETA(DisplayName = "Attack"),
	EUSL_RUSH		 UMETA(DisplayName = "Rush"),
	EUSL_DEFENSE UMETA(DisplayName = "Defense"),
	EUSL_DEAD		UMETA(DisplayName = "Dead"),

	EUSL_MAX			UMETA(DisplayName = "DefaultMax")
};

UENUM(BlueprintType)
enum class EUderAnimLeft : uint8
{
	EUAL_OFF			 UMETA(DisplayName = "OFF"),
	EUAL_ON			 UMETA(DisplayName = "ON"),

	EUAL_MAX			 UMETA(DisplayName = "Max")
};


UCLASS()
class PROJECTHIT_API AEnemyUderLeft : public AEnemyUder
{
	GENERATED_BODY()
	
public:
	AEnemyUderLeft();

	void Accept(VisitorPtr Visitor) override;
		
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void Die() override;

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void Attack() override;

	virtual void AttackEnd() override;

	virtual void RushAttack()override;

	virtual void RushEnd() override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	virtual void SetIsFalseHit() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;


protected:
	UFUNCTION()
	void AttackRangeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void AttackRangeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void RightHandOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void RushRangeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()		
	void RushRangeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	UFUNCTION()
	void RushBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void OnInvincibility();

	void OffInvincibility();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Uder | Enum", meta = (AllowPrivateAccess = "true"))
	EUderStateLeft UderStateLeft;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Uder | Enum", meta = (AllowPrivateAccess = "true"))
	EUderAnimLeft UderAnimLeft;

public:
	void SetUderStateLeftEnum(EUderStateLeft State);
	
	FORCEINLINE void SetUderStateLeft(EUderStateLeft State) { UderStateLeft = State; }

	FORCEINLINE void SetEUderAnimLeft(EUderAnimLeft State) { UderAnimLeft = State; }
	FORCEINLINE EUderAnimLeft GetEUderAnimLeft() const { return UderAnimLeft; }

};
