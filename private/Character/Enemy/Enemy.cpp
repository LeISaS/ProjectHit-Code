// Fill out your copyright notice in the Description page of Project Settings.


#include "../../../Public/Character/Enemy/Enemy.h"
#include "../../../Public/Character/Enemy/EnemyController.h"
#include "../../../Public/Character/PlayerCharacter/PlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/PoseableMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/ShapeComponent.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"


// Sets default values
AEnemy::AEnemy() :
	MonsterGuardPoint(EMonsterGuardPoint::EMGP_OFF),
	EnemyAutoTarget(EEnemyAutoTarget::EEAT_AUTOTARGET),
	MonsterMoveState(EMonsterMoveState::EMMS_IDLE),
	MonsterType(EMonsterType::EMT_OTHER),
	PullType(EPullType::EPT_ALLOW),
	BaseDamage(20.f),
	ParryingDamage(30.f),
	AcceptableRadius(50.f),
	DissolveTimeRate(0.25f),
	Dissolve(1.f),
	HitMontagePlayRate(1.f),
	AttackMontagePlayRate(1.f),
	OnDamageMatTime(0.2f),
	MaxGP(0),
	GP(0),
	bHitDamage(false),
	bAgroInit(false),
	HealthBarDisplayTime(4.f),
	DeathTime(3.f),
	CurShiledRatio(1),
	HitStopTime(0.5f),
	MoveLeftSpeed(GetCharacterMovement()->GetMaxSpeed()),
	MoveRightSpeed(GetCharacterMovement()->GetMaxSpeed()),
	MoveBackSpeed(GetCharacterMovement()->GetMaxSpeed()),
	AttackCoolDownTime(5.f)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AgroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AgroSphere"));
	AgroSphere->SetupAttachment(GetRootComponent());

	TargetWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("TargetWidget"));
	TargetWidget->SetupAttachment(GetRootComponent());

}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	RerunConstructionScripts();

	if(TargetWidget)
		TargetWidgetSetHidden();

	FindPlayerCharacter();

	if (SpawnParticles && bSpawnEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(
			SpawnParticles,
			GetMesh(),
			TEXT("None"),
			GetActorLocation() + FVector(100.f, 0.f, 0.f), GetActorRotation(), FVector(2.f, 2.f, 2.f),
			EAttachLocation::KeepWorldPosition);
	}

	SpawnDefaultController();

	AgroSphere->OnComponentBeginOverlap.AddUniqueDynamic(this, &AEnemy::AgroSphereOverlap);

	//Mesh the CollsiionChannel Setting
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	//Ignore the camera for mesh and capsule
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	//Get AIController
	EnemyController = Cast<AEnemyController>(GetController());
	
	//BlackBoard Init
	if (EnemyController)
	{
		EnemyController->GetBlackBoardComponent()->SetValueAsFloat(FName("Monster_Hp"), GetHp());
		EnemyController->GetBlackBoardComponent()->SetValueAsBool(FName("bAgroInit"), bAgroInit);
		EnemyController->RunBehaviorTree(BehaviorTree);
		SetEnemyState(EEnemyState::EES_IDLE);

	}
	SetGP(GetMaxGP());

	APlayerCharacter* player_ = Cast<APlayerCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (player_)
	{
		player_->AddAutoTargetingEnemy(this);
	}

	OnGaurdAble();

}
void AEnemy::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	//GetWorld()->GetTimerManager().ClearAllTimersForObject(this);

	if(CharacterComboTimer.IsValid())
		GetWorldTimerManager().ClearTimer(CharacterComboTimer);
	if (HealthBarTimer.IsValid())
		GetWorldTimerManager().ClearTimer(HealthBarTimer);
	if (DeathTimer.IsValid())
		GetWorldTimerManager().ClearTimer(DeathTimer);
	if (HitStopTimer.IsValid())
		GetWorldTimerManager().ClearTimer(HitStopTimer);
	if (MatTimer.IsValid())
		GetWorldTimerManager().ClearTimer(MatTimer);
	if (AttackCoolDownTimer.IsValid())
		GetWorldTimerManager().ClearTimer(AttackCoolDownTimer);

	if(EnemyDie_Del.IsBound())
		EnemyDie_Del.Clear();

	if (EnemyController)
	{
		EnemyController->UnPossess();
		EnemyController->Destroy();
	}
}
void AEnemy::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	UDataTable* DT_MonsterData = LoadObject<UDataTable>(nullptr, TEXT("DataTable'/Game/ProjectHit/Blueprint/DataTable/Monster/DT_Monster_Master.DT_Monster_Master'"));

	if (DT_MonsterData)
	{
		TArray<FName> RowNames = DT_MonsterData->GetRowNames();

		if (DT_MonsterData)
		{
			for (int i = 0; i < RowNames.Num(); ++i)
			{
				FMonsterMaster MonsterData = *(DT_MonsterData->FindRow<FMonsterMaster>(RowNames[i], RowNames[i].ToString()));

				if (MonsterData.Key == TableKey)
				{
					bAgroInit = MonsterData.MonsterSubType;
					SetMaxHp(MonsterData.MaxHealth);
					BaseDamage = MonsterData.BaseDamage;
					ParryingDamage = MonsterData.ParryingDamage;
					AcceptableRadius = MonsterData.AcceptableRadius;
					RetreatRadius = MonsterData.RetreatRadius;
					GetCharacterMovement()->MaxWalkSpeed = MonsterData.WalkSpeed;
					bSpawnEffect = MonsterData.SpawnEffect;
					SetHp(GetMaxHp());
					HitParticles = MonsterData.HitParticles;
				}
			}
		}
	}

	UDataTable* DT_HitData = LoadObject<UDataTable>(nullptr, TEXT("DataTable'/Game/ProjectHit/Blueprint/DataTable/Monster/DT_Monster_HitData.DT_Monster_HitData'"));

	EnemyInfo.Empty();
	if (DT_HitData)
	{
		TArray<FName> RowNames = DT_HitData->GetRowNames();
		if (RowNames.Num())
		{
			for (int i = 0; i < RowNames.Num(); i++)
			{
				FEnemyName EnemyName = *(DT_HitData->FindRow<FEnemyName>(RowNames[i], RowNames[i].ToString()));
				
				switch (GetMonsterType())
				{
				case EMonsterType::EMT_TOYMON:
					EnemyInfo.Add(EnemyName.Toymon);
					break;
				case EMonsterType::EMT_FORKYDOLL:
					EnemyInfo.Add(EnemyName.Forky);
					break;
				case EMonsterType::EMT_BOMBER:
					EnemyInfo.Add(EnemyName.Bomber);
					break;
				case EMonsterType::EMT_UDER:
					EnemyInfo.Add(EnemyName.Uder);
					break;
				case EMonsterType::EMT_DOGGY:
					EnemyInfo.Add(EnemyName.Doggy);
					break;
				case EMonsterType::EMT_KINGCROWN_2PHASE:
					EnemyInfo.Add(EnemyName.KingCrown_2Phase);
					break;
				}
			}
		}
	}

	
}

