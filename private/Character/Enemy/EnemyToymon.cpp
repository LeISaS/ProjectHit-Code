// Fill out your copyright notice in the Description page of Project Settings.

#include "../../../Public/Character/Enemy/EnemyToymon.h"
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


AEnemyToymon::AEnemyToymon() :
	RightHandBloodSocket(FName("FX_BloodParticle")),
	KnockDownMontagePlayRate(1.0f)
{
	PrimaryActorTick.bCanEverTick = true;

	AttackRangeSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AttackRangeCollision"));
	AttackRangeSphere->SetupAttachment(GetRootComponent());

	RightHandCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("RightHandCollision"));
	RightHandCollision->SetupAttachment(GetMesh(), FName("RightHandBone"));

	HealthBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidget"));
	HealthBarWidget->SetupAttachment(GetRootComponent());

	Visitors.insert(std::make_pair("Die",std::make_shared<DieVisitor>()));
	Visitors.insert(std::make_pair("Attack",std::make_shared<AttackVisitor>()));

	RightHandCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightHandCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	RightHandCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	RightHandCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

}

void AEnemyToymon::BeginPlay()
{
	Super::BeginPlay();

	SetMonsterMoveState(EMonsterMoveState::EMMS_FORWARD);

	AttackRangeSphere->OnComponentBeginOverlap.AddUniqueDynamic(this, &AEnemyToymon::AttackRangeBeginOverlap);
	AttackRangeSphere->OnComponentEndOverlap.AddUniqueDynamic(this, &AEnemyToymon::AttackRangeEndOverlap);

	RightHandCollision->OnComponentBeginOverlap.AddUniqueDynamic(this, &AEnemyToymon::RightHandOverlap);
	RightHandCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (GetDistanceTo(Player) <= AttackRangeSphere->GetScaledSphereRadius())
		EnemyController->GetBlackBoardComponent()->SetValueAsBool(FName("IsAttackRange"), true);
}

void AEnemyToymon::RightHandOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto Character = Cast <APlayerCharacter>(OtherActor);

	if (Character)
	{
		DoDamage(Character, BaseDamage, EnemyController);
		UGameplayStatics::SpawnEmitterAttached(BloodParticles, Character->GetMesh(), TEXT("None"),
			Character->GetActorLocation(), Character->GetActorRotation(), EAttachLocation::KeepWorldPosition);
		DeactivateCollision(RightHandCollision);
	}
}

float AEnemyToymon::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
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
				Knockback(Character,Character->GetCurrentSkillID());
				DeactivateCollision(RightHandCollision);
				break;
			case EKnockBackState::EKBS_KNOCKDOWN:
				KnockDown(Character, Character->GetCurrentSkillID());
				DeactivateCollision(RightHandCollision);
				break;
			case EKnockBackState::EKBS_STUN:
				StunStart();
				DeactivateCollision(RightHandCollision);
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

void AEnemyToymon::AttackRangeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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

void AEnemyToymon::AttackRangeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
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

void AEnemyToymon::OnConstruction(const FTransform& Transform)
{
	SetMonsterType(EMonsterType::EMT_TOYMON);
	Super::OnConstruction(Transform);

	DynamicZeroMat = SetDynamicMat(ZeroMatInstance, DynamicZeroMat, 0, this);	
	DynamicOneMat = SetDynamicMat(OneMatInstance, DynamicOneMat, 1, this);	
}

void AEnemyToymon::KnockDown(APlayerCharacter* Target, int32 index)
{
	Super::KnockDown(Target, index);

	DeactivateCollision(RightHandCollision);
	PlayHighPriorityMontage(KnockDownMontage, FName("KnockDown"), KnockDownMontagePlayRate);
}

void AEnemyToymon::Die()
{

	AttackRangeSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightHandCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AttackRangeSphere->OnComponentBeginOverlap.RemoveAll(this);
	RightHandCollision->OnComponentBeginOverlap.RemoveAll(this);

	Super::Die();

	HideHealthBar();

}


void AEnemyToymon::Tick(float DeltaTime)
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
}

void AEnemyToymon::Accept(VisitorPtr visitor)
{
	visitor->Visit(this);
}


