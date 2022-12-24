// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Enemy/EnemyForkyDoll.h"
#include "Components/BoxComponent.h"
#include "../../../Public/Character/Enemy/Enemy.h"
#include "../../../Public/Character/Enemy/EnemyController.h"
#include "../../../Public/Character/PlayerCharacter/PlayerCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/EngineTypes.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DrawDebugHelpers.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"

AEnemyForkyDoll::AEnemyForkyDoll() :
	AttackRushPower(10000.f),
	KnockDownMontagePlayRate(1.0f),
	RushTime(1.1f),
	RushEndTime(0.3f)
{
	PrimaryActorTick.bCanEverTick = true;

	RightHandCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("RightHandCollsiion"));
	RightHandCollision->SetupAttachment(GetMesh(), FName("RightHandBone"));

	AttackRangeSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AttackRangeCollision"));
	AttackRangeSphere->SetupAttachment(GetRootComponent());
	
	HealthBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidget"));
	HealthBarWidget->SetupAttachment(GetRootComponent());

	PS_Stun = CreateDefaultSubobject< UParticleSystemComponent>(TEXT("PS_Stun"));
	PS_Stun->SetupAttachment(GetRootComponent());

	Visitors.insert(std::make_pair("Die", std::make_shared<DieVisitor>()));
	Visitors.insert(std::make_pair("Attack", std::make_shared <AttackVisitor>()));
	
	RightHandCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	RightHandCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	RightHandCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
}


void AEnemyForkyDoll::BeginPlay()
{
	Super::BeginPlay();

	PS_Stun->SetFloatParameter(FName("StunFade"), 0);


	SetMonsterMoveState(EMonsterMoveState::EMMS_FORWARD);

	AttackRangeSphere->OnComponentBeginOverlap.AddUniqueDynamic(this, &AEnemyForkyDoll::AttackRangeBeginOverlap);
	AttackRangeSphere->OnComponentEndOverlap.AddUniqueDynamic(this, &AEnemyForkyDoll::AttackRangeEndOverlap);

	RightHandCollision->OnComponentBeginOverlap.AddUniqueDynamic(this, &AEnemyForkyDoll::RightHandOverlap);

	RightHandCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if(GetDistanceTo(Player)<= AttackRangeSphere->GetScaledSphereRadius())
		EnemyController->GetBlackBoardComponent()->SetValueAsBool(FName("IsAttackRange"), true);

}


void AEnemyForkyDoll::OnConstruction(const FTransform& Transform)
{
	SetMonsterType(EMonsterType::EMT_FORKYDOLL);
	Super::OnConstruction(Transform);

	DynamicZeroMat = SetDynamicMat(ZeroMatInstance, DynamicZeroMat, 0, this);	//Fork
	DynamicOneMat = SetDynamicMat(OneMatInstance, DynamicOneMat, 1, this);	//Cloth
}

void AEnemyForkyDoll::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetCharacterMovement()->IsFalling())
	{
		GetCharacterMovement()->FallingLateralFriction = 5.0f;
		GetCharacterMovement()->GravityScale = 300.0f;
	}
	else
	{
		GetCharacterMovement()->FallingLateralFriction = 0.8f;
		GetCharacterMovement()->GravityScale = 1.0f;
	}
	
	if (GetEnemyState() == EEnemyState::EES_DEAD)
	{
		float DissolveTime = DissolveTimeRate * DeltaTime;

		DissolveChange(DynamicZeroMat, FName("Dissolve_Con"), DissolveTime);
		DissolveChange(DynamicOneMat, FName("Dissolve_Con"), DissolveTime);
	}

	if (GetEnemyState() == EEnemyState::EES_DEAD)
		return;

	if (!EnemyController->GetBlackBoardComponent()->GetValueAsBool("IsAttackRange") &&
		!EnemyController->GetBlackBoardComponent()->GetValueAsBool("bAttackCoolDown"))
		SetMonsterMoveState(EMonsterMoveState::EMMS_FORWARD);
	else if (EnemyController->GetBlackBoardComponent()->GetValueAsBool("IsAttackRange") && 
		EnemyController->GetBlackBoardComponent()->GetValueAsBool("bAttackCoolDown"))
		SetMonsterMoveState(EMonsterMoveState::EMMS_BACK);
	else if (!EnemyController->GetBlackBoardComponent()->GetValueAsBool("IsAttackRange") &&
		EnemyController->GetBlackBoardComponent()->GetValueAsBool("bAttackCoolDown"))
		if (!(GetMonsterMoveState() == EMonsterMoveState::EMMS_LEFT || GetMonsterMoveState() == EMonsterMoveState::EMMS_RIGHT))
			FMath::RandRange(0, 1) ? SetMonsterMoveState(EMonsterMoveState::EMMS_LEFT) : SetMonsterMoveState(EMonsterMoveState::EMMS_RIGHT);
	
}

void AEnemyForkyDoll::RightHandOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto Character = Cast<APlayerCharacter>(OtherActor);

	if (Character)
	{
		float damage = DoDamage(Character, BaseDamage, EnemyController);
		SpawnBlood(FName("FX_Socket"), BloodParticles, Character->GetMesh(),damage);
		DeactivateCollision(RightHandCollision);
	}
}