void AEnemy::Skill_HitParrying_Implementation(AActor* _Actor)
{
	//Parring ����
}

void AEnemy::EndTimer()
{
	if (Player)
	{
		auto MyPlayerState = Player->GetCurrentState();
		if (MyPlayerState == EPlayerState::EPS_DEAD)
		{
			if (EnemyController)
			{
				GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
				GetWorld()->GetTimerManager().ClearTimer(AttackCoolDownTimer);
				EnemyController->UnPossess();
				EnemyController->Destroy();
			}
		}
	}
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bAttackRotChar)
	{
		FRotator LookAtYaw = GetLookAtRotationYaw(GetPlayerLocation());
		SetActorRotation(LookAtYaw);

		bAttackRotChar = false;
	}

	//EndTimer();
}

/**<Start><Combat Section>*/

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	auto Character = Cast<APlayerCharacter>(DamageCauser);

	if (GetMonsterGuardPoint() == EMonsterGuardPoint::EMGP_ON && GP > 0)
	{
		if (GP - DamageAmount <= 0.f)
		{
			GP = 0;
			SetMonsterGuardPoint(EMonsterGuardPoint::EMGP_OFF);
		}
		else
			GP -= DamageAmount;
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
	}

	if (DamageCauser)
	{
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

	return DamageAmount;
}


void AEnemy::Die()
{
	if (GetEnemyState() == EEnemyState::EES_DEAD)
		return;

	GetWorld()->GetTimerManager().ClearTimer(AttackCoolDownTimer);

	AgroSphere->OnComponentBeginOverlap.RemoveAll(this);
	AgroSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	SetEnemyState(EEnemyState::EES_DEAD);

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && DeathMontage)
	{
		AnimInstance->Montage_Play(DeathMontage);
	}
	if (EnemyController)
	{
		EnemyController->StopMovement();
		EnemyController->UnPossess();
	}
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel5, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);

	TargetWidgetSetHidden();
	
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);

	auto spawnLocation = GetActorLocation() + DropOffset;
	for(auto& asset : DropAssets)
		GetWorld()->SpawnActor(asset,&spawnLocation);
}

