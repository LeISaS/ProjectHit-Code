// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Boss/KingCrownCrane.h"
#include "Components/BoxComponent.h"
#include "../../../Public/Character/Enemy/EnemyController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/CapsuleComponent.h"
#include "../../../Public/Character/PlayerCharacter/PlayerCharacter.h"
#include "Character/Boss/OnePhaseKingCrown.h"
#include "GameFramework/Controller.h"
#include "Particles/ParticleSystem.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/SkeletalMeshComponent.h"

AKingCrownCrane::AKingCrownCrane() :
	CraneStampState(ECraneStampState::ECSS_IDLE),
	AttackStartPlayRate(1.f),
	AttackEndPlayRate(1.f),
	CraneDamage(100.f),
	AttackStartTime(5.0f),
	AttackKeepTime(0.5f),
	AttackKeepEndTime(3.0f),
	AttackIdleTime(2.2f)
{
	PrimaryActorTick.bCanEverTick = true;

	CraneAttackCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CraneAttackCollision"));
	CraneAttackCollision->SetupAttachment(GetMesh(),FName("AttackSocket"));

	CraneRangeCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CraneRangeCollision"));
	CraneRangeCollision->SetupAttachment(GetMesh(), FName("AttackSocket"));

	SignParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("SignParticle"));
	SignParticle->SetupAttachment(GetRootComponent());

	SignParticle_1 = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("SignParticle_1"));
	SignParticle_1->SetupAttachment(GetRootComponent());

	SignParticle_3 = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("SignParticle_3"));
	SignParticle_3->SetupAttachment(GetRootComponent());

	PS_GroogyParticles = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("PS_GroogyParticles"));
	PS_GroogyParticles->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AKingCrownCrane::BeginPlay()
{
	Super::BeginPlay();

	SetPullType(EPullType::EPT_DISALLOW);
	SetMonsterMoveState(EMonsterMoveState::EMMS_FORWARD);

	PS_GroogyParticles->Deactivate();

	SignParticle->Deactivate();
	SignParticle_1->Deactivate();
	SignParticle_3->Deactivate();

	CraneAttackCollision->OnComponentBeginOverlap.AddUniqueDynamic(this, &AKingCrownCrane::CraneAttackOverlap);

	CraneAttackCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CraneAttackCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CraneAttackCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CraneAttackCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	CraneAttackCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel5, ECollisionResponse::ECR_Overlap);

	CraneRangeCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
	CraneRangeCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel5, ECollisionResponse::ECR_Block);
	
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel5, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);

	SetEnemyAutoTarget(EEnemyAutoTarget::EEAT_NOTARGET);
	SetMonsterGuardable(EMonsterGuardable::EMG_DISABLE);
}

void AKingCrownCrane::CraneAttackOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto Character = Cast <APlayerCharacter>(OtherActor);

	if (Character)
	{
		DoDamage(Character, CraneDamage, EnemyController);
		DeactivateCollision(CraneAttackCollision);
		DeactivateCollision(CraneRangeCollision);
		GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
		GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel5, ECollisionResponse::ECR_Ignore);
	}
}

void AKingCrownCrane::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	DynamicZeroMat = SetDynamicMat(ZeroMatInstance, DynamicZeroMat, 0, this);	
	DynamicOneMat = SetDynamicMat(OneMatInstance, DynamicOneMat, 1, this);	
}

void AKingCrownCrane::EndPlay(const EEndPlayReason::Type EndPlayReason)
{

	if (AttackStartTimer.IsValid())
		GetWorldTimerManager().ClearTimer(AttackStartTimer);
	if (AttackKeepTimer.IsValid())
		GetWorldTimerManager().ClearTimer(AttackKeepTimer);
	if (AttackKeepEndTimer.IsValid())
		GetWorldTimerManager().ClearTimer(AttackKeepEndTimer);
	if (AttackIdleTimer.IsValid())
		GetWorldTimerManager().ClearTimer(AttackIdleTimer);
	if (LoopingHandle.IsValid())
		GetWorldTimerManager().ClearTimer(LoopingHandle);

	Super::EndPlay(EndPlayReason);

}

void AKingCrownCrane::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/*if (Player->GetCurrentState() == EPlayerState::EPS_DEAD)
	{
		GetWorldTimerManager().ClearTimer(AttackStartTimer);
		GetWorldTimerManager().ClearTimer(AttackKeepTimer);
		GetWorldTimerManager().ClearTimer(AttackKeepEndTimer);
		GetWorldTimerManager().ClearTimer(AttackIdleTimer);
		GetWorldTimerManager().ClearTimer(LoopingHandle);
	}*/
}

void AKingCrownCrane::CraneAttackReady(AOnePhaseKingCrown* kingcrown)
{
	KingCrown = kingcrown;
	if (EnemyController)
	{
		SetCraneStampState(ECraneStampState::ECSS_ATTACKREADY);
		SignParticle_3->Activate();

		if (KingCrown)
		{
			KingCrown->SetPatternEnum(EKingCrownPattern::EKCP_CRANE);
		}

		PlayFModEventSound(FMOD_Sign);

		EnemyController->GetBlackBoardComponent()->SetValueAsEnum(FName("ECraneStampState"), 1);

		ActivateCollision(CraneAttackCollision);
		ActivateCollision(CraneRangeCollision);

		GetWorldTimerManager().SetTimer(AttackStartTimer, this, &AKingCrownCrane::CraneAttackStart, AttackStartTime);
	}
}

