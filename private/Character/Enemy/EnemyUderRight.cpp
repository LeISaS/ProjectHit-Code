// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Enemy/EnemyUderRight.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../../../Public/Character/Enemy/EnemyController.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "../../../Public/Character/PlayerCharacter/PlayerCharacter.h"
#include "..\..\..\Public\Character\Enemy\EnemyUderRight.h"
#include "Components/CapsuleComponent.h"
#include "Particles/ParticleSystemComponent.h"

AEnemyUderRight::AEnemyUderRight() :
	UderAnimRight(EUderAnimRight::EUAR_OFF)
{
	PrimaryActorTick.bCanEverTick = true;

	Visitors.insert(std::make_pair("Die", std::make_shared<DieVisitor>()));
	Visitors.insert(std::make_pair("Attack", std::make_shared<AttackVisitor>()));
	Visitors.insert(std::make_pair("Rush", std::make_shared<RushVisitor>()));
}

void AEnemyUderRight::BeginPlay()
{
	Super::BeginPlay();

	if(Player)
		Player->AddCombatTarget(this);

	GetAttackRangeSphere()->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::AttackRangeBeginOverlap);
	GetAttackRangeSphere()->OnComponentEndOverlap.AddUniqueDynamic(this, &ThisClass::AttackRangeEndOverlap);

	GetRightHandCollision()->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::RightHandOverlap);

	GetRushRangeSphere()->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::RushRangeBeginOverlap);
	GetRushRangeSphere()->OnComponentEndOverlap.AddUniqueDynamic(this, &ThisClass::RushRangeEndOverlap);

	GetRushCollision()->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::RushBeginOverlap);

	GetRightHandCollision()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetRushCollision()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	UderSpawnLevel->Add(this);

	RightHp = GetMaxHp();
	EnemyController->GetBlackBoardComponent()->SetValueAsFloat(FName("RightHp"), RightHp);
}

void AEnemyUderRight::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetEnemyState() == EEnemyState::EES_DEAD)
		return;

	if (EnemyController->GetBlackBoardComponent()->GetValueAsEnum(FName("EUderActivity")) == uint8(EUderActivity::EUA_RIGHT))
	{
		OnInvincibility();
	}
	else
	{
		OffInvincibility();
	}

	if (GetDistanceTo(Player) <= UderDistance)
	{
		if (!EnemyController->GetBlackBoardComponent()->GetValueAsBool("bAttackCoolDown"))
			SetMonsterMoveState(EMonsterMoveState::EMMS_FORWARD);
		else
		{
			if (!(GetEUderInvincibility() == EUderInvincibility::EUI_INVINCIBILITY))
				SetMonsterMoveState(EMonsterMoveState::EMMS_BACK);
		}
	}
	else
		SetMonsterMoveState(EMonsterMoveState::EMMS_FORWARD);


	if (GetEUderInvincibility() == EUderInvincibility::EUI_INVINCIBILITY && bInvi)
	{
		boundaryCheck(-1, 180.f, GetMonsterbackSpeed());
	}

	
}

void AEnemyUderRight::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}
void AEnemyUderRight::Attack()
{
	Super::Attack();
	SetUderStateRightEnum(EUderStateRight::EUSR_ATTACK);
}
void AEnemyUderRight::AttackEnd()
{
	Super::AttackEnd();

	SetUderStateRightEnum(EUderStateRight::EUSR_IDLE);
}

void AEnemyUderRight::RushAttack()
{
	Super::RushAttack();
	SetUderStateRightEnum(EUderStateRight::EUSR_RUSH);
}

void AEnemyUderRight::RushEnd()
{
	Super::RushEnd();

	SetUderStateRightEnum(EUderStateRight::EUSR_IDLE);
}

