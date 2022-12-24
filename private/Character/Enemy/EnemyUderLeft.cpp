// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Enemy/EnemyUderLeft.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../../../Public/Character/Enemy/EnemyController.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "../../../Public/Character/PlayerCharacter/PlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Particles/ParticleSystemComponent.h"

AEnemyUderLeft::AEnemyUderLeft() :
	UderAnimLeft(EUderAnimLeft::EUAL_OFF)
{
	PrimaryActorTick.bCanEverTick = true;

	Visitors.insert(std::make_pair("Die", std::make_shared<DieVisitor>()));
	Visitors.insert(std::make_pair("Attack", std::make_shared<AttackVisitor>()));
	Visitors.insert(std::make_pair("Rush", std::make_shared<RushVisitor>()));

}

void AEnemyUderLeft::BeginPlay()
{
	Super::BeginPlay();


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

	LeftHp = GetMaxHp();
	EnemyController->GetBlackBoardComponent()->SetValueAsFloat(FName("LeftHp"), LeftHp);
}

void AEnemyUderLeft::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetEnemyState() == EEnemyState::EES_DEAD)
		return;

	if (EnemyController->GetBlackBoardComponent()->GetValueAsEnum(FName("EUderActivity")) == uint8(EUderActivity::EUA_LEFT))
	{
		OnInvincibility();
	}
	else
	{
		OffInvincibility();
	}

	if (GetDistanceTo(Player) <= UderDistance)
	{

		if (EnemyController->GetBlackBoardComponent()->GetValueAsEnum("EUderNumbering") == uint8(EUderNumbering::EUN_NONE))
		{
			if (!EnemyController->GetBlackBoardComponent()->GetValueAsBool("bRushCoolDown"))
				SetMonsterMoveState(EMonsterMoveState::EMMS_FORWARD);
			else
			{
				if (!(GetEUderInvincibility() == EUderInvincibility::EUI_INVINCIBILITY))
					SetMonsterMoveState(EMonsterMoveState::EMMS_BACK);
			}
		}
		else
		{
			if (!EnemyController->GetBlackBoardComponent()->GetValueAsBool("bAttackCoolDown"))
				SetMonsterMoveState(EMonsterMoveState::EMMS_FORWARD);
			else
			{
				if (!(GetEUderInvincibility() == EUderInvincibility::EUI_INVINCIBILITY))
					SetMonsterMoveState(EMonsterMoveState::EMMS_BACK);
			}
		}
		
	}
	else
		SetMonsterMoveState(EMonsterMoveState::EMMS_FORWARD);


	if (GetEUderInvincibility() == EUderInvincibility::EUI_INVINCIBILITY && bInvi)
	{
		boundaryCheck(-1, 180.f, GetMonsterbackSpeed());
	}

}

void AEnemyUderLeft::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void AEnemyUderLeft::Attack()
{
	Super::Attack();
	SetUderStateLeftEnum(EUderStateLeft::EUSL_ATTACK);
}

void AEnemyUderLeft::AttackEnd()
{
	Super::AttackEnd();
	SetUderStateLeftEnum(EUderStateLeft::EUSL_IDLE);
}

void AEnemyUderLeft::RushAttack()
{
	Super::RushAttack();
	SetUderStateLeftEnum(EUderStateLeft::EUSL_RUSH);
}
void AEnemyUderLeft::RushEnd()
{
	Super::RushEnd();
	SetUderStateLeftEnum(EUderStateLeft::EUSL_IDLE);
}
void AEnemyUderLeft::SetIsFalseHit()
{
	Super::SetIsFalseHit();

	SetUderStateLeftEnum(EUderStateLeft::EUSL_IDLE);
	EnemyController->GetBlackBoardComponent()->SetValueAsEnum(FName("EUderAttackState"), uint8(EUderAttackState::EUAS_NOATTACKING));
}

void AEnemyUderLeft::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

