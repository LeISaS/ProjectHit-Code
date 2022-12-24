// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Boss/TwoPhaseKingCrown.h"
#include "../../../Public/Character/Enemy/EnemyController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/PlayerCharacter/PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/BoxComponent.h"
#include "Character/Enemy/EnemyRangeToymonBlock.h"
#include "Kismet/GameplayStatics.h"
#include "../../../Public/Character/PlayerCharacter/PlayerCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "Particles/ParticleSystemComponent.h"
#include "Character/Boss/TwoPhaseJumpActor.h"

ATwoPhaseKingCrown::ATwoPhaseKingCrown():
	TwoBossState(ETwoBossState::ETBS_IDLE),
	TwoBossAttackState(ETwoBossAttackState::ETBAS_NONE),
	RightThrowAnimTime(1.0f),
	LeftThrowAnimTime(1.0f),
	Throw_1AnimTime(1.0f),
	Throw_2AnimTime(1.0f),
	Scrach_2AnimTime(1.0f),
	RightThrowAttackCoolDown(5.0f),
	LeftThrowAttackCoolDown(5.0f),
	Throw_1AttackCoolDown(5.0f),
	Throw_2AttackCoolDown(5.0f),
	Throw_2AttackCount(0),
	Throw_2ThrowCount(0),
	Throw_2PatternSpeed(3000.f),
	PatternSpeed(2500.f),
	bAttacking(false),
	bCondition(false),
	TwoPhaseShockWaveParticleDamage(50.f),
	PatternLeftSpeed(3000.f),
	PatternRightSpeed(3000.f),
	PatternThrow_1Speed(3000.f),
	PatternArc(0.85f),
	PatternLeftArc(0.85f),
	PatternRightArc(0.85f),
	PatternThrow_1Arc(0.85f),
	PatternThrow_2Arc(0.85f)
{
	PrimaryActorTick.bCanEverTick = true;

	LeftHandCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftHandCollision"));
	LeftHandCollision->SetupAttachment(GetMesh(), FName("LeftHandCollision"));

	RightHandCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("RightHandCollision"));
	RightHandCollision->SetupAttachment(GetMesh(), FName("RightHandCollision"));

	RightHandCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	RightHandCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	RightHandCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	RightHandCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel5, ECollisionResponse::ECR_Overlap);

	LeftHandCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	LeftHandCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	LeftHandCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	LeftHandCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel5, ECollisionResponse::ECR_Overlap);

	Visitors.insert(std::make_pair("Attack", std::make_shared<AttackVisitor>()));
	Visitors.insert(std::make_pair("RightThrowAttack", std::make_shared< TwoPhaseRightAttackVisitor>()));
	Visitors.insert(std::make_pair("LeftThrowAttack", std::make_shared< TwoPhaseLeftAttackVisitor>()));
	Visitors.insert(std::make_pair("Throw_1Attack", std::make_shared< TwoPhaseThrow_1Visitor>()));
	Visitors.insert(std::make_pair("Throw_2Attack", std::make_shared< TwoPhaseThrow_2Visitor>()));
	Visitors.insert(std::make_pair("Die", std::make_shared< DieVisitor>()));
	Visitors.insert(std::make_pair("JumpStart", std::make_shared< TwoPhaseJumpVisitor>()));
	Visitors.insert(std::make_pair("SetTwoBossNoAttack", std::make_shared< TwoPhaseAttackingStateVisitor>()));
	Visitors.insert(std::make_pair("Scrach_2", std::make_shared< TwoPhaseScrachVisitor>()));
}

void ATwoPhaseKingCrown::OnConstruction(const FTransform& Transform)
{
	SetMonsterType(EMonsterType::EMT_KINGCROWN_2PHASE);
	Super::OnConstruction(Transform);

	DynamicZeroMat = SetDynamicMat(ZeroMatInstance, DynamicZeroMat, 1, this);	//kingcrown
	DynamicOneMat = SetDynamicMat(OneMatInstance, DynamicOneMat, 2, this);		//facial
}

