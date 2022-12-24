// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Boss/OnePhaseKingCrown.h"
#include "..\..\..\Public\Character\Boss\OnePhaseKingCrown.h"
#include"Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "../../../Public/Character/Enemy/EnemyController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "../../../Public/Character/PlayerCharacter/PlayerCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Character/Boss/KingCrownCrane.h"
#include "Particles/ParticleSystemComponent.h"
#include "Character/Boss/KingCrownPunch.h"
#include "Character/PlayerCharacter/PlayerCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Character/Boss/KingCrown.h"

AOnePhaseKingCrown::AOnePhaseKingCrown() :
	KingCrownPhase(EKingCrownPhase::EKCP_ONEPHASE),
	KingCrownPattern(EKingCrownPattern::EKCP_CRANE),
	KingCrownState(EKingCrownState::EKCS_NOGROGGY),
	KingCrownShiledRatio(0.8f),
	ShockWaveDamage(100.f),
	PatternWindPower(10000.f),
	bGiftFire(false),
	GroggyWindTime(1.f),
	CranePatternCount(0),
	MaxFireTime(0.84f),
	FireTime(0.f),
	MinFireCount(3),
	MaxFireCount(9),
	CalcFireCount(0),
	ActiveBehaviorTime(3.f)
{
	PrimaryActorTick.bCanEverTick = true;

	ShockWaveRange = CreateDefaultSubobject<USphereComponent>(TEXT("ShockWaveRange"));
	ShockWaveRange->SetupAttachment(GetMesh(), FName("Root"));

	WindCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("WindCollision"));
	WindCollision->SetupAttachment(GetMesh(), FName("MainCollision"));

	Visitors.insert(std::make_pair("FireGiftBox", std::make_shared<KingCrownFireVisitor>()));
	Visitors.insert(std::make_pair("CraneAttack", std::make_shared<KingCrownCraneVisitor>()));
	Visitors.insert(std::make_pair("Punch", std::make_shared<KingCrownPunchVisitor>()));
	Visitors.insert(std::make_pair("Die", std::make_shared<DieVisitor>()));
	Visitors.insert(std::make_pair("CraneLoopPatternEnd", std::make_shared<KingCrownCraneLoopVisitor>()));

}

void AOnePhaseKingCrown::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void AOnePhaseKingCrown::BeginPlay()
{
	Super::BeginPlay();

	TargetWidgetSetHidden();

	SetPullType(EPullType::EPT_DISALLOW);
	MonsterGuardPoint = EMonsterGuardPoint::EMGP_ON;

	if (EnemyController)
	{
		SetKingCrownState(EKingCrownState::ECKS_MAX);
		SetStateEnum(EKingCrownState::ECKS_MAX);
		EnemyController->GetBlackBoardComponent()->SetValueAsObject(FName("Target"), Player);
	}

	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	ShockWaveRange->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ShockWaveRange->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	ShockWaveRange->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ShockWaveRange->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	ShockWaveRange->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel5, ECollisionResponse::ECR_Overlap);

	ShockWaveRange->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::ShockWaveBeginOverlap);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel5, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);

	WindCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	WindCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	WindCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel5, ECollisionResponse::ECR_Ignore);
	WindCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	WindCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);
	WindCollision->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel3);

	WindCollision->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::WindCollisionBeginOveralp);

	CurShiledRatio = KingCrownShiledRatio;
}

