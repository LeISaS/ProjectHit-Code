// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Enemy/EnemyUder.h"
#include "Components/WidgetComponent.h"
#include "../../../Public/Character/Enemy/Enemy.h"
#include "../../../Public/Character/Enemy/EnemyController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../../../Public/Character/PlayerCharacter/PlayerCharacter.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Character/Enemy/Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "Components/PoseableMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/SkeletalMeshComponent.h"

float AEnemyUder::TotalHp = 0;
float AEnemyUder::LeftHp = 0;
float AEnemyUder::RightHp = 0;
bool AEnemyUder::bHealthCheck = 0;

AEnemyUder::AEnemyUder() :
	UderActivity(EUderActivity::EUA_TOGETHER),
	UderAttackState(EUderAttackState::EUAS_NOATTACKING),
	UderNumbering(EUderNumbering::EUN_NONE),
	AttackLaunchPower(1000.f),
	BoomTime(2.f),
	AttackDamage(100.f),
	RushDamage(25.f),
	ShieldCount(1),
	RushEnemyKnockBack(10000.f),
	UderBoomAttackDamage(30.f),
	ShiledOneRatio(0.8),
	ShiledTwoRatio(0.6),
	DieBoomScale(FVector(1.f, 1.f, 1.f)),
	RushCoolDownTime(10.f),
	UderDistance(1200.f),
	bInvi(false)
	{
	PrimaryActorTick.bCanEverTick = true;

	RightHandCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("RightHandCollision"));
	RightHandCollision->SetupAttachment(GetMesh(), FName("RightHandBone"));

	HealthBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidget"));
	HealthBarWidget->SetupAttachment(GetRootComponent());

	AttackRangeSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AttackRangeCollision"));
	AttackRangeSphere->SetupAttachment(GetRootComponent());

	BoomRangeSphere = CreateDefaultSubobject<USphereComponent>(TEXT("BoomRangeSphere"));
	BoomRangeSphere->SetupAttachment(GetMesh(), FName("Root"));

	RushCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("RushCollision"));
	RushCollision->SetupAttachment(GetMesh(), FName("RushCollision"));

	RushRangeSphere = CreateDefaultSubobject<USphereComponent>(TEXT("RushRangeSphere"));
	RushRangeSphere->SetupAttachment(GetRootComponent());

	RightArm = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("RightArm"));
	RightArm->SetupAttachment(GetMesh(),FName("ShieldRightSocket"));

	LeftArm = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("LeftArm"));
	LeftArm->SetupAttachment(GetMesh(), FName("ShieldLeftSocket"));

	PS_Shiled_Blue = CreateDefaultSubobject< UParticleSystemComponent>(TEXT("PS_Shiled_Blue"));
	PS_Shiled_Blue->SetupAttachment(GetRootComponent());

	PS_Shiled_Red = CreateDefaultSubobject< UParticleSystemComponent>(TEXT("PS_Shiled_Red"));
	PS_Shiled_Red->SetupAttachment(GetRootComponent());

	PS_Shiled_Invi = CreateDefaultSubobject< UParticleSystemComponent>(TEXT("PS_Shiled_Invi"));
	PS_Shiled_Invi->SetupAttachment(GetRootComponent());

	RightHandCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	RightHandCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	RightHandCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	RightHandCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel5, ECollisionResponse::ECR_Overlap);


	BoomRangeSphere->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	BoomRangeSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	BoomRangeSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	BoomRangeSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel5, ECollisionResponse::ECR_Overlap);

	RushCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	RushCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	RushCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	RushCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel5, ECollisionResponse::ECR_Overlap);
}

void AEnemyUder::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (EnemyState == EEnemyState::EES_DEAD)
		return;

	if (bHealthCheck)
	{
		if (LeftHp > RightHp)
		{
			SetEUderActivityEnum(EUderActivity::EUA_RIGHT);
		}
		else
		{
			SetEUderActivityEnum(EUderActivity::EUA_LEFT);
		}
		bHealthCheck = false;
	}



}