void AEnemy::FinishDeath()
{
	GetMesh()->bPauseAnims = true;

	GetWorldTimerManager().SetTimer(DeathTimer, this, &AEnemy::DestroyEnemy, DeathTime);
}

void AEnemy::DestroyEnemy()
{
	if (EnemyController)
	{
		EnemyController->Destroy();
	}

	Destroy();
}

void AEnemy::SpawnBlood(FName SocketName, UParticleSystem* Particle, USkeletalMeshComponent* PlayerMesh,float Damage)
{
	const USkeletalMeshSocket* TipSocket{ GetMesh()->GetSocketByName(SocketName) };
	if (TipSocket&& Damage>=1)
	{	
		const FTransform SocketTransform = TipSocket->GetSocketTransform(PlayerMesh);

		UGameplayStatics::SpawnEmitterAttached(Particle, PlayerMesh, TEXT("None"), 
			SocketTransform.GetLocation(), SocketTransform.GetRotation().Rotator(), EAttachLocation::KeepWorldPosition);
	}
}
/**<Start><PlayerAttackCombo Reset>*/
void AEnemy::ResetTakeHitDamage(float AttackTiming)
{
	GetWorldTimerManager().ClearTimer(CharacterComboTimer);
	GetWorldTimerManager().SetTimer(CharacterComboTimer, this, &AEnemy::ComboTimerReset, AttackTiming);
}

void AEnemy::Knockback(APlayerCharacter* Target,int32 index)
{
	if (EnemyInfo.IsValidIndex(index))
	{
		auto dir = GetActorLocation() - Target->GetActorLocation();
		dir.Z = 0;
		dir.Normalize();
		LaunchCharacter(dir * EnemyInfo[index].KnockBackPower, false, false);
	}
}

void AEnemy::ComboTimerReset()
{
	bHitDamage = false;
}

void AEnemy::TargetWidgetSetVisible()
{
	TargetWidget->GetUserWidgetObject()->SetVisibility(ESlateVisibility::Visible);
}

void AEnemy::TargetWidgetSetHidden()
{
	TargetWidget->GetUserWidgetObject()->SetVisibility(ESlateVisibility::Hidden);
}

/**<Start><HealthBar>*/
void AEnemy::ShowHealthBar_Implementation()
{
	GetWorldTimerManager().ClearTimer(HealthBarTimer);
	GetWorldTimerManager().SetTimer(HealthBarTimer, this, &AEnemy::HideHealthBar, HealthBarDisplayTime);
}

/**MontageSection*/

void AEnemy::AgroSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		auto Character = Cast<APlayerCharacter>(OtherActor);
		if (Character)
		{
			bAgroInit = true;
			if (EnemyController)
			{
				EnemyController->GetBlackBoardComponent()->SetValueAsBool(FName("bAgroInit"), bAgroInit);
				
			}
		}
	}
}

void AEnemy::SetIsTrueHit()
{
	if (EnemyController)
	{
		int HitReactNumber = FMath::RandRange(0, 3);
		FString PlayHitReactSection = "HitReact_0" + FString::FromInt(HitReactNumber);

		SetEnemyState(EEnemyState::EES_KNOCKBACK);
		PlayHighPriorityMontage(HitMontage, FName(*PlayHitReactSection), HitMontagePlayRate);
		EnemyController->StopMovement();
	}
}

void AEnemy::SetIsFalseHit()
{
	if (EnemyController)
	{
		SetEnemyState(EEnemyState::EES_MOVE);
	}
}