void AOnePhaseKingCrown::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetKingCrownState() == EKingCrownState::EKCS_DIE)
		return;

	if (EnemyController)
	{
		auto EnemyPatternState = EnemyController->GetBlackBoardComponent()->GetValueAsEnum("EKingCrownPattern");

		switch (EnemyPatternState)
		{
		case uint8(EKingCrownPattern::EKCP_IDLE):
			SetLEDMat(1.f, 1.f, 0.f, 0.f, 0.f);
			GetKingCrownMainCollision()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
			GetKingCrownMainCollision()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel5, ECollisionResponse::ECR_Block);
			WindCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
			WindCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel5, ECollisionResponse::ECR_Ignore);

			KingCrownSpawnLevel->GetCurCount();

			EnemyController->GetBlackBoardComponent()->SetValueAsInt(FName("SpawnCount"), KingCrownSpawnLevel->GetCurCount());
			{
		
				FRotator LookAtYaw = GetLookAtRotationYaw(GetPlayerLocation());
				auto InterpRot = FMath::RInterpTo(GetActorRotation(), LookAtYaw, DeltaTime, 3.0f);
				SetActorRotation(InterpRot);
				
			}
			break;
		case uint8(EKingCrownPattern::EKCP_CRANE):
		case uint8(EKingCrownPattern::EKCP_LOOPCRANE):
		{
			FRotator LookAtYaw = GetLookAtRotationYaw(GetPlayerLocation());
			auto InterpRot = FMath::RInterpTo(GetActorRotation(), LookAtYaw, DeltaTime, 3.0f);
			SetActorRotation(InterpRot);
		}
		break;
		}
	}

	if (bGiftFire)
	{

		
		auto GiftFireLamb = this;

		FTimerDelegate GiftFireCallback;
		GiftFireCallback.BindLambda([GiftFireLamb]
			{
				if (IsValid(GiftFireLamb))
				{
					GiftFireLamb->bGiftFire = true;

					FRotator FireSocketRot = GiftFireLamb->GetMesh()->GetSocketRotation(FName("GiftFireSocket"));
					FVector FireSocketLoc = GiftFireLamb->GetMesh()->GetSocketLocation(FName("GiftFireSocket"));
					FTransform GiftFireTransform = GiftFireLamb->GetMesh()->GetSocketTransform(FName("GiftFireSocket"));

					auto Gift = GiftFireLamb->GetWorld()->SpawnActor<AKingCrownGiftBox>(GiftFireLamb->GiftBox, FireSocketLoc, FireSocketRot);

					if (GiftFireLamb->GetPS_GiftFireParticles())
					{
						UGameplayStatics::SpawnEmitterAtLocation(GiftFireLamb->GetWorld(), GiftFireLamb->GetPS_GiftFireParticles(), GiftFireTransform);
					}
					if (GiftFireLamb->FMOD_GiftBox_Spawn)
					{
						GiftFireLamb->PlayFModEventSound(GiftFireLamb->FMOD_GiftBox_Spawn);
					}

				}

			});

		GetWorldTimerManager().SetTimer(FireTimer, GiftFireCallback, FireTime, false);
		bGiftFire = false;

	}

	float PS_GroggyParticle = DeltaTime * 0.5;
	if (GetKingCrownState() == EKingCrownState::EKCS_GROGGY ||
		GetKingCrownPattern() == EKingCrownPattern::EKCP_IDLE ||
		GetKingCrownPattern() == EKingCrownPattern::EKCP_CRANE)
	{
		if (GroggyWindTime <= 0)
		{
			GroggyWindTime = 0.f;
		}

		GetPS_Wind()->SetFloatParameter(FName("WindFade"), GroggyWindTime -= PS_GroggyParticle);
	}
	else
	{
		if (GroggyWindTime >= 1)
		{
			GroggyWindTime = 1.f;
		}
		GetPS_Wind()->SetFloatParameter(FName("WindFade"), GroggyWindTime += PS_GroggyParticle);
	}

}

float AOnePhaseKingCrown::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (KingCrownPattern == EKingCrownPattern::EKCP_PUNCH ||
		KingCrownPattern ==EKingCrownPattern::EKCP_SHOCKWAVE ||
		KingCrownPattern == EKingCrownPattern::EKCP_GIFT)
		return 0.0f;

	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	auto Character = Cast<APlayerCharacter>(DamageCauser);

	if (GetHp() <= 0.f)
	{
		Player->UpdateKingCrownHUD(false);
	}

	if (GetMonsterGuardPoint() == EMonsterGuardPoint::EMGP_ON)
	{
		Hp -= DamageAmount * CurShiledRatio;
	}
	
	StartHit();
	
	if (GetGP() <= 0.f)
	{
		GroggyPatternStart_120();

	}

	return DamageAmount;
}

