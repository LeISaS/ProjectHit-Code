#include "Character/Unit.h"
#include "Kismet/GameplayStatics.h"
#include "Components/ShapeComponent.h"

AUnit::AUnit():
ShieldKnockbackPower(1000.f)
{
	PrimaryActorTick.bCanEverTick = true;

}
void AUnit::BeginPlay()
{
	Super::BeginPlay();
	
}

void AUnit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

float AUnit::DoDamage(AActor* Victim, float Damage, class AController* _Controller)
{
	if (Victim == nullptr) return -1.f;

	return UGameplayStatics::ApplyDamage(Victim, Damage, _Controller, this, UDamageType::StaticClass());
}

void AUnit::StartHitStop(float _HitStopTime,ESelectHitStop _HitStop, AActor* _Target)
{
	switch (_HitStop)
	{
	case ESelectHitStop::ESHS_SELF:
		CustomTimeDilation = 0.f;
		break;
	case ESelectHitStop::ESHS_TARGET:
		_Target->CustomTimeDilation = 0.f;
		break;
	case ESelectHitStop::ESHS_TOGETHER:
		CustomTimeDilation = 0.f;
		_Target->CustomTimeDilation = 0.f;
		break;
	}

	auto HitStopLamb = this;

	FTimerDelegate HitStopCallback;
	HitStopCallback.BindLambda([HitStopLamb, _Target]() {
		if (IsValid(HitStopLamb))
		{
			HitStopLamb->CustomTimeDilation = 1.0f;
			_Target->CustomTimeDilation = 1.0f;
		}
		});

	GetWorldTimerManager().ClearTimer(HitStopTimerHandle);
	GetWorldTimerManager().SetTimer(HitStopTimerHandle, HitStopCallback, _HitStopTime, false);
}

void AUnit::ActivateCollision(UShapeComponent* _Collision)
{
	_Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AUnit::DeactivateCollision(UShapeComponent* _Collision)
{
	_Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

UMaterialInstanceDynamic* AUnit::SetDynamicMat(UMaterialInstance* _Instance, UMaterialInstanceDynamic* _DynamicInstance, int value, AActor* _Actor)
{
	if (_Instance)
	{
		_DynamicInstance = UMaterialInstanceDynamic::Create(_Instance, _Actor);
		GetMesh()->SetMaterial(value, _DynamicInstance);

		return _DynamicInstance;
	}
	return nullptr;
}

void AUnit::SetMIDScalarParameterValue(UMaterialInstanceDynamic* MID, FName PName, float Value)
{
	if (MID)
		MID->SetScalarParameterValue(PName, Value);
}