float AEnemyUderRight::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (GetEUderInvincibility() == EUderInvincibility::EUI_INVINCIBILITY) return -1.f;

	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (GetMonsterGuardPoint() == EMonsterGuardPoint::EMGP_ON && GP > 0)
	{
		if (GP - DamageAmount <= 0.f)
		{
			GP = 0;
			SetMonsterGuardPoint(EMonsterGuardPoint::EMGP_OFF);
		}
		else
		{
			Hp -= DamageAmount * CurShiledRatio;
			GP -= DamageAmount;
		}
	}
	else
	{
		if (Hp - DamageAmount <= 0.f)
			Hp = 0;
		else
			Hp -= DamageAmount;
	}

	if (EnemyController)
	{
		EnemyController->GetBlackBoardComponent()->SetValueAsFloat(FName("Monster_Hp"), GetHp());
		RightHp = EnemyController->GetBlackBoardComponent()->GetValueAsFloat(FName("Monster_Hp"));

		TotalHp = LeftHp + RightHp;
		EnemyController->GetBlackBoardComponent()->SetValueAsFloat(FName("TotalHp"), TotalHp);
		EnemyController->GetBlackBoardComponent()->SetValueAsFloat(FName("RightHp"), RightHp);
	}

	uint8 State = EnemyController->GetBlackBoardComponent()->GetValueAsEnum(FName("EUderStateRight"));
	uint8 AttackState = EnemyController->GetBlackBoardComponent()->GetValueAsEnum(FName("EUderAttackState"));
	if (DamageCauser)
	{
		auto Character = Cast<APlayerCharacter>(DamageCauser);

		if (Character)
		{
			if (State == uint8(EUderStateRight::EUSR_IDLE) && AttackState == uint8(EUderAttackState::EUAS_NOATTACKING))
			{
				if (Character->GetKnockBackStatus() == EKnockBackState::EKBS_STUN)
				{
					StunStart();
					Knockback(Character, Character->GetCurrentSkillID());
					bAttackRotChar = true;
				}
				else
				{
					SetIsTrueHit();
					Knockback(Character, Character->GetCurrentSkillID());
					bAttackRotChar = true;
				}
			}

			if (MonsterGuardPoint == EMonsterGuardPoint::EMGP_OFF)
			{
				if (Character->GetKnockBackStatus() == EKnockBackState::EKBS_STUN)
				{
					StunStart();
					Knockback(Character, Character->GetCurrentSkillID());
					bAttackRotChar = true;
				}
				else
				{
					SetIsTrueHit();
					Knockback(Character, Character->GetCurrentSkillID());
					bAttackRotChar = true;
				}
				EnemyController->GetBlackBoardComponent()->SetValueAsBool(FName("bRushAttack"), false);
				EnemyController->GetBlackBoardComponent()->SetValueAsEnum(FName("EUderStateRight"), 0);
				EnemyController->GetBlackBoardComponent()->SetValueAsEnum(FName("EUderAttackState"), 0);
			}
		}
	}

	return DamageAmount;
}

void AEnemyUderRight::SetIsFalseHit()
{
	Super::SetIsFalseHit();

	SetUderStateRightEnum(EUderStateRight::EUSR_IDLE);
	EnemyController->GetBlackBoardComponent()->SetValueAsEnum(FName("EUderAttackState"), uint8(EUderAttackState::EUAS_NOATTACKING));
}