void AOnePhaseKingCrown::WindCollisionBeginOveralp(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto EnemyPatternState = EnemyController->GetBlackBoardComponent()->GetValueAsEnum("EKingCrownPattern");
	auto EnemyGroggyState = EnemyController->GetBlackBoardComponent()->GetValueAsEnum("EKingCrownState");

	if (EnemyPatternState == uint8(EKingCrownPattern::EKCP_IDLE) || EnemyPatternState == uint8(EKingCrownPattern::EKCP_CRANE)
		|| EnemyGroggyState == uint8(EKingCrownState::EKCS_GROGGY))
		return;

	auto Character = Cast<APlayerCharacter>(OtherActor);

	if (Character)
	{
		Character->LaunchCharacter((Character->GetActorForwardVector() * -1) * PatternWindPower, false, false);
		Character->KnockDown(Character->GetActorForwardVector() * -1);
	}
}



void AOnePhaseKingCrown::ShockWaveBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto Character = Cast <APlayerCharacter>(OtherActor);

	if (Character)
	{
		DoDamage(Character, ShockWaveDamage, EnemyController);
		DeactivateCollision(ShockWaveRange);
	}
}

void AOnePhaseKingCrown::CranePatternStart()
{
	if (KingCrownCrane)
	{
		KingCrownCrane->CraneAttackReady(this);

		SetLEDMat(1.f, 1.f, 1.f, 0.f, 0.f);
		SetFacialMat(1);
	}
}

void AOnePhaseKingCrown::CraneLoopPatternStart(float AnimPlayRate, int PatternEffectNumber)
{
	if (KingCrownCrane)
	{
		KingCrownCrane->CraneLoopingAttackStart(this,AnimPlayRate, PatternEffectNumber);
		SetLEDMat(1.f, 1.f, 1.f, 0.f, 0.f);
		SetFacialMat(1);
		CranePatternCount++;
		EnemyController->GetBlackBoardComponent()->SetValueAsInt(FName("CranePatternCount"), GetCranePatternCount());
	}
}

void AOnePhaseKingCrown::SetCraneLoopPattern(int Count, int PatternKey,bool bMoveTarget)
{
	KingCrownCrane->SetPlayerMoveTarget(bMoveTarget);
	SetCranePatternCount(0);
	EnemyController->GetBlackBoardComponent()->SetValueAsInt(FName("CranePatternCount"), 0);
	EnemyController->GetBlackBoardComponent()->SetValueAsInt(FName("SetLoopPatternKey"), PatternKey);
	EnemyController->GetBlackBoardComponent()->SetValueAsInt(FName("SetCranePatternCount"), Count);
	SetPatternEnum(EKingCrownPattern::EKCP_LOOPCRANE);
}

void AOnePhaseKingCrown::CraneLoopPatternEnd()
{
	if (KingCrownCrane)
	{
		KingCrownCrane->CraneLoopingAttackEnd();
		SetCranePatternCount(0);
		EnemyController->GetBlackBoardComponent()->SetValueAsInt(FName("CranePatternCount"), 0);
		EnemyController->GetBlackBoardComponent()->SetValueAsInt(FName("SetCranePatternCount"), 0);

		SetLEDMat(1.f, 1.f, 0.f, 0.f, 0.f);
		SetFacialMat(0);
	}
}

void AOnePhaseKingCrown::GiftPatternStart()
{
	Super::GiftPatternStart();

	if (GetWorld() && GiftBox != nullptr)
	{
		if (EnemyController)
		{
			PlayerCameraCollisionOn();
			SetPatternEnum(EKingCrownPattern::EKCP_GIFT);
			GetKingCrownMainCollision()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
			GetKingCrownMainCollision()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel5, ECollisionResponse::ECR_Block);
			GetKingCrownMainCollision()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);

			WindCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
			WindCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel5, ECollisionResponse::ECR_Overlap);

			CalcFireCount = FMath::FRandRange(MinFireCount, MaxFireCount);

			FireTime = (MaxFireTime / CalcFireCount) / 1.3;
		}
	}
}