void AEnemyUder::BeginPlay()
{
	Super::BeginPlay();

	SetPullType(EPullType::EPT_DISALLOW);
	SetMonsterMoveState(EMonsterMoveState::EMMS_FORWARD);

	CurShiledRatio = ShiledTwoRatio;


	PS_Shiled_Blue->SetFloatParameter(FName("UderBarrierFade"), 1);
	PS_Shiled_Red->SetFloatParameter(FName("UderBarrierFade"), 0);

	PS_Shiled_Invi->SetFloatParameter(FName("UderInviFade"), 0);

	MonsterGuardPoint = EMonsterGuardPoint::EMGP_ON;

	BoomRangeSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BoomRangeSphere->OnComponentBeginOverlap.AddUniqueDynamic(this, &AEnemyUder::BoomRangeBeginOverlap);
	BoomRangeSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse(ECR_Ignore));


	auto StartBindLamb = this;

	FTimerDelegate StartBinding;
	StartBinding.BindLambda([StartBindLamb] {
		if (IsValid(StartBindLamb))
		{
			TotalHp = LeftHp + RightHp;
			StartBindLamb->TotalMaxHp = TotalHp;
			if (StartBindLamb->EnemyController)
			{
				StartBindLamb->EnemyController->GetBlackBoardComponent()->SetValueAsFloat(FName("TotalHp"), TotalHp);
				StartBindLamb->EnemyController->GetBlackBoardComponent()->SetValueAsFloat(FName("TotalMaxHp"), StartBindLamb->TotalMaxHp);
				if(StartBindLamb->Player)
					StartBindLamb->Player->UpdateUderHUD(true);
			}
		}
		});

	GetWorldTimerManager().SetTimer(StartTimer, StartBinding, 1.f, false);
}

void AEnemyUder::OnConstruction(const FTransform& Transform)
{
	SetMonsterType(EMonsterType::EMT_UDER);
	Super::OnConstruction(Transform);

	DynamicZeroMat = SetDynamicMat(ZeroMatInstance, DynamicZeroMat, 0, this);	//Head
	DynamicOneMat = SetDynamicMat(OneMatInstance, DynamicOneMat, 1, this);	//Arm
	DynamicTwoMat = SetDynamicMat(TwoMatInstance, DynamicTwoMat, 2, this);	//Body
}

void AEnemyUder::SetEUderActivityEnum(EUderActivity State)
{
	if (EnemyController)
	{
		SetEUderActivity(State);
		EnemyController->GetBlackBoardComponent()->SetValueAsEnum(FName("EUderActivity"), uint8(State));
	}
}

void AEnemyUder::SetEUderNumberingEnum(EUderNumbering State)
{
	if (EnemyController)
	{
		SetEUderNumbering(State);
		EnemyController->GetBlackBoardComponent()->SetValueAsEnum(FName("EUderNumbering"), uint8(State));
	}
}

void AEnemyUder::SetEUderInvincibilityEnum(EUderInvincibility State)
{
	if (EnemyController)
	{
		SetEUderInvincibility(State);
		EnemyController->GetBlackBoardComponent()->SetValueAsEnum(FName("EUderInvincibility"), uint8(State));
	}
}

float AEnemyUder::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	//Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	OnDamageColorMat(DynamicZeroMat, FName("DamageColorSwitch"), MatTimer_0);
	OnDamageColorMat(DynamicOneMat, FName("DamageColorSwitch"), MatTimer_0);
	OnDamageColorMat(DynamicTwoMat, FName("DamageColorSwitch"), MatTimer_0);

	ShieldFunc();

	if (DamageCauser)
	{
		auto Character = Cast<APlayerCharacter>(DamageCauser);
		if (Character)
		{
			auto GetKnockBack = Character->GetKnockBackStatus();
			switch (GetKnockBack)
			{
			case EKnockBackState::EKBS_KNOCKBACK:
				PlayFModEventSound(FMOD_KnockBackHitEvent);
				break;
			case EKnockBackState::EKBS_KNOCKDOWN:
				PlayFModEventSound(FMOD_KnockDownHitEvent);
				break;
			case EKnockBackState::EKBS_STUN:
				PlayFModEventSound(FMOD_StunHitEvent);
				break;
			}
		}
	}

	if (GetEnemyState() == EEnemyState::EES_DEAD)	return DamageAmount;

	ShowHealthBar();

	return DamageAmount;
}

void AEnemyUder::Accept(VisitorPtr Visitor)
{
	Visitor->Visit(this);
}

void AEnemyUder::Die()
{
	Super::Die();
	
	HideHealthBar();
	GetWorldTimerManager().ClearTimer(RushCoolDownTimer);
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);

	if (!Player->GetArrayCombatTarget().Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("UpdateUderHUD"));
		Player->UpdateUderHUD(false);
	}

}