void AEnemy::Attack()
{
	if (GetEnemyState() == EEnemyState::EES_DEAD)
		return;

	if (EnemyController->GetBlackBoardComponent()->GetValueAsBool("bAttackCoolDown"))
		return;


	bAttackRotChar = true;

	if (EnemyController)
	{
		SetEnemyState(EEnemyState::EES_ATTACK);
		EnemyController->StopMovement();
	}
	PlayHighPriorityMontage(AttackMontage, FName("Attack"), AttackMontagePlayRate);

	EnemyController->GetBlackBoardComponent()->SetValueAsBool("bAttackCoolDown", true);

	auto AttackCoolLamb = this;

	FTimerDelegate AttackCoolDown;

	AttackCoolDown.BindLambda([AttackCoolLamb]() {
		if (IsValid(AttackCoolLamb))
		{
			if (AttackCoolLamb->EnemyController)
			{
				AttackCoolLamb->EnemyController->GetBlackBoardComponent()->SetValueAsBool("bAttackCoolDown", false);
			}
		}
		});

	GetWorldTimerManager().SetTimer(AttackCoolDownTimer, AttackCoolDown, AttackCoolDownTime, false);

}

void AEnemy::AttackEnd()
{
	if (EnemyController)
	{
		SetEnemyState(EEnemyState::EES_MOVE);
		SetMonsterGuardable(EMonsterGuardable::EMG_ABLE);
	}
}

bool AEnemy::RetreatCheck()
{
	TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjectTypes;
	TraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

	UClass* PlayerCharacter = APlayerCharacter::StaticClass();

	TArray<AActor*> Actors;
	TArray<AActor*> OutActors;

	bool bResult = UKismetSystemLibrary::SphereOverlapActors(
		GetWorld(),
		GetActorLocation(),
		RetreatRadius,
		TraceObjectTypes,
		PlayerCharacter,
		Actors,
		OutActors
	);

	FColor DrawColor;

	if (bResult)
		DrawColor = FColor::Green;
	else
		DrawColor = FColor::Red;

	DrawDebugSphere(GetWorld(), GetActorLocation(), RetreatRadius, 12, DrawColor, false, 5.f);

	for (AActor* overlappedActor : OutActors)
	{
		if (PlayerCharacter)
		{
			return true;
		}
	}
	return false;
}


void AEnemy::OnDamageColorMat(UMaterialInstanceDynamic* Mat, FName Name, FTimerHandle TimerHandle)
{
	GetWorldTimerManager().ClearTimer(TimerHandle);
	SetMIDScalarParameterValue(Mat, Name, 1.0f);

	auto OffDamageLamb = this;

	FTimerDelegate OffDamageColor;
	OffDamageColor.BindLambda([OffDamageLamb,Mat,Name]() {
		if (OffDamageLamb)
		{
			OffDamageLamb->SetMIDScalarParameterValue(Mat, Name, 0.0f);
		}
		});

	GetWorldTimerManager().SetTimer(TimerHandle, OffDamageColor, OnDamageMatTime, false);
}


void AEnemy::DissolveChange(UMaterialInstanceDynamic* Mat, FName Name, float DeltaTime)
{
	if (Dissolve <= -1)
	{
		Dissolve = -1;
	}

	if (Mat)
	{
		Mat->SetScalarParameterValue(Name, Dissolve -= DeltaTime);
	}

}

float AEnemy::PlayHighPriorityMontage(UAnimMontage* Montage, FName StartSectionName, float InPlayRate)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (!AnimInstance->Montage_IsPlaying(Montage))
	{
		UAnimMontage* HighMontage = Montage;

		return PlayAnimMontage(HighMontage, InPlayRate, StartSectionName);
	}

	return 0.0f;
}

void AEnemy::MoveToTarget(APlayerCharacter* Target)
{
	if (EnemyController)
	{
		SetEnemyState(EEnemyState::EES_MOVE);
		FAIMoveRequest MoveRequest;
		MoveRequest.SetGoalActor(Target);
		MoveRequest.SetAcceptanceRadius(AcceptableRadius);
		FNavPathSharedPtr NavPath;
	
		EnemyController->GetBlackBoardComponent()->SetValueAsObject(FName("Target"), Target);
		switch (MonsterMoveState)
		{
		case EMonsterMoveState::EMMS_FORWARD:
			EnemyController->MoveToActor(Target, AcceptableRadius,true,false);
			EnemyController->ClearFocus(EAIFocusPriority::LastFocusPriority);
			break;
		case EMonsterMoveState::EMMS_FORWARD_2:
			EnemyController->MoveTo(MoveRequest, &NavPath);
			EnemyController->ClearFocus(EAIFocusPriority::LastFocusPriority);
			break;
		case EMonsterMoveState::EMMS_LEFT:
			boundaryCheck(-1, 90.f, MoveLeftSpeed);
			break;
		case EMonsterMoveState::EMMS_RIGHT:
			boundaryCheck(1, 90.f, MoveRightSpeed);
			break;
		case EMonsterMoveState::EMMS_BACK:
			boundaryCheck(-1, 180.f, MoveBackSpeed);
			break;
		}

		if (Target)
		{
			FRotator LookAtYaw = GetLookAtRotationYaw(GetPlayerLocation());
			UWorld* World = GetWorld();
			auto InterpRot = FMath::RInterpTo(GetActorRotation(), LookAtYaw, World->GetDeltaSeconds(), 10.0f);

			SetActorRotation(InterpRot);
		}
	}
}
void AEnemy::boundaryCheck(int dir, float Dest, float MoveSpeed)
{
	FVector Forward = GetActorForwardVector();

	FRotator DestRot = FRotator(0.f, Dest* dir, 0.f);
	FVector DestVec = DestRot.RotateVector(Forward);

	GetCharacterMovement()->MoveSmooth(DestVec * MoveSpeed, GetWorld()->GetDeltaSeconds());
}