void AOnePhaseKingCrown::PunchPatternEnd()
{
	if (KingCrownState == EKingCrownState::EKCS_GROGGY)
			return;

	SetFacialMat(0);


	Super::PunchPatternEnd();
}


void AOnePhaseKingCrown::GiftFireFunc()
{
	bGiftFire = true;
}

void AOnePhaseKingCrown::GiftFireEndFunc()
{
	bGiftFire = false;
	GetWorldTimerManager().ClearTimer(FireTimer);
}

void AOnePhaseKingCrown::GiftPatternEnd()
{
	if (EnemyController)
	{
		GetKingCrownMainCollision()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);

		WindCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
		WindCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel5, ECollisionResponse::ECR_Ignore);
		SetPatternEnum(EKingCrownPattern::EKCP_IDLE);
		SetMonsterGuardable(EMonsterGuardable::EMG_ABLE);
		SetFacialMat(0);
	}
}

void AOnePhaseKingCrown::PunchPatternStart()
{
	Super::PunchPatternStart();
	if (EnemyController)
	{
		PlayerCameraCollisionOn();
		SetPatternEnum(EKingCrownPattern::EKCP_PUNCH);
		GetKingCrownMainCollision()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
		GetKingCrownMainCollision()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel5, ECollisionResponse::ECR_Block);
		GetKingCrownMainCollision()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);

		WindCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		WindCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel5, ECollisionResponse::ECR_Overlap);
	}
}

void AOnePhaseKingCrown::PunchSpawn()
{
	/*if (EnemyController)
		SetPatternEnum(EKingCrownPattern::EKCP_PUNCH);*/

	Super::PunchSpawn();
}

void AOnePhaseKingCrown::PunchPatternEndNotify()
{
	if (EnemyController)
	{
		GetKingCrownMainCollision()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);
		WindCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
		WindCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel5, ECollisionResponse::ECR_Ignore);
		SetPatternEnum(EKingCrownPattern::EKCP_IDLE);
		SetMonsterGuardable(EMonsterGuardable::EMG_ABLE);
	}
}

void AOnePhaseKingCrown::StartHit()
{
	if (EnemyController && KingCrownState == EKingCrownState::EKCS_NOGROGGY)
	{
		PlayHighPriorityMontage(GetKingCrownHitMontage(), FName("HitReact"), 1.0f);
	}
}



void AOnePhaseKingCrown::GroggyPatternStart_120()
{
	Super::GroggyPatternStart_120();

	if (EnemyController)
	{
		PlayHighPriorityMontage(GetGroggyMontage_120(), FName("Start"), 1.0f);
		GetKingCrownMainCollision()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
		GetKingCrownMainCollision()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);
		GetKingCrownMainCollision()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
		GetKingCrownMainCollision()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel5, ECollisionResponse::ECR_Block);
		WindCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
		WindCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel5, ECollisionResponse::ECR_Ignore);

		SetStateEnum(EKingCrownState::EKCS_GROGGY);
		ClearFoucsSetting();
		SetPatternEnum(EKingCrownPattern::EKCP_IDLE);
	}
}

void AOnePhaseKingCrown::EndShockWave()
{
	if (EnemyController)
	{
		GetKingCrownMainCollision()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);
		WindCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
		WindCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel5, ECollisionResponse::ECR_Ignore);
		SetFacialMat(0);

		SetStateEnum(EKingCrownState::EKCS_NOGROGGY);
		SetPatternEnum(EKingCrownPattern::EKCP_IDLE);
		EnemyController->SetFocus(GetWorld()->GetFirstPlayerController()->GetPawn());

		SetGP(GetMaxGP());
		SetMonsterGuardPoint(EMonsterGuardPoint::EMGP_ON);
		SetMonsterGuardable(EMonsterGuardable::EMG_ABLE);
	}
}

void AOnePhaseKingCrown::UnNotifyCrash()
{
	Super::UnNotifyCrash();
	SetFacialMat(1);

	GetKingCrownMainCollision()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);
}

