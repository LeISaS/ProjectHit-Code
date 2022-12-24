// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Enemy/EnemyDoggy.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "../../../Public/Character/Enemy/Enemy.h"
#include "../../../Public/Character/Enemy/EnemyController.h"
#include "../../../Public/Character/PlayerCharacter/PlayerCharacter.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/WidgetComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"

AEnemyDoggy::AEnemyDoggy() :
	BloodSocket("FX_BloodParticle"),
	KnockDownMontagePlayRate(1.0f),
	RetreatCoolDownTime(5.0f),
	bRetreat(false)
{
	PrimaryActorTick.bCanEverTick = true;

	AttackCollision = CreateDefaultSubobject<USphereComponent>(TEXT("AttackCollision"));
	AttackCollision->SetupAttachment(GetMesh(), FName("AttackCollision"));

	AttackRangeSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AttackRangeSphere"));
	AttackRangeSphere->SetupAttachment(GetRootComponent());

	HealthBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidget"));
	HealthBarWidget->SetupAttachment(GetRootComponent());

	RetreatCollision = CreateDefaultSubobject<USphereComponent>(TEXT("RetreatCollision"));
	RetreatCollision->SetupAttachment(GetRootComponent());

	PS_Stun = CreateDefaultSubobject< UParticleSystemComponent>(TEXT("PS_Stun"));
	PS_Stun->SetupAttachment(GetRootComponent());

	Visitors.insert(std::make_pair("Die", std::make_shared<DieVisitor>()));
	Visitors.insert(std::make_pair("Attack", std::make_shared<AttackVisitor>()));
	Visitors.insert(std::make_pair("Retreat", std::make_shared<DoggyRetreatVisitor>()));

}

void AEnemyDoggy::BeginPlay()
{
	Super::BeginPlay();

	SetMonsterMoveState(EMonsterMoveState::EMMS_FORWARD_2);

	PS_Stun->SetFloatParameter(FName("StunFade"), 0);

	AttackCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AttackCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	AttackCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	AttackCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	AttackCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel5, ECollisionResponse::ECR_Overlap);


	AttackCollision->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::AttackCollisionOverlap);

	AttackRangeSphere->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::AttackRangeBeginOverlap);
	AttackRangeSphere->OnComponentEndOverlap.AddUniqueDynamic(this, &ThisClass::AttackRangeEndOverlap);	

	RetreatCollision->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::RetreatRangeBeginOverlap);
	RetreatCollision->OnComponentEndOverlap.AddUniqueDynamic(this, &ThisClass::RetreatRangeEndOverlap);
}

void AEnemyDoggy::Tick(float DeltaTime)
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
		float DissolveTime = DissolveTimeRate * DeltaTime * 1.5f;

		DissolveChange(DynamicZeroMat, FName("Dissolve_Con"), DissolveTime);
	}
}

float AEnemyDoggy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
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
				if (!bRetreat)
				{
					SetIsTrueHit();
					Knockback(Character, Character->GetCurrentSkillID());
					DeactivateCollision(AttackCollision);
					PS_Stun->SetFloatParameter(FName("StunFade"), 0);
				}
				break;
			case EKnockBackState::EKBS_KNOCKDOWN:
				bRetreat = false;
				KnockDown(Character, Character->GetCurrentSkillID());
				PS_Stun->SetFloatParameter(FName("StunFade"), 0);
				DeactivateCollision(AttackCollision);
				break;
			case EKnockBackState::EKBS_STUN:
				StunStart();
				DeactivateCollision(AttackCollision);
				break;
			}
		}
	}
	OnDamageColorMat(DynamicZeroMat, FName("DamageColorSwitch"), MatTimer_0);

	if (GetEnemyState() == EEnemyState::EES_DEAD) return DamageAmount;

	ShowHealthBar();

	return DamageAmount;
}

void AEnemyDoggy::AttackCollisionOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto Character = Cast <APlayerCharacter>(OtherActor);

	if (Character)
	{
		float damage = DoDamage(Character, BaseDamage, EnemyController);
		SpawnBlood(BloodSocket, BloodParticles, Character->GetMesh(),damage);
		DeactivateCollision(AttackCollision);
	}
}

void AEnemyDoggy::AttackRangeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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

void AEnemyDoggy::AttackRangeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
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