float AEnemyForkyDoll::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (DamageCauser)
	{
		auto Character = Cast<APlayerCharacter>(DamageCauser);

		if (Character)
		{
			auto GetKnockBack = Character->GetKnockBackStatus();
			switch (GetKnockBack)
			{
			case EKnockBackState::EKBS_KNOCKBACK:
				if(bAttackCancel == false)
				{
					AttackingCancelCheck();
					SetIsTrueHit();
					Knockback(Character, Character->GetCurrentSkillID());
					PS_Stun->SetFloatParameter(FName("StunFade"), 0);
					DeactivateCollision(RightHandCollision);
				}
				break;
			case EKnockBackState::EKBS_KNOCKDOWN:
				if (bAttackCancel == false)
				{
					AttackingCancelCheck();
					KnockDown(Character, Character->GetCurrentSkillID());
					PS_Stun->SetFloatParameter(FName("StunFade"), 0);
					DeactivateCollision(RightHandCollision);
				}
				break;
			case EKnockBackState::EKBS_STUN:
				if (bAttackCancel == false)
				{
					AttackingCancelCheck();
					StunStart();
					DeactivateCollision(RightHandCollision);
				}
				break;
			}
		}
	}
	OnDamageColorMat(DynamicZeroMat, FName("DamageColorSwitch"), MatTimer_0);
	OnDamageColorMat(DynamicOneMat, FName("DamageColorSwitch"), MatTimer_0);

	if (GetEnemyState() == EEnemyState::EES_DEAD) return DamageAmount;

	ShowHealthBar();

	return DamageAmount;
}


void AEnemyForkyDoll::AttackRangeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == nullptr) return;

	auto Character = Cast<APlayerCharacter>(OtherActor);

	if (Character)
	{
		if (EnemyController)
		{
			EnemyController->GetBlackBoardComponent()->SetValueAsBool(FName("IsAttackRange"), true);
		}
	}
}

void AEnemyForkyDoll::AttackRangeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == nullptr) return;

	auto Character = Cast<APlayerCharacter>(OtherActor);

	if (Character)
	{
		if (EnemyController)
		{
			EnemyController->GetBlackBoardComponent()->SetValueAsBool(FName("IsAttackRange"), false);
		}
	}
}

void AEnemyForkyDoll::Die()
{
	GetWorldTimerManager().ClearTimer(RushTimer);
	GetWorldTimerManager().ClearTimer(RushEndTimer);

	AttackRangeSphere->OnComponentBeginOverlap.RemoveAll(this);
	RightHandCollision->OnComponentBeginOverlap.RemoveAll(this);
	AttackRangeSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightHandCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PS_Stun->SetFloatParameter(FName("StunFade"), 0);

	Super::Die();

	HideHealthBar();

	AttackingCancelCheck();
}

void AEnemyForkyDoll::Attack()
{
	Super::Attack();
	bAttackCancel = false;
	GetWorldTimerManager().SetTimer(RushTimer, this, &AEnemyForkyDoll::AttackLaunch,RushTime);
}

void AEnemyForkyDoll::AttackLaunch()
{
	auto GetForward = this->GetActorForwardVector();
	LaunchCharacter(GetForward * AttackRushPower,false,false);

	bAttackCancel = true;

	GetWorldTimerManager().SetTimer(RushEndTimer, this, &AEnemyForkyDoll::AttackRushEnd, RushEndTime);
}

void AEnemyForkyDoll::AttackRushEnd()
{	
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	bAttackCancel = false;

	if (AnimInstance)
	{
		AnimInstance->Montage_Play(AttackMontage, 1.0f);
		AnimInstance->Montage_JumpToSection(FName("Attack_Last"), AttackMontage);
		DeactivateCollision(RightHandCollision);
	}
}

void AEnemyForkyDoll::AttackingCancelCheck()
{
	if (EnemyController)
	{
		GetWorldTimerManager().ClearTimer(RushTimer);
		GetWorldTimerManager().ClearTimer(RushEndTimer);
		uint8 myState = EnemyController->GetBlackBoardComponent()->GetValueAsEnum(FName("EnemyState"));
		
		if (myState == uint8(EEnemyState::EES_ATTACK))
		{
			UAnimInstance* AnimInstacne = GetMesh()->GetAnimInstance();
			if (AnimInstacne)
			{
				AnimInstacne->Montage_Stop(0.f);
			}
			SetEnemyState(EEnemyState::EES_MOVE);
		}
	}
}

void AEnemyForkyDoll::StunStart()
{
	Super::StunStart();
	PS_Stun->SetFloatParameter(FName("StunFade"), 1);
	DeactivateCollision(RightHandCollision);
	PlayHighPriorityMontage(StunMontage, FName("StunStart"), 1.0f);
}

void AEnemyForkyDoll::StunEnd()
{
	Super::StunEnd();
	PS_Stun->SetFloatParameter(FName("StunFade"), 0);
}

void AEnemyForkyDoll::EndTimer()
{
	Super::EndTimer();

	if (Player)
	{
		auto MyPlayerState = Player->GetCurrentState();
		if (MyPlayerState == EPlayerState::EPS_DEAD)
		{
			if (EnemyController)
			{
				GetWorld()->GetTimerManager().ClearTimer(RushTimer);
				GetWorld()->GetTimerManager().ClearTimer(RushEndTimer);
			}
		}
	}
}

void AEnemyForkyDoll::KnockDown(APlayerCharacter* Target, int32 index)
{
	Super::KnockDown(Target, index);

	DeactivateCollision(RightHandCollision);
	PlayHighPriorityMontage(KnockDownMontage, FName("KnockDown"), KnockDownMontagePlayRate);
}

void AEnemyForkyDoll::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (RushTimer.IsValid())
		GetWorldTimerManager().ClearTimer(RushTimer);

	if (RushEndTimer.IsValid())
		GetWorldTimerManager().ClearTimer(RushEndTimer);

	if (MatTimer_0.IsValid())
		GetWorldTimerManager().ClearTimer(MatTimer_0);

	Super::EndPlay(EndPlayReason);
}

void AEnemyForkyDoll::Accept(VisitorPtr visitor)
{
	visitor->Visit(this);
}