void AEnemyUder::LoopDeath()
{
	FVector GetRoot = GetMesh()->GetBoneLocation(FName("Root"));

	UGameplayStatics::SpawnEmitterAtLocation(this, BoomSignParticles, GetRoot,
		GetActorRotation(), FVector(1.f, 1.f, 1.f));

	auto DestroyLamb = this;


	FTimerDelegate DestroyParticle;
	DestroyParticle.BindLambda([DestroyLamb,GetRoot] {
		if (IsValid(DestroyLamb))
		{
			if (DestroyLamb->BoomParticles)
			{
				UGameplayStatics::SpawnEmitterAtLocation(DestroyLamb, DestroyLamb->BoomParticles,
					FVector(GetRoot.X, GetRoot.Y, GetRoot.Z + 50.f),
					DestroyLamb->GetActorRotation(), DestroyLamb->DieBoomScale);

				UFMODBlueprintStatics::PlayEvent2D(DestroyLamb->GetWorld(), DestroyLamb->FMOD_UderBoom, true);
			}
		}
		});
	
	GetWorldTimerManager().SetTimer(DestroyParticleTimer, DestroyParticle, BoomTime - 0.1f, false);

	auto FinishDeathLamb = this;

	FTimerDelegate FinishDeath;
	FinishDeath.BindLambda([FinishDeathLamb] {
		if (IsValid( FinishDeathLamb))
		{
			FinishDeathLamb->ActivateCollision(FinishDeathLamb->BoomRangeSphere);
			if (FinishDeathLamb->BoomCameraShake)
				FinishDeathLamb->GetWorld()->GetFirstPlayerController()->PlayerCameraManager->StartMatineeCameraShake(FinishDeathLamb->BoomCameraShake);

			FinishDeathLamb->Destroy();
		}
		});

	GetWorldTimerManager().SetTimer(BoomTimer, FinishDeath, BoomTime, false);

}

void AEnemyUder::RushAttack()
{
	if (EnemyController)
	{
		if (EnemyController->GetBlackBoardComponent()->GetValueAsBool("bRushCoolDown"))
			return;

		SetEnemyState(EEnemyState::EES_ATTACK);
		SetMonsterKnockBackState(EMonsterKnockBackState::EMKS_KNOCKDOWN);
		EnemyController->GetBlackBoardComponent()->SetValueAsEnum(FName("EUderAttackState"), uint8(EUderAttackState::EUAS_ATTACKING));
		
		PlayHighPriorityMontage(RushMontage, FName("Rush"), 1.0f);
	
		EnemyController->StopMovement();
		if (Player)
		{
			FRotator LookAtYaw = GetLookAtRotationYaw(Player->GetActorLocation());
			SetActorRotation(LookAtYaw);
		}

		FHitResult HitResult;
		const FVector Start = GetActorLocation() + FVector(0.f, 0.f, -30.f);
		const FVector End = Start + FVector(0.f, 0.f, -400.f);
		FCollisionQueryParams QueryParams;
		QueryParams.bReturnPhysicalMaterial = true;

		GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_GameTraceChannel1, QueryParams);

		if (RushParticles)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), RushParticles, HitResult.Location + FVector(0.f, 0.f, 3.f), GetActorRotation());
		}

		EnemyController->GetBlackBoardComponent()->SetValueAsBool("bRushCoolDown", true);

		
	}
}

void AEnemyUder::RushEnd()
{
	auto RushCoolLambda = this;

	FTimerDelegate RushCoolDown;
	RushCoolDown.BindLambda([RushCoolLambda]() {
		if (IsValid(RushCoolLambda))
		{
			RushCoolLambda->EnemyController->GetBlackBoardComponent()->SetValueAsBool("bRushCoolDown", false);

		}
		});

	GetWorldTimerManager().SetTimer(RushCoolDownTimer, RushCoolDown, RushCoolDownTime, false);

	SetEnemyState(EEnemyState::EES_MOVE);
	SetMonsterGuardable(EMonsterGuardable::EMG_ABLE);

	if (EnemyController)
	{
		EnemyController->GetBlackBoardComponent()->SetValueAsEnum(FName("EUderAttackState"), uint8(EUderAttackState::EUAS_NOATTACKING));
	}
}

void AEnemyUder::Attack()
{
	Super::Attack();

	SetMonsterKnockBackState(EMonsterKnockBackState::EMKS_KNOCKBACK);
	bAttackRotChar = true;
	if (EnemyController)
	{
		EnemyController->GetBlackBoardComponent()->SetValueAsEnum(FName("EUderAttackState"), uint8(EUderAttackState::EUAS_ATTACKING));
	}
}

void AEnemyUder::AttackEnd()
{
	Super::AttackEnd();

	if (EnemyController)
	{
		EnemyController->GetBlackBoardComponent()->SetValueAsEnum(FName("EUderAttackState"), uint8(EUderAttackState::EUAS_NOATTACKING));
	}
}