void AOnePhaseKingCrown::CollisionOverlapChange()
{
	SetStateEnum(EKingCrownState::EKCS_NOGROGGY);
	SetPatternEnum(EKingCrownPattern::EKCP_SHOCKWAVE);
	GetKingCrownMainCollision()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	GetKingCrownMainCollision()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel5, ECollisionResponse::ECR_Overlap);
	WindCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	WindCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel5, ECollisionResponse::ECR_Overlap);
}

void AOnePhaseKingCrown::Die()
{
	GetKingCrownMainCollision()->OnComponentBeginOverlap.RemoveAll(this);
	ShockWaveRange->OnComponentBeginOverlap.RemoveAll(this);
	ShockWaveRange->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetKingCrownMainCollision()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WindCollision ->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	
	Super::Die();
}

void AOnePhaseKingCrown::EndPlay(const EEndPlayReason::Type EndPlayReason)
{

	if (BehaviorTimerHandle.IsValid())
		GetWorldTimerManager().ClearTimer(BehaviorTimerHandle);

	if (FireTimer.IsValid())
		GetWorldTimerManager().ClearTimer(FireTimer);

	Super::EndPlay(EndPlayReason);
}

void AOnePhaseKingCrown::DeathStart()
{
	Super::DeathStart();

	SetStateEnum(EKingCrownState::EKCS_DIE);
	SetPatternEnum(EKingCrownPattern::EKCP_MAX);
}

void AOnePhaseKingCrown::PlayerCameraCollisionOn()
{
	GetKingCrownMainCollision()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Block);
}

void AOnePhaseKingCrown::PlayerCameraCollisionOFF()
{
	GetKingCrownMainCollision()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
}

//Active -> Trigger
void AOnePhaseKingCrown::ActivateOnePhaseKingCrownBehaviorTree()
{
	if (Player)
	{
		Player->SetCombatTarget(this);
		Player->UpdateKingCrownHUD(true);
		GetKingCrownMainCollision()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);
		GetKingCrownMainCollision()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		GetKingCrownMainCollision()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel5, ECollisionResponse::ECR_Overlap);
		WindCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
		WindCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel5, ECollisionResponse::ECR_Ignore);
		SetPatternEnum(EKingCrownPattern::EKCP_SHOCKWAVE);
		PlayHighPriorityMontage(GetStartShockWaveMontage(), "Start", 1.0f);
	}
	SetKingCrownState(EKingCrownState::EKCS_NOGROGGY);
	SetStateEnum(EKingCrownState::EKCS_NOGROGGY);


	auto VelocityLamb = this;

	FTimerDelegate VelocityCallback;
	VelocityCallback.BindLambda([VelocityLamb]() {
		if (IsValid(VelocityLamb))
		{
			for (TObjectIterator<AKingCrownCrane> Itr; Itr; ++Itr)
			{
				if (Itr->IsA(AKingCrownCrane::StaticClass()))
				{
					VelocityLamb->KingCrownCrane = *Itr;
				}
			}
		}
		});
	GetWorldTimerManager().SetTimer(BehaviorTimerHandle, VelocityCallback, ActiveBehaviorTime, false);
}



//SetEnum &BehEnum
void AOnePhaseKingCrown::SetPatternEnum(EKingCrownPattern Pattern)
{
	if (EnemyController)
	{
		SetKingCrownPattern(Pattern);
		EnemyController->GetBlackBoardComponent()->SetValueAsEnum(FName("EKingCrownPattern"), uint8(Pattern));
	}
}

void AOnePhaseKingCrown::SetStateEnum(EKingCrownState State)
{
	if (EnemyController)
	{
		SetKingCrownState(State);
		EnemyController->GetBlackBoardComponent()->SetValueAsEnum(FName("EKingCrownState"), uint8(State));
	}
}

void AOnePhaseKingCrown::KingCrownClear()
{
	KingCrownCrane->Destroy();

	auto temp = KingCrownSpawnLevel->Actors;
	for (auto it : temp)
		it->Destroy();
}

void AOnePhaseKingCrown::Accept(VisitorPtr Visitor)
{
	Visitor->Visit(this);
}
