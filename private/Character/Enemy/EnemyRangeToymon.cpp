// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Enemy/EnemyRangeToymon.h"
#include "../../../Public/Character/Enemy/Enemy.h"
#include "../../../Public/Character/Enemy/EnemyController.h"
#include "../../../Public/Character/PlayerCharacter/PlayerCharacter.h"
#include "Components/BoxComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Components/WidgetComponent.h"
#include "Character/Enemy/EnemyRangeToymonBlock.h"
#include "Kismet/KismetMathLibrary.h"
#include "Particles/ParticleSystemComponent.h"


AEnemyRangeToymon::AEnemyRangeToymon():
	bAttacking(false),
	KnockDownMontagePlayRate(1.0f),
	ThrowSpeed(1500.f)
{
	PrimaryActorTick.bCanEverTick = true;

	AttackRangeSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AttackRangeCollision"));
	AttackRangeSphere->SetupAttachment(GetRootComponent());

	HealthBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidget"));
	HealthBarWidget->SetupAttachment(GetRootComponent());

	PS_Stun = CreateDefaultSubobject< UParticleSystemComponent>(TEXT("PS_Stun"));
	PS_Stun->SetupAttachment(GetRootComponent());

	Visitors.insert(std::make_pair("Die", std::make_shared<DieVisitor>()));
	Visitors.insert(std::make_pair("Attack", std::make_shared<AttackVisitor>()));


}

void AEnemyRangeToymon::BeginPlay()
{
	Super::BeginPlay();



	PS_Stun->SetFloatParameter(FName("StunFade"), 0);

	SetMonsterMoveState(EMonsterMoveState::EMMS_FORWARD);

	AttackRangeSphere->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::AttackRangeBeginOverlap);
	AttackRangeSphere->OnComponentEndOverlap.AddUniqueDynamic(this, &ThisClass::AttackRangeEndOverlap);

	if (GetDistanceTo(Player) <= AttackRangeSphere->GetScaledSphereRadius())
		EnemyController->GetBlackBoardComponent()->SetValueAsBool(FName("IsAttackRange"), true);
}

float AEnemyRangeToymon::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
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
				SetIsTrueHit();
				Knockback(Character, Character->GetCurrentSkillID());
				PS_Stun->SetFloatParameter(FName("StunFade"), 0);
				break;
			case EKnockBackState::EKBS_KNOCKDOWN:
				KnockDown(Character, Character->GetCurrentSkillID());
				PS_Stun->SetFloatParameter(FName("StunFade"), 0);
				break;
			case EKnockBackState::EKBS_STUN:
				StunStart();
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

void AEnemyRangeToymon::AttackRangeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == nullptr) return;

	auto Character = Cast<APlayerCharacter>(OtherActor);

	if (Character)
	{
		if (EnemyController)
		{
			EnemyController->GetBlackBoardComponent()->SetValueAsBool(TEXT("IsAttackRange"), true);

		}
	}
}

void AEnemyRangeToymon::AttackRangeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == nullptr) return;

	auto Character = Cast<APlayerCharacter>(OtherActor);

	if (Character)
	{
		if (EnemyController)
		{
			EnemyController->GetBlackBoardComponent()->SetValueAsBool(TEXT("IsAttackRange"), false);
		}
	}
}

void AEnemyRangeToymon::OnConstruction(const FTransform& Transform)
{
	SetMonsterType(EMonsterType::EMT_TOYMON);
	Super::OnConstruction(Transform);

	DynamicZeroMat = SetDynamicMat(ZeroMatInstance, DynamicZeroMat, 0, this);
	DynamicOneMat = SetDynamicMat(OneMatInstance, DynamicOneMat, 1, this);

	ArrBlockClass.Add(BlockClass_1);
	ArrBlockClass.Add(BlockClass_2);
	ArrBlockClass.Add(BlockClass_3);
	ArrBlockClass.Add(BlockClass_4);
}

void AEnemyRangeToymon::KnockDown(APlayerCharacter* Target, int32 index)
{
	Super::KnockDown(Target, index);
	bAttacking = false;

	PlayHighPriorityMontage(KnockDownMontage, FName("KnockDown"), KnockDownMontagePlayRate);
}

void AEnemyRangeToymon::StunStart()
{
	Super::StunStart();
	bAttacking = false;
	
	PS_Stun->SetFloatParameter(FName("StunFade"), 1);

	PlayHighPriorityMontage(StunMontage, FName("StunStart"), 1.0f);
}

void AEnemyRangeToymon::StunEnd()
{
	Super::StunEnd();
	PS_Stun->SetFloatParameter(FName("StunFade"), 0);
}

void AEnemyRangeToymon::BlockSpawn()
{
	int Number = FMath::RandRange(0, 3);
	
	if (ArrBlockClass.IsValidIndex(Number) &&ArrBlockClass[Number]!=nullptr)
	{
		if (GetWorld())
		{
			FRotator BlockSocketRot = GetMesh()->GetSocketRotation(FName("BlockSocket"));
			FVector BlockSocketLoc = GetMesh()->GetSocketLocation(FName("BlockSocket"));

			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;

			Block = GetWorld()->SpawnActor<AEnemyRangeToymonBlock>(ArrBlockClass[Number], BlockSocketLoc, BlockSocketRot, SpawnParams);

			if (Block)
			{
				Block->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale , FName("BlockSocket"));
			}
		}
	}
}

void AEnemyRangeToymon::DeattachBlock()
{
	if (Block)
	{
		bAttacking = false;
		Block->StartFire(Block, ThrowSpeed,0.8f);
	}
}

void AEnemyRangeToymon::DeattachHitBlock()
{
	if (Block->IsValidLowLevel())
		Block->HitDetach();
}

void AEnemyRangeToymon::IgnoreHitBlock()
{
	if (Block->IsValidLowLevel())
	{
		Block->GetBlockMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
		Block->GetBlockMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel5, ECollisionResponse::ECR_Ignore);

	}
}

void AEnemyRangeToymon::Attack()
{
	Super::Attack();
	bAttacking = true;
}

void AEnemyRangeToymon::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (MatTimer_0.IsValid())
		GetWorldTimerManager().ClearTimer(MatTimer_0);

	if (Block->IsValidLowLevel())
		Block->Destroy();

	Super::EndPlay(EndPlayReason);
}

void AEnemyRangeToymon::Die()
{
	bAttacking = false;

	AttackRangeSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AttackRangeSphere->OnComponentBeginOverlap.RemoveAll(this);

	PS_Stun->SetFloatParameter(FName("StunFade"), 0);
	Super::Die();

	HideHealthBar();

}


void AEnemyRangeToymon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetEnemyState() == EEnemyState::EES_DEAD)
	{
		float DissolveTime = DissolveTimeRate * DeltaTime;

		DissolveChange(DynamicZeroMat, FName("Dissolve_Con"), DissolveTime);
		DissolveChange(DynamicOneMat, FName("Dissolve_Con"), DissolveTime);
	}

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


	if (bAttacking)
	{
		FRotator LookAtYaw = GetLookAtRotationYaw(GetPlayerLocation());
		auto InterpRot = FMath::RInterpTo(GetActorRotation(), LookAtYaw, DeltaTime, 5.0f);
		SetActorRotation(InterpRot);
	}
}

void AEnemyRangeToymon::Accept(VisitorPtr visitor)
{
	visitor->Visit(this);
}