void AKingCrownCrane::CraneAttackStart()
{
	if (EnemyController)
	{
		SetCraneStampState(ECraneStampState::ECSS_ATTACK);
		EnemyController->GetBlackBoardComponent()->SetValueAsEnum(FName("ECraneStampState"), 2);
		GetWorldTimerManager().SetTimer(AttackKeepTimer, this, &AKingCrownCrane::CraneAttacking, AttackKeepTime);
		GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
		GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel5, ECollisionResponse::ECR_Block);
	}
}

void AKingCrownCrane::CraneAttacking()
{
	if(EnemyController)
	{;
		EnemyController->StopMovement();
		SetCraneStampState(ECraneStampState::ECSS_ATTACKKEEP);
		GetCapsuleComponent()->SetCollisionObjectType(ECollisionChannel::ECC_Pawn);
		DeactivateCollision(CraneAttackCollision);

		GetWorld()->GetFirstPlayerController()->PlayerCameraManager->StartMatineeCameraShake(AttackCameraShake);

		GetWorldTimerManager().SetTimer(AttackKeepEndTimer, this, &AKingCrownCrane::CraneAttackEnd, AttackKeepEndTime);
		GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
		GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel5, ECollisionResponse::ECR_Ignore);
	}
}

void AKingCrownCrane::CraneAttackEnd()
{
	if (EnemyController)
	{
		SetCraneStampState(ECraneStampState::ECSS_ATTACKKEEPEND);
		GetCapsuleComponent()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);

		PS_GroogyParticles->Deactivate();

		auto KingCrownCrane = this;

		FTimerDelegate StateChange;
		StateChange.BindLambda([KingCrownCrane]() {

			if (IsValid(KingCrownCrane))
			{
				KingCrownCrane->SetCraneStampState(ECraneStampState::ECSS_IDLE);
				KingCrownCrane->EnemyController->GetBlackBoardComponent()->SetValueAsEnum(FName("ECraneStampState"), 0);
				KingCrownCrane->KingCrown->SetPatternEnum(EKingCrownPattern::EKCP_IDLE);
				KingCrownCrane->KingCrown->SetFacialMat(0);
			}
			});

		GetWorldTimerManager().SetTimer(AttackIdleTimer, StateChange, AttackIdleTime, false);
	}
}



void AKingCrownCrane::SetPlayerMoveTarget(bool bMoveTarget)
{
	if(bMoveTarget)
		EnemyController->GetBlackBoardComponent()->SetValueAsEnum(FName("ECraneStampState"), 2);
	else
		EnemyController->GetBlackBoardComponent()->SetValueAsEnum(FName("ECraneStampState"), 0);
}

void AKingCrownCrane::CraneLoopingAttackStart(AOnePhaseKingCrown* kingcrown, float AnimPlayRate, int PatternEffectNumber)
{
	KingCrown = kingcrown;

	if (KingCrown)
	{
		KingCrown->SetPatternEnum(EKingCrownPattern::EKCP_LOOPCRANE);
	}

	switch (PatternEffectNumber)
	{
	case 1:
		SignParticle_1->Activate();
		PatternEffectTime = 1.0f;
		break;
	case 2:
		SignParticle->Activate();
		PatternEffectTime = 2.f;
		break;
	case 3:
		SignParticle_3->Activate();
		PatternEffectTime = 3.f;
		break;

	}

	auto KingCrownCraneLoop = this;

	FTimerDelegate LoopingCallback;
	LoopingCallback.BindLambda([KingCrownCraneLoop, AnimPlayRate]
		{
			if (IsValid(KingCrownCraneLoop))
			{
				KingCrownCraneLoop->ActivateCollision(KingCrownCraneLoop->CraneAttackCollision);
				KingCrownCraneLoop->ActivateCollision(KingCrownCraneLoop->CraneRangeCollision);

				KingCrownCraneLoop->CraneAttackEndNotify();

				UAnimInstance* AnimInstance = KingCrownCraneLoop->GetMesh()->GetAnimInstance();

				if (AnimInstance)
				{
					AnimInstance->Montage_Play(KingCrownCraneLoop->LoppingAttackMontage, AnimPlayRate);
					AnimInstance->Montage_JumpToSection("Start", KingCrownCraneLoop->LoppingAttackMontage);
					KingCrownCraneLoop->SignParticle->Deactivate();
					KingCrownCraneLoop->SignParticle_1->Deactivate();
					KingCrownCraneLoop->SignParticle_3->Deactivate();
				}
			}
		});
	GetWorldTimerManager().SetTimer(LoopingHandle, LoopingCallback, PatternEffectTime, false);

	
}

void AKingCrownCrane::CraneLoopingAttackEnd()
{
	if (KingCrown)
	{
		KingCrown->SetPatternEnum(EKingCrownPattern::EKCP_IDLE);
	}
	CraneAttackStartNotify();

	DeactivateCollision(CraneAttackCollision);
	DeactivateCollision(CraneRangeCollision);

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance)
	{
		AnimInstance->Montage_Play(LoppingAttackMontage);
		AnimInstance->Montage_JumpToSection("End", LoppingAttackMontage);
		CraneAttackEndNotify();


	}
}

void AKingCrownCrane::CraneAttackEndNotify()
{
	EnemyController->GetBlackBoardComponent()->SetValueAsEnum(FName("ECraneStampState"), 0);

	SignParticle->DeactivateSystem();
	SignParticle_1->DeactivateSystem();
	SignParticle_3->DeactivateSystem();
}

void AKingCrownCrane::CraneAttackStartNotify()
{
	EnemyController->GetBlackBoardComponent()->SetValueAsEnum(FName("ECraneStampState"), 2);
}