void AEnemyDoggy::RetreatRangeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (GetEnemyState() == EEnemyState::EES_DEAD)
		return;

	if (OtherActor == nullptr) return;

	auto Character = Cast<APlayerCharacter>(OtherActor);

	if (Character)
	{
		if (EnemyController)
		{
			EnemyController->GetBlackBoardComponent()->SetValueAsBool(TEXT("IsRetreatRange"), true);

		}
	}
}

void AEnemyDoggy::RetreatRangeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (GetEnemyState() == EEnemyState::EES_DEAD)
		return;

	if (OtherActor == nullptr) return;

	auto Character = Cast<APlayerCharacter>(OtherActor);

	if (Character)
	{
		if (EnemyController)
		{
			EnemyController->GetBlackBoardComponent()->SetValueAsBool(TEXT("IsRetreatRange"), false);

		}
	}
}

void AEnemyDoggy::OnConstruction(const FTransform& Transform)
{
	SetMonsterType(EMonsterType::EMT_DOGGY);
	Super::OnConstruction(Transform);

	DynamicZeroMat = SetDynamicMat(ZeroMatInstance, DynamicZeroMat, 0, this);

}

void AEnemyDoggy::KnockDown(APlayerCharacter* Target, int32 index)
{
	Super::KnockDown(Target,index);

	DeactivateCollision(AttackCollision);
	PlayHighPriorityMontage(KnockDownMontage, FName("KnockDown"), KnockDownMontagePlayRate);
}

void AEnemyDoggy::StunStart()
{
	Super::StunStart();
	PS_Stun->SetFloatParameter(FName("StunFade"), 1);
	PlayHighPriorityMontage(StunMontage, FName("StunStart"), 1.0f);
}

void AEnemyDoggy::StunEnd()
{
	Super::StunEnd();
	PS_Stun->SetFloatParameter(FName("StunFade"), 0);

}

void AEnemyDoggy::Die()
{
	AttackRangeSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AttackCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AttackRangeSphere->OnComponentBeginOverlap.RemoveAll(this);
	AttackCollision->OnComponentBeginOverlap.RemoveAll(this);

	PS_Stun->SetFloatParameter(FName("StunFade"), 0);

	Super::Die();

	HideHealthBar();
}

void AEnemyDoggy::Attack()
{
	Super::Attack();
	bRetreat = false;
}

void AEnemyDoggy::AttackEnd()
{
	Super::AttackEnd();
}

void AEnemyDoggy::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (MatTimer_0.IsValid())
		GetWorldTimerManager().ClearTimer(MatTimer_0);

	if (RetreatCoolDownTimer.IsValid())
		GetWorldTimerManager().ClearTimer(RetreatCoolDownTimer);

	Super::EndPlay(EndPlayReason);
}

void AEnemyDoggy::Accept(VisitorPtr Visitor)
{
	Visitor->Visit(this);
}

void AEnemyDoggy::RetreatStart()
{
	if (GetEnemyState() == EEnemyState::EES_DEAD)
		return;

	bRetreat = true;

	EnemyController->GetBlackBoardComponent()->SetValueAsBool("bRetreat", bRetreat);

	if (EnemyController->GetBlackBoardComponent()->GetValueAsBool("bRetreatCoolDown"))
		return;

	EnemyController->StopMovement();

	TArray<float> RotatorSetting = { -30.0f,0.f,30.f };

	int RandomVaue = FMath::RandRange(0, 2);

	FRotator DestRot = FRotator(0.f, RotatorSetting[RandomVaue], 0.f);
	AddActorLocalRotation(DestRot);

	PlayHighPriorityMontage(RetreateMontage, FName("Retreat"), 1.0f);

	EnemyController->GetBlackBoardComponent()->SetValueAsBool("bRetreatCoolDown", true);

	auto RetreatCoolLamb = this;

	FTimerDelegate RetreatCoolDown;

	RetreatCoolDown.BindLambda([RetreatCoolLamb]() {
		if (IsValid(RetreatCoolLamb))
		{
			if (RetreatCoolLamb->EnemyController)
			{
				RetreatCoolLamb->EnemyController->GetBlackBoardComponent()->SetValueAsBool("bRetreatCoolDown", false);
			}
		}
		});

	GetWorldTimerManager().SetTimer(RetreatCoolDownTimer, RetreatCoolDown, RetreatCoolDownTime, false);

}

void AEnemyDoggy::RetreatEnd()
{
	bRetreat = false;
	EnemyController->GetBlackBoardComponent()->SetValueAsBool("bRetreat", false);
}