float AEnemyUderLeft::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
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
		LeftHp = EnemyController->GetBlackBoardComponent()->GetValueAsFloat(FName("Monster_Hp"));

		TotalHp = LeftHp + RightHp;
		EnemyController->GetBlackBoardComponent()->SetValueAsFloat(FName("TotalHp"), TotalHp);
		EnemyController->GetBlackBoardComponent()->SetValueAsFloat(FName("LeftHp"), LeftHp);
	}

	uint8 State = EnemyController->GetBlackBoardComponent()->GetValueAsEnum(FName("EUderStateLeft"));
	uint8 AttackState = EnemyController->GetBlackBoardComponent()->GetValueAsEnum(FName("EUderAttackState"));
	if (DamageCauser)
	{
		auto Character = Cast<APlayerCharacter>(DamageCauser);

		if (Character)
		{
			if (State == uint8(EUderStateLeft::EUSL_IDLE) && AttackState == uint8(EUderAttackState::EUAS_NOATTACKING))
			{
				if (Character->GetKnockBackStatus() == EKnockBackState::EKBS_STUN)
				{
					if (GetMonsterGuardPoint() == EMonsterGuardPoint::EMGP_ON)
					{
						StunStart();
						Knockback(Character, Character->GetCurrentSkillID());
						bAttackRotChar = true;
					}
					
				}
				else
				{
					if (GetMonsterGuardPoint() == EMonsterGuardPoint::EMGP_ON)
					{
						SetIsTrueHit();
						Knockback(Character, Character->GetCurrentSkillID());
						bAttackRotChar = true;
					}
				}
			}

			if (MonsterGuardPoint == EMonsterGuardPoint::EMGP_OFF )
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
				EnemyController->GetBlackBoardComponent()->SetValueAsEnum(FName("EUderStateLeft"), 0);
				EnemyController->GetBlackBoardComponent()->SetValueAsEnum(FName("EUderAttackState"), 0);
			}
		}
	}

	return DamageAmount;
}

void AEnemyUderLeft::AttackRangeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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

void AEnemyUderLeft::AttackRangeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
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

void AEnemyUderLeft::RightHandOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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

void AEnemyUderLeft::RushRangeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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

void AEnemyUderLeft::RushRangeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
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

void AEnemyUderLeft::RushBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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

void AEnemyUderLeft::OnInvincibility()
{

	if (EnemyController->GetBlackBoardComponent()->GetValueAsEnum(FName("EUderInvincibility")) == uint8(EUderInvincibility::EUI_INVINCIBILITY))
		return;
	
	if (EnemyController)
	{
		UAnimInstance* AnimInstacne = GetMesh()->GetAnimInstance();
		if (AnimInstacne->Montage_IsPlaying(AttackMontage))
		{
			AnimInstacne->Montage_Stop(0.f);
		}
		SetUderStateLeftEnum(EUderStateLeft::EUSL_IDLE);
		SetEnemyState(EEnemyState::EES_MOVE);
		EnemyController->GetBlackBoardComponent()->SetValueAsEnum(FName("EUderAttackState"), uint8(EUderAttackState::EUAS_NOATTACKING));
		EnemyController->StopMovement();
	}


	SetEUderInvincibilityEnum(EUderInvincibility::EUI_INVINCIBILITY);
	SetEUderAnimLeft(EUderAnimLeft::EUAL_ON);

	/*GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);*/
	SetEnemyAutoTarget(EEnemyAutoTarget::EEAT_NOTARGET);


	GetPS_BlueParticleComponent()->SetFloatParameter(FName("UderBarrierFade"),0);
	GetPS_RedParticleComponent()->SetFloatParameter(FName("UderBarrierFade"), 0);
}

void AEnemyUderLeft::OffInvincibility()
{

	if (EnemyController->GetBlackBoardComponent()->GetValueAsEnum(FName("EUderInvincibility")) == uint8(EUderInvincibility::EUI_NONE))
		return;

	SetEUderInvincibilityEnum(EUderInvincibility::EUI_NONE);
	SetEUderAnimLeft(EUderAnimLeft::EUAL_OFF);

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

void AEnemyUderLeft::Die()
{
	Player->RemoveCombatTarget(this);
	
	//if (!Player->GetArrayCombatTarget().Num())
	//{
	//	Player->UpdateUderHUD(false);
	//}

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

void AEnemyUderLeft::SetUderStateLeftEnum(EUderStateLeft State)
{
	if (EnemyController)
	{
		SetUderStateLeft(State);
		EnemyController->GetBlackBoardComponent()->SetValueAsEnum(FName("EUderStateLeft"), uint8(State));
	}
}

void AEnemyUderLeft::Accept(VisitorPtr Visitor)
{
	Visitor->Visit(this);
}