void ATwoPhaseKingCrown::BeginPlay()
{
	Super::BeginPlay();
	

	TargetWidgetSetHidden();
	SetPullType(EPullType::EPT_DISALLOW);
	SetEnemyState(EEnemyState::EES_MOVE);

	SetMonsterMoveState(EMonsterMoveState::EMMS_FORWARD);

	RightHandCollision->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::RightHandAttackOverlap);
	RightHandCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	LeftHandCollision->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::LeftHandAttackOverlap);
	LeftHandCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (EnemyController)
	{
		EnemyController->GetBlackBoardComponent()->SetValueAsObject(FName("Target"), Player);
	}
	
	if (!JumpCurve)
		return;

	FOnTimelineFloat TimelineProgress;
	TimelineProgress.BindUFunction(this, FName("JumpTimelineProgress"));
	JumpCurveFTimeline.AddInterpFloat(JumpCurve, TimelineProgress);

	JumpCurveFTimeline.SetTimelineLengthMode(TL_LastKeyFrame);

	if (JumpActor_1) Arr_JumpActor.Add(JumpActor_1);
	if (JumpActor_2) Arr_JumpActor.Add(JumpActor_2);
	if (JumpActor_3) Arr_JumpActor.Add(JumpActor_3);
	if (JumpActor_4) Arr_JumpActor.Add(JumpActor_4);

	if (Player)
	{
		Player->SetCombatTarget(this);
		Player->UpdateTwoKingCrownHUD(true);
	}

	
}

void ATwoPhaseKingCrown::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetEnemyState() == EEnemyState::EES_DEAD)
		return;

	if (JumpCurveFTimeline.IsPlaying())
	{
		JumpCurveFTimeline.TickTimeline(DeltaTime);
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

	if (bAttacking)
	{
		FRotator LookAtYaw = GetLookAtRotationYaw(GetPlayerLocation());
		auto InterpRot = FMath::RInterpTo(GetActorRotation(), LookAtYaw, DeltaTime, 5.0f);
		SetActorRotation(InterpRot);
	}	

	if (GetTwoBossState() == ETwoBossState::ETBS_IDLE)
		MoveToTarget(Player);
}

float ATwoPhaseKingCrown::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (DamageCauser)
	{
		auto Character = Cast<APlayerCharacter>(DamageCauser);

		if (Character)
		{
			if (TwoBossState == ETwoBossState::ETBS_IDLE)
			{
				if (Character->GetKnockBackStatus() == EKnockBackState::EKBS_KNOCKBACK)
				{
					Start_ShortHit();
					Knockback(Character, Character->GetCurrentSkillID());
				}
				else
				{
					Start_LongHit();
					Knockback(Character, Character->GetCurrentSkillID());		
				}
				bAttacking = true;
			}
		}
	}


	OnDamageColorMat(DynamicZeroMat, FName("DamageColorSwitch"), MatTimer_0);
	OnDamageColorMat(DynamicOneMat, FName("DamageColorSwitch"), MatTimer_0);

	return DamageAmount;
}

void ATwoPhaseKingCrown::LeftHandAttackOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto Character = Cast<APlayerCharacter>(OtherActor);

	if (Character)
	{
		float damage = DoDamage(Character, BaseDamage, EnemyController);
		SpawnBlood(FName("FX_Socket_Left"), BloodParticles, Character->GetMesh(), damage);
		DeactivateCollision(LeftHandCollision);
	}
}

void ATwoPhaseKingCrown::RightThrowBlockSpawn()
{
	if (GetWorld())
	{
		FRotator BlockSocketRot = GetMesh()->GetSocketRotation(FName("RightHandCollision"));
		FVector BlockSocketLoc = GetMesh()->GetSocketLocation(FName("RightHandCollision"));
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;

		Block = GetWorld()->SpawnActor<AEnemyRangeToymonBlock>(BlockClass, BlockSocketLoc, BlockSocketRot, SpawnParams);

		if (Block)
		{
			Block->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("RightHandCollision"));
		}
		if (FMOD_BlockSpawn)
		{
			PlayFModEventSound(FMOD_BlockSpawn);
		}
	}

}

