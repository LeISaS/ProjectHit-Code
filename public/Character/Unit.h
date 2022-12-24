#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/Skillinteractive.h"
#include "Unit.generated.h"

UENUM(BlueprintType)
enum class ESelectHitStop : uint8
{
	ESHS_SELF				UMETA(DisplayName = "Self"),
	ESHS_TARGET			UMETA(DisplayName = "Target"),
	ESHS_TOGETHER		UMETA(DisplayName = "Together"),
	ESHS_WORLD			UMETA(DisplayName = "world"),
	
	ESHS_DEFAULTMAX	UMETA(DisplayName = "DefaultMax")
};

UCLASS()
class PROJECTHIT_API AUnit : public ACharacter,public ISkillinteractive
{
	GENERATED_BODY()

public:
	AUnit();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

protected:
	virtual float DoDamage(class AActor* Victim, float Damage, class AController* _Controller);

	void StartHitStop(float _HitStopTime = 0.2f, ESelectHitStop _HitStop = ESelectHitStop::ESHS_TOGETHER, AActor* _Target = nullptr);

	UPROPERTY()
	ESelectHitStop SelectHitStop;

	UFUNCTION(BlueprintCallable)
	void ActivateCollision(class UShapeComponent* _Collision);
	UFUNCTION(BlueprintCallable)
	void DeactivateCollision(UShapeComponent* _Collision);

	UMaterialInstanceDynamic* SetDynamicMat(UMaterialInstance* _Instance, UMaterialInstanceDynamic* _DynamicInstance, int value, AActor* _Actor);

	void SetMIDScalarParameterValue(UMaterialInstanceDynamic* MaterialInstance, FName PName, float Value);

	//���ο� ��� �߰����� (8��8��)

protected:
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Unit | Status")
	float MaxHp;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Unit | Status")
	float Hp;

	UPROPERTY()
	FTimerHandle HitStopTimerHandle;
	
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Unit | Status", meta = (AllowPrivateAccess = "true"))
	float ShieldKnockbackPower;
	
public:
	virtual int GetShieldKnockbackPower_Implementation() override {return ShieldKnockbackPower; }
	
public:
	FORCEINLINE float GetMaxHp() const { return MaxHp; }
	FORCEINLINE void SetMaxHp(float _MaxHp) { MaxHp = _MaxHp; }
	
	FORCEINLINE float GetHp() const { return Hp; }
	FORCEINLINE void SetHp(float _Hp) { Hp = _Hp; }

	FORCEINLINE ESelectHitStop GetSelectHitStop() const { return SelectHitStop; }
	FORCEINLINE void SetSelectHitStop(ESelectHitStop HitStop) { SelectHitStop = HitStop; }
};