void AEnemyUder::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (MatTimer_0.IsValid())
		GetWorldTimerManager().ClearTimer(MatTimer_0);

	if (BoomTimer.IsValid())
		GetWorldTimerManager().ClearTimer(BoomTimer);

	if (DestroyParticleTimer.IsValid())
		GetWorldTimerManager().ClearTimer(DestroyParticleTimer);

	if (StartTimer.IsValid())
		GetWorldTimerManager().ClearTimer(StartTimer);

	if (RushCoolDownTimer.IsValid())
		GetWorldTimerManager().ClearTimer(RushCoolDownTimer);


	Super::EndPlay(EndPlayReason);
}

void AEnemyUder::ShieldFunc()
{
	if (GetGP() <= GetMaxGP() / 2 && ShieldCount ==1)
	{
		PS_Shiled_Blue->SetFloatParameter(FName("UderBarrierFade"), 0);
		PS_Shiled_Red->SetFloatParameter(FName("UderBarrierFade"), 1);
		CurShiledRatio = ShiledOneRatio;
		--ShieldCount;
	}
	
	if(GetGP() <=0.f && ShieldCount == 0)
	{
		--ShieldCount;
		LeftArm->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		RightArm->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		LeftArm->SetVisibility(true);
		RightArm->SetVisibility(true);

		SetMIDScalarParameterValue(DynamicOneMat, FName("Opacity Mask"), 0.f);

		PS_Shiled_Blue->SetFloatParameter(FName("UderBarrierFade"), 0);
		PS_Shiled_Red->SetFloatParameter(FName("UderBarrierFade"), 0);
		CurShiledRatio = 1;
	}	
}

void AEnemyUder::BehHealthCheck()
{
	EnemyController->GetBlackBoardComponent()->SetValueAsBool(FName("bHealthCheck"), true);
	bHealthCheck = true;
}

void AEnemyUder::UderNumberChange(EUderNumbering Numbering)
{
	if (EnemyController->GetBlackBoardComponent()->GetValueAsEnum("EUderNumbering") == uint8(EUderNumbering::EUN_MAX))
		return;

	SetEUderNumberingEnum(Numbering);

	if (EnemyController->GetBlackBoardComponent()->GetValueAsEnum("EUderNumbering") == uint8(EUderNumbering::EUN_LAST))
	{
		SetEUderActivityEnum(EUderActivity::EUA_TOGETHER);
		SetEUderNumberingEnum(EUderNumbering::EUN_LASTTOGETHER);
		return;
	}

	auto Activity = EnemyController->GetBlackBoardComponent()->GetValueAsEnum(FName("EUderActivity"));
	if (Activity == uint8(EUderActivity::EUA_LEFT))
	{
		SetEUderActivityEnum(EUderActivity::EUA_RIGHT);
	}
	if(Activity == uint8(EUderActivity::EUA_RIGHT))
	{
		SetEUderActivityEnum(EUderActivity::EUA_LEFT);
	}
}

void AEnemyUder::InvinRetreatStart()
{
	bInvi = true;
}

void AEnemyUder::InvinRetreatEnd()
{
	bInvi = false;
}

void AEnemyUder::StunStart()
{
	Super::StunStart();

	DeactivateCollision(RightHandCollision);
	DeactivateCollision(GetRushCollision());
	PlayHighPriorityMontage(StunMontage, FName("StunStart"), 1.0f);
}

void AEnemyUder::EndTimer()
{
	Super::EndTimer();
	if (Player)
	{
		auto MyPlayerState = Player->GetCurrentState();
		if (MyPlayerState == EPlayerState::EPS_DEAD)
		{
			if (EnemyController)
			{
				GetWorld()->GetTimerManager().ClearTimer(RushCoolDownTimer);
			}
		}
	}
}

void AEnemyUder::SpawnInviParticle()
{
	GetInviParticleComponent()->SetFloatParameter(FName("UderInviFade"), 1);
}


void AEnemyUder::BoomRangeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	SetMonsterKnockBackState(EMonsterKnockBackState::EMKS_KNOCKDOWN);
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors, ACharacter::StaticClass());

	auto Uder = Cast<AEnemyUder>(OtherActor);

	for (auto Actor : OverlappingActors)
	{
		if (!Uder)
		{
			DoDamage(Actor, UderBoomAttackDamage, Actor->GetInstigatorController());
			DeactivateCollision(BoomRangeSphere);
		}
	}	
}

void AEnemyUder::EndVisibility()
{
	RightArm->SetVisibility(false);
	LeftArm->SetVisibility(false);
}

void AEnemyUder::DetachArm()
{
	RightArm->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	LeftArm->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
}