void ATwoPhaseKingCrown::RightThrowPatternStart()
{
	if (GetEnemyState() == EEnemyState::EES_DEAD)
		return;

	//if (EnemyController->GetBlackBoardComponent()->GetValueAsBool("bRightThrowAttackCoolDown"))
	//	return;

	if (EnemyController)
	{
		if (DynamicOneMat)
			SetMIDScalarParameterValue(DynamicOneMat, FName("Facial Expression Selector"), 1.f);
		SetTwoBossPattern(ETwoBossState::ETBS_RIGHTTHROW);
		SetTwoBossAttackPattern(ETwoBossAttackState::ETBAS_ATTACKING);
		SetEnemyState(EEnemyState::EES_ATTACK);
		bAttacking = true;

		PatternArc = PatternRightArc;

		EnemyController->StopMovement();
		PlayHighPriorityMontage(RightThrowMontage, FName("Start"), RightThrowAnimTime);

		PatternSpeed = PatternRightSpeed;
		//EnemyController->GetBlackBoardComponent()->SetValueAsBool("bRightThrowAttackCoolDown", true);
		
	}
}

void ATwoPhaseKingCrown::RightThrowPatternEnd()
{
	if (EnemyController)
	{

		if (DynamicOneMat)
			SetMIDScalarParameterValue(DynamicOneMat, FName("Facial Expression Selector"), 0.f);
		SetTwoBossPattern(ETwoBossState::ETBS_IDLE);
		SetEnemyState(EEnemyState::EES_MOVE);

		bAttacking = false;
		/*auto RightThrowLamb = this;

		FTimerDelegate RightThrowCoolDown;

		RightThrowCoolDown.BindLambda([RightThrowLamb]() {
			if (IsValid(RightThrowLamb))
			{
				if (RightThrowLamb->EnemyController)
				{
					RightThrowLamb->EnemyController->GetBlackBoardComponent()->SetValueAsBool("bRightThrowAttackCoolDown", false);
				}
			}
			});

		GetWorldTimerManager().SetTimer(RightThrowHandle, RightThrowCoolDown, RightThrowAttackCoolDown, false);*/
	}
}

void ATwoPhaseKingCrown::LeftThrowBlockSpawn()
{
	if (GetWorld())
	{
		FRotator BlockSocketRot = GetMesh()->GetSocketRotation(FName("LeftHandCollision"));
		FVector BlockSocketLoc = GetMesh()->GetSocketLocation(FName("LeftHandCollision"));

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;

		Block = GetWorld()->SpawnActor<AEnemyRangeToymonBlock>(BlockClass, BlockSocketLoc, BlockSocketRot, SpawnParams);

		if (Block)
		{
			Block->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("LeftHandCollision"));
		}

		if (FMOD_BlockSpawn)
		{
			PlayFModEventSound(FMOD_BlockSpawn);
		}
	}
}

void ATwoPhaseKingCrown::LeftThrowPatternStart()
{
	if (GetEnemyState() == EEnemyState::EES_DEAD)
		return;

	/*if (EnemyController->GetBlackBoardComponent()->GetValueAsBool("bLeftThrowAttackCoolDown"))
		return;*/

	if (EnemyController)
	{
		if (DynamicOneMat)
			SetMIDScalarParameterValue(DynamicOneMat, FName("Facial Expression Selector"), 1.f);
		SetTwoBossPattern(ETwoBossState::ETBS_LEFTTHROW);
		SetEnemyState(EEnemyState::EES_ATTACK);
		SetTwoBossAttackPattern(ETwoBossAttackState::ETBAS_ATTACKING);
		
		PatternArc = PatternLeftArc;

		bAttacking = true;
		EnemyController->StopMovement();
		PlayHighPriorityMontage(LeftThrowMontage, FName("Start"), LeftThrowAnimTime);

		PatternSpeed = PatternLeftSpeed;
		//EnemyController->GetBlackBoardComponent()->SetValueAsBool("bLeftThrowAttackCoolDown", true);
	}
}