FVector AEnemy::GetPlayerLocation()
{
	if (GetWorld()->GetFirstPlayerController()->GetPawn())
	{
		FVector PlayerLocation = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
		return PlayerLocation;
	}
	return FVector(0.f);
}

FRotator AEnemy::GetLookAtRotationYaw(FVector Target)
{
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target);
	FRotator LookAtRotationYaw(0.f, LookAtRotation.Yaw, 0.f);

	return LookAtRotationYaw;
}

void AEnemy::ExecuteVisitor(FString key)
{
	auto it = Visitors.find(key);
	if (it != Visitors.end())
		Accept(it->second);
}

void AEnemy::FindPlayerCharacter()
{
	class UWorld* const world = GetWorld();

	if (world)
	{
		for (TObjectIterator<APlayerCharacter> Itr; Itr; ++Itr)
		{
			if (Itr->IsA(APlayerCharacter::StaticClass()))
			{
				Player = *Itr;
			}
		}
	}
}

void AEnemy::PlayFModEventSound(class UFMODEvent* Event)
{
	if (Event)
	{
		UFMODBlueprintStatics::PlayEventAtLocation(GetWorld(), Event, GetActorTransform(), true);
	}
}

void AEnemy::Parrying()
{

}

void AEnemy::SetEnemyState(EEnemyState State)
{
	if (EnemyController)
	{
		EnemyState = State;

		EnemyController->GetBlackBoardComponent()->SetValueAsEnum(FName("EnemyState"), uint8(EnemyState));
	}
}

void AEnemy::KnockDown(APlayerCharacter* Target, int32 index)
{
	if (EnemyController)
	{
		SetEnemyState(EEnemyState::EES_KNOCKDOWN);
		EnemyController->StopMovement();

		if (EnemyInfo.IsValidIndex(index))
		{
			auto dir = GetActorLocation() - Target->GetActorLocation();
			dir.Z = 0;
			dir.Normalize();
			LaunchCharacter(dir * EnemyInfo[index].KnockDownPower, false, false);
		}
	}
}

void AEnemy::KnockDownEnd()
{
	if (EnemyController)
	{
		SetEnemyState(EEnemyState::EES_MOVE);
	}
}

void AEnemy::StunStart()
{
	if (EnemyController)
	{
		SetEnemyState(EEnemyState::EES_STUN);
		EnemyController->StopMovement();
	}
}

void AEnemy::StunEnd()
{
	if (EnemyController)
	{
		SetEnemyState(EEnemyState::EES_MOVE);
	}
}

void AEnemy::DieDelNotify()
{

	if (EnemyDie_Del.IsBound())
		EnemyDie_Del.Broadcast(this);
}


void AEnemy::OnGaurdAble()
{
	SetMonsterGuardable(EMonsterGuardable::EMG_ABLE);
}

void AEnemy::OffGaurdAble()
{
	SetMonsterGuardable(EMonsterGuardable::EMG_DISABLE);
}


void AEnemy::StartAttackCameraShake()
{
	if(AttackCameraShake)
		GetWorld()->GetFirstPlayerController()->PlayerCameraManager->StartMatineeCameraShake(AttackCameraShake);
}

void AEnemy::SetFocusBeh()
{
	FRotator LookAtYaw = GetLookAtRotationYaw(GetPlayerLocation());
	auto InterpRot = FMath::RInterpTo(GetActorRotation(), LookAtYaw, GetWorld()->GetDeltaSeconds(), 5.0f);
	SetActorRotation(InterpRot);
}