void AEnemyUderRight::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AEnemyUderRight::AttackRangeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == nullptr) return;

	auto Character = Cast<APlayerCharacter>(OtherActor);

	if (Character)
	{
		if (EnemyController)
		{
			EnemyController->GetBlackBoardComponent()->SetValueAsBool(FName("bAttackRange"), true);
			EnemyController->GetBlackBoardComponent()->SetValueAsBool(FName("bRushAttack"), true);
		}
	}
}
void AEnemyUderRight::AttackRangeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == nullptr) return;

	auto Character = Cast<APlayerCharacter>(OtherActor);

	if (Character)
	{
		if (EnemyController)
		{
			EnemyController->GetBlackBoardComponent()->SetValueAsBool(FName("bAttackRange"), false);
		}
	}
}
void AEnemyUderRight::RightHandOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto Character = Cast <APlayerCharacter>(OtherActor);

	if (Character)
	{
		float damage = DoDamage(Character, GetAttackDamage(), EnemyController);
		SpawnBlood(FName("FX_Socket"), BloodParticles, Character->GetMesh(),damage);
		if (damage > 0.f)
		{
			Character->Stun();
		}
		DeactivateCollision(GetRightHandCollision());
	}
}
void AEnemyUderRight::RushRangeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == nullptr) return;

	auto Character = Cast<APlayerCharacter>(OtherActor);

	if (Character)
	{
		if (EnemyController)
		{
			EnemyController->GetBlackBoardComponent()->SetValueAsBool(FName("bRushAttack"), true);
		}
	}
}
void AEnemyUderRight::RushRangeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == nullptr) return;

	auto Character = Cast<APlayerCharacter>(OtherActor);

	if (Character)
	{
		if (EnemyController)
		{
			EnemyController->GetBlackBoardComponent()->SetValueAsBool(FName("bRushAttack"), false);
		}
	}
}
void AEnemyUderRight::RushBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto Character = Cast<APlayerCharacter>(OtherActor);

	if (Character)
	{
		float damage = DoDamage(Character, GetRushDamage(), EnemyController);
		SpawnBlood(FName("FX_Socket"), BloodParticles, Character->GetMesh(),damage);

		//StartHitStop(0.1f, ESelectHitStop::ESHS_TOGETHER, Character);

		DeactivateCollision(GetRushCollision());

	}
}
void AEnemyUderRight::OnInvincibility()
{

	if (EnemyController->GetBlackBoardComponent()->GetValueAsEnum(FName("EUderInvincibility")) == uint8(EUderInvincibility::EUI_INVINCIBILITY))
		return;

	//UE_LOG(LogTemp, Warning, TEXT("EUA_Right"));

	UAnimInstance* AnimInstacne = GetMesh()->GetAnimInstance();
	if (AnimInstacne->Montage_IsPlaying(AttackMontage))
	{
		AnimInstacne->Montage_Stop(0.f);
	}
	SetUderStateRightEnum(EUderStateRight::EUSR_IDLE);
	SetEnemyState(EEnemyState::EES_MOVE);
	EnemyController->GetBlackBoardComponent()->SetValueAsEnum(FName("EUderAttackState"), uint8(EUderAttackState::EUAS_NOATTACKING));

	SetEUderInvincibilityEnum(EUderInvincibility::EUI_INVINCIBILITY);
	SetEUderAnimRight(EUderAnimRight::EUAR_ON);
	EnemyController->StopMovement();

	/*GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);*/

	SetEnemyAutoTarget(EEnemyAutoTarget::EEAT_NOTARGET);

	GetPS_BlueParticleComponent()->SetFloatParameter(FName("UderBarrierFade"), 0);
	GetPS_RedParticleComponent()->SetFloatParameter(FName("UderBarrierFade"), 0);
	//GetInviParticleComponent()->SetFloatParameter(FName("UderInviFade"), 1);

}
void AEnemyUderRight::OffInvincibility()
{
	if (EnemyController->GetBlackBoardComponent()->GetValueAsEnum(FName("EUderInvincibility")) == uint8(EUderInvincibility::EUI_NONE))
		return;

	SetEUderInvincibilityEnum(EUderInvincibility::EUI_NONE);
	SetEUderAnimRight(EUderAnimRight::EUAR_OFF);

	/*GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);*/
	SetEnemyAutoTarget(EEnemyAutoTarget::EEAT_AUTOTARGET);

	GetInviParticleComponent()->SetFloatParameter(FName("UderInviFade"), 0);

	if (GetGP() > GetMaxGP() / 2)
	{
		GetPS_BlueParticleComponent()->SetFloatParameter(FName("UderBarrierFade"), 1);
		GetPS_RedParticleComponent()->SetFloatParameter(FName("UderBarrierFade"), 0);
	}
	else if (GetGP() <= GetMaxGP() / 2 && GetGP() > 0.f)
	{
		GetPS_BlueParticleComponent()->SetFloatParameter(FName("UderBarrierFade"), 0);
		GetPS_RedParticleComponent()->SetFloatParameter(FName("UderBarrierFade"), 1);;
	}
	else if (GetGP() <= 0.f)
	{
		GetPS_BlueParticleComponent()->SetFloatParameter(FName("UderBarrierFade"), 0);
		GetPS_RedParticleComponent()->SetFloatParameter(FName("UderBarrierFade"), 0);
		GetInviParticleComponent()->SetFloatParameter(FName("UderInviFade"), 0);
	}
}
void AEnemyUderRight::SetUderStateRightEnum(EUderStateRight State)
{
	if (EnemyController)
	{
		SetUderStateRight(State);
		EnemyController->GetBlackBoardComponent()->SetValueAsEnum(FName("EUderStateRight"), uint8(State));
	}
}
void AEnemyUderRight::Die()
{
	if(Player)
		Player->RemoveCombatTarget(this);

	GetAttackRangeSphere()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetRightHandCollision()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetRushRangeSphere()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetRushCollision()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GetAttackRangeSphere()->OnComponentBeginOverlap.RemoveAll(this);
	GetRightHandCollision()->OnComponentBeginOverlap.RemoveAll(this);
	GetRushRangeSphere()->OnComponentBeginOverlap.RemoveAll(this);
	GetRushCollision()->OnComponentBeginOverlap.RemoveAll(this);

	Super::Die();

}

void AEnemyUderRight::Accept(VisitorPtr Visitor)
{
	Visitor->Visit(this);
}