void ATwoPhaseKingCrown::LeftThrowPatternEnd()
{
	if (EnemyController)
	{
		if (DynamicOneMat)
			SetMIDScalarParameterValue(DynamicOneMat, FName("Facial Expression Selector"), 0.f);
		SetTwoBossPattern(ETwoBossState::ETBS_IDLE);
		SetEnemyState(EEnemyState::EES_MOVE);

		bAttacking = false;

		/*auto LeftThrowLamb = this;

		FTimerDelegate LeftThrowCoolDown;

		LeftThrowCoolDown.BindLambda([LeftThrowLamb]() {
			if (IsValid(LeftThrowLamb))
			{
				if (LeftThrowLamb->EnemyController)
				{
					LeftThrowLamb->EnemyController->GetBlackBoardComponent()->SetValueAsBool("bLeftThrowAttackCoolDown", false);
				}
			}
			});

		GetWorldTimerManager().SetTimer(LeftThrowHandle, LeftThrowCoolDown, LeftThrowAttackCoolDown, false);*/
	}
}



void ATwoPhaseKingCrown::Throw_1PatternStart()
{
	if (GetEnemyState() == EEnemyState::EES_DEAD)
		return;

	/*if (EnemyController->GetBlackBoardComponent()->GetValueAsBool("bThorw_1CoolDown"))
		return;*/

	if (EnemyController)
	{

		if (DynamicOneMat)
			SetMIDScalarParameterValue(DynamicOneMat, FName("Facial Expression Selector"), 1.f);

		SetTwoBossPattern(ETwoBossState::ETBS_THROW_1);
		SetEnemyState(EEnemyState::EES_ATTACK);
		SetTwoBossAttackPattern(ETwoBossAttackState::ETBAS_ATTACKING);
		bAttacking = true;

		PatternArc = PatternThrow_1Arc;

		EnemyController->StopMovement();
		PlayHighPriorityMontage(Throw_1Montage, FName("Start"), Throw_1AnimTime);

		PatternSpeed = PatternThrow_1Speed;

		//EnemyController->GetBlackBoardComponent()->SetValueAsBool("bThorw_1CoolDown", true);
	}
}

void ATwoPhaseKingCrown::Throw_1PatternEnd()
{
	if (EnemyController)
	{
		if (DynamicOneMat)
			SetMIDScalarParameterValue(DynamicOneMat, FName("Facial Expression Selector"), 0.f);

		SetTwoBossPattern(ETwoBossState::ETBS_IDLE);
		SetEnemyState(EEnemyState::EES_MOVE);

		bAttacking = false;

		/*auto Throw_1Lamb = this;

		FTimerDelegate Throw_1CoolDown;

		Throw_1CoolDown.BindLambda([Throw_1Lamb]() {
			if (IsValid(Throw_1Lamb))
			{
				if (Throw_1Lamb->EnemyController)
				{
					Throw_1Lamb->EnemyController->GetBlackBoardComponent()->SetValueAsBool("bThorw_1CoolDown", false);
				}
			}
			});

		GetWorldTimerManager().SetTimer(Throw_1Handle, Throw_1CoolDown, Throw_1AttackCoolDown, false);*/
	}
}

void ATwoPhaseKingCrown::Throw_2PatternStart()
{
	if (GetEnemyState() == EEnemyState::EES_DEAD)
		return;

	/*if (EnemyController->GetBlackBoardComponent()->GetValueAsBool("bThrow_2CoolDown"))
		return;*/

	if (EnemyController)
	{
		if (DynamicOneMat)
			SetMIDScalarParameterValue(DynamicOneMat, FName("Facial Expression Selector"), 1.0f);

		SetTwoBossPattern(ETwoBossState::ETBS_THROW_2);
		SetTwoBossAttackPattern(ETwoBossAttackState::ETBAS_ATTACKING);
		SetEnemyState(EEnemyState::EES_ATTACK);
		bAttacking = true;

		PatternArc = PatternThrow_2Arc;

		EnemyController->StopMovement();
		PlayHighPriorityMontage(Throw_2Montage, FName("Start"), Throw_2AnimTime);

		//EnemyController->GetBlackBoardComponent()->SetValueAsBool("bThrow_2CoolDown", true);
		
	}
}

