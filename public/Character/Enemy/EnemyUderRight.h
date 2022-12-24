// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Enemy/EnemyUder.h"
#include "EnemyUderRight.generated.h"

UENUM(BlueprintType)
enum class EUderStateRight : uint8
{
	EUSR_IDLE			 UMETA(DisplayName = "Idle"),
	EUSR_ATTACK	 UMETA(DisplayName = "Attack"),
	EUSR_RUSH		 UMETA(DisplayName = "Rush"),
	EUSR_DEFENSE UMETA(DisplayName = "Defense"),
	EUSR_DEAD		UMETA(DisplayName = "Dead"),

	EUSR_MAX			UMETA(DisplayName = "DefaultMax")
};

UENUM(BlueprintType)
enum class EUderAnimRight : uint8
{
	EUAR_OFF			 UMETA(DisplayName = "OFF"),
	EUAR_ON			 UMETA(DisplayName = "ON"),

	EUAR_MAX			 UMETA(DisplayName = "Max")
};

UCLASS()
class PROJECTHIT_API AEnemyUderRight : public AEnemyUder
{
	GENERATED_BODY()
	
public:
	AEnemyUderRight();

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
	EUderStateRight UderStateRight;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Uder | Enum", meta = (AllowPrivateAccess = "true"))
	EUderAnimRight UderAnimRight;




public:
	void SetUderStateRightEnum(EUderStateRight State);

	FORCEINLINE void SetUderStateRight(EUderStateRight State) { UderStateRight = State; }


	FORCEINLINE void SetEUderAnimRight(EUderAnimRight State) { UderAnimRight = State; }
	FORCEINLINE EUderAnimRight GetEUderAnimRight() const { return UderAnimRight; }

};