void ATwoPhaseKingCrown::Throw_2PatternSpawn()
{
	if (GetWorld())
	{
		FString ThrowCount = "BlockSpawn_" + FString::FromInt(Throw_2AttackCount);

		FRotator BlockSocketRot = GetMesh()->GetSocketRotation(FName(*ThrowCount));
		FVector BlockSocketLoc = GetMesh()->GetSocketLocation(FName(*ThrowCount));

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;

		Block_2 = GetWorld()->SpawnActor<AEnemyRangeToymonBlock>(Block_2Class, BlockSocketLoc, BlockSocketRot, SpawnParams);

		Throw_2BlockInfo.Add(Block_2);

		if (Block_2)
		{
			Throw_2BlockInfo[Throw_2AttackCount]->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName(*ThrowCount));
		}
		if (FMOD_BlockSpawn)
		{
			PlayFModEventSound(FMOD_BlockSpawn);
		}

		Throw_2AttackCount++;
	}
}

void ATwoPhaseKingCrown::Throw_2PatternEnd()
{
	if (EnemyController)
	{
		if (DynamicOneMat)
			SetMIDScalarParameterValue(DynamicOneMat, FName("Facial Expression Selector"), 0.f);

		SetTwoBossPattern(ETwoBossState::ETBS_IDLE);
		SetEnemyState(EEnemyState::EES_MOVE);
		Throw_2AttackCount = 0;
		Throw_2ThrowCount = 0;
		Throw_2BlockInfo.Empty();
		bAttacking = false;

		/*auto Throw_2Lamb = this;

		FTimerDelegate Throw_2CoolDown;

		Throw_2CoolDown.BindLambda([Throw_2Lamb]() {
			if (IsValid(Throw_2Lamb))
			{
				if (Throw_2Lamb->EnemyController)
				{
					Throw_2Lamb->EnemyController->GetBlackBoardComponent()->SetValueAsBool("bThrow_2CoolDown", false);
				}
			}
			});

		GetWorldTimerManager().SetTimer(Throw_2Handle, Throw_2CoolDown, Throw_2AttackCoolDown, false);*/
	}
}

void ATwoPhaseKingCrown::Thorw_2DetachBlock()
{
	if (Throw_2BlockInfo.Num())
	{
		if (Block_2)
		{
			Throw_2BlockInfo[Throw_2ThrowCount]->StartFire(Throw_2BlockInfo[Throw_2ThrowCount], Throw_2PatternSpeed,PatternArc);
			Throw_2ThrowCount++;
		}
	}
}

void ATwoPhaseKingCrown::JumpPatternStart()
{
	//Jump();
	PlayHighPriorityMontage(JumpMontage, FName("Start"), 1.0f);
	EnemyController->StopMovement();
	SetTwoBossPattern(ETwoBossState::ETBS_JUMP);
	bCondition = false;

	if (DynamicOneMat)
		SetMIDScalarParameterValue(DynamicOneMat, FName("Facial Expression Selector"), 1.f);
}

void ATwoPhaseKingCrown::JumpingMove()
{
	bAttacking = true;
	JumpStartLoc = this->GetActorTransform().GetLocation();
	JumpStartLoc.Z = 320.f;
	JumpCurveFTimeline.PlayFromStart();
	
	TArray<int> JumpLocation;
	TArray<int> TempLocation;
	int MaxInit = INT_MIN;

	for (int i = 0; i < Arr_JumpActor.Num(); i++)
		JumpLocation.Add(FMath::Abs((GetActorLocation().Size() - GetRandomPosition(i).Size())));

	TempLocation = JumpLocation;

	JumpLocation.Sort();

	int32 RandomValueNumber = FMath::RandRange(2,3);

	int32 NumberIndex = JumpLocation.IndexOfByKey(TempLocation[RandomValueNumber]);

	JumpAnchor = Arr_JumpActor[NumberIndex]->GetActorLocation();
	JumpAnchor.Z += 1000.f;

	JumpTargetLoc = Arr_JumpActor[NumberIndex]->GetActorLocation();
	JumpTargetLoc.Z = 320.f;
}

void ATwoPhaseKingCrown::JumpTimelineProgress(float Value)
{
	JumpResultLoc = GetLocationBezier(JumpStartLoc, JumpAnchor, JumpTargetLoc, Value);

	SetActorLocation(JumpResultLoc);
}


FVector ATwoPhaseKingCrown::GetRandomPosition(int index)
{
	FVector Extent = Arr_JumpActor[index]->GetBoxComponent()->GetScaledBoxExtent();
	FVector Origin = Arr_JumpActor[index]->GetBoxComponent()->GetComponentLocation();

	FVector Point = UKismetMathLibrary::RandomPointInBoundingBox(Origin, Extent);

	return Point;
}

void ATwoPhaseKingCrown::JumpPatternEnd()
{
	SetTwoBossPattern(ETwoBossState::ETBS_IDLE);
	SetEnemyState(EEnemyState::EES_MOVE);

	if (DynamicOneMat)
		SetMIDScalarParameterValue(DynamicOneMat, FName("Facial Expression Selector"), 0.f);
}

FVector ATwoPhaseKingCrown::GetLocationBezier(FVector StartPoint, FVector Anchor, FVector EndPoint, float Time)
{
	FVector TempA = FMath::Lerp(StartPoint, Anchor, Time);
	FVector TempB = FMath::Lerp(Anchor, EndPoint, Time);

	FVector Result = FMath::Lerp(TempA, TempB, Time);

	return Result;
}

void ATwoPhaseKingCrown::AttackScratch_2()
{
	if (GetEnemyState() == EEnemyState::EES_DEAD)
		return;

	if (EnemyController)
	{
		SetTwoBossPattern(ETwoBossState::ETBS_SCRATCH_2);
		SetTwoBossAttackPattern(ETwoBossAttackState::ETBAS_ATTACKING);
		SetEnemyState(EEnemyState::EES_ATTACK);
		bAttacking = true;

		EnemyController->StopMovement();
		PlayHighPriorityMontage(Scrach_2Montage, FName("Attack"), Scrach_2AnimTime);

		//EnemyController->GetBlackBoardComponent()->SetValueAsBool("bThrow_2CoolDown", true);

	}
}

void ATwoPhaseKingCrown::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (MatTimer_0.IsValid())
		GetWorldTimerManager().ClearTimer(MatTimer_0);

	if (RightThrowHandle.IsValid())
		GetWorldTimerManager().ClearTimer(RightThrowHandle);

	if (LeftThrowHandle.IsValid())
		GetWorldTimerManager().ClearTimer(LeftThrowHandle);

	if (Throw_1Handle.IsValid())
		GetWorldTimerManager().ClearTimer(Throw_1Handle);

	if (Throw_2Handle.IsValid())
		GetWorldTimerManager().ClearTimer(Throw_2Handle);

	Super::EndPlay(EndPlayReason);
}

void ATwoPhaseKingCrown::JumpSpawnParticle()
{
	if (PS_2PageShockWave)
	{
		FHitResult HitResult;
		const FVector Start = GetActorLocation();
		const FVector End = Start + FVector(0.f, 0.f, -300.f);
		FCollisionQueryParams QueryParams;
		QueryParams.bReturnPhysicalMaterial = true;

		bool bResult = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_GameTraceChannel1, QueryParams);

		if (bResult)
		{

			UParticleSystemComponent* PSC = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),
				PS_2PageShockWave, HitResult.Location + FVector(0.f, 0.f, 5.f), FRotator(0.f));

			FScriptDelegate ParticleDelegate;
			ParticleDelegate.BindUFunction(this, "OnParticleHit");
			PSC->OnParticleCollide.Add(ParticleDelegate);
		}
	}
}

void ATwoPhaseKingCrown::OnParticleHit(FName EventName, float EmitterTime, int32 ParticleTime, FVector Location, FVector Velocity, FVector Direction, FVector Normal, FName BoneName, UPhysicalMaterial* PhysMat)
{
	if (!bCondition)
	{		
		if (Player)
		{
			float damage = DoDamage(Player, TwoPhaseShockWaveParticleDamage, EnemyController);
			SpawnBlood(FName("FX_Socket_Right"), BloodParticles, Player->GetMesh(), damage);
		}
		bCondition = true;
	}
}



void ATwoPhaseKingCrown::Start_ShortHit()
{
	SetEnemyState(EEnemyState::EES_STUN);
	DeactivateCollision(RightHandCollision);
	DeactivateCollision(LeftHandCollision);
	PlayHighPriorityMontage(ShortHitMontage, FName("Start"), 1.0f);
}

void ATwoPhaseKingCrown::Start_LongHit()
{
	SetEnemyState(EEnemyState::EES_STUN);
	DeactivateCollision(RightHandCollision);
	DeactivateCollision(LeftHandCollision);
	PlayHighPriorityMontage(LongHitMontage, FName("Start"), 1.0f);
}

void ATwoPhaseKingCrown::StunEnd()
{
	Super::StunEnd();
	
	bAttacking = false;
}

void ATwoPhaseKingCrown::Die()
{
	DeactivateCollision(RightHandCollision);
	DeactivateCollision(LeftHandCollision);

	if (Player)
	{
		Player->UpdateTwoKingCrownHUD(false);
	}

	for (auto temp : Throw_2BlockInfo)
	{
		temp->Destroy();
	}

	Throw_2BlockInfo.Empty();

	Super::Die();
	bAttacking = false;
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
}

void ATwoPhaseKingCrown::DeattachBlock()
{
	if (Block)
	{
		Block->StartFire(Block, PatternSpeed,PatternArc);
	}
}

void ATwoPhaseKingCrown::Attack()
{
	Super::Attack();
	if (DynamicOneMat)
		SetMIDScalarParameterValue(DynamicOneMat, FName("Facial Expression Selector"), 1.f);
	SetTwoBossAttackPattern(ETwoBossAttackState::ETBAS_ATTACKING);
	SetTwoBossPattern(ETwoBossState::ETBS_SCRATCH);
}

void ATwoPhaseKingCrown::AttackEnd()
{
	Super::AttackEnd();
	if (DynamicOneMat)
		SetMIDScalarParameterValue(DynamicOneMat, FName("Facial Expression Selector"), 0.f);
	SetTwoBossPattern(ETwoBossState::ETBS_IDLE);
	SetEnemyState(EEnemyState::EES_MOVE);
}

void ATwoPhaseKingCrown::Accept(VisitorPtr Visitor)
{
	Visitor->Visit(this);
}

void ATwoPhaseKingCrown::RightHandAttackOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto Character = Cast<APlayerCharacter>(OtherActor);

	if (Character)
	{
		float damage = DoDamage(Character, BaseDamage, EnemyController);
		SpawnBlood(FName("FX_Socket_Right"), BloodParticles, Character->GetMesh(), damage);
		DeactivateCollision(RightHandCollision);
	}
}



void ATwoPhaseKingCrown::SetTwoBossPattern(ETwoBossState State)
{
	if (EnemyController)
	{
		SetTwoBossState(State);
		EnemyController->GetBlackBoardComponent()->SetValueAsEnum(FName("ETwoBossState"), uint8(State));
	}
}

void ATwoPhaseKingCrown::SetTwoBossNoAttack()
{
	SetTwoBossAttackPattern(ETwoBossAttackState::ETBAS_NONE);
}

void ATwoPhaseKingCrown::SetTwoBossAttackPattern(ETwoBossAttackState State)
{
	if (EnemyController)
	{
		SetTwoBossAttackState(State);
		EnemyController->GetBlackBoardComponent()->SetValueAsEnum(FName("ETwoBossAttackState"), uint8(State));
	}
}
