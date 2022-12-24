// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Enemy/EnemyBomber.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../../../Public/Character/Enemy/EnemyController.h"
#include "../../../Public/Character/PlayerCharacter/PlayerCharacter.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"

AEnemyBomber::AEnemyBomber():
	IgnitionSpeed(150.f),
	BoomDamage(100.f)
{
	PrimaryActorTick.bCanEverTick = true;

	IgnitionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("IgnitionSphere"));
	IgnitionSphere->SetupAttachment(GetRootComponent());

	HealthBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidget"));
	HealthBarWidget->SetupAttachment(GetRootComponent());

	Visitors.insert(std::make_pair("Die", std::make_shared<DieVisitor>()));
	Visitors.insert(std::make_pair("Attack", std::make_shared<AttackVisitor>()));
}

void AEnemyBomber::BeginPlay()
{
	Super::BeginPlay();
	SetMonsterMoveState(EMonsterMoveState::EMMS_FORWARD);


	IgnitionSphere->OnComponentBeginOverlap.AddUniqueDynamic(this, &AEnemyBomber::IgnitionSphereBeginOverlap);

	IgnitionSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);

	if (BoomMatCurve)
	{
		FOnTimelineFloat MovementLineFunction;
		MovementLineFunction.BindUFunction(this, FName("BoomLine"));
		BoomMatTimeline.AddInterpFloat(BoomMatCurve, MovementLineFunction);

		BoomMatTimeline.SetTimelineLengthMode(TL_LastKeyFrame);
	}
}

void AEnemyBomber::OnConstruction(const FTransform& Transform)
{
	SetMonsterType(EMonsterType::EMT_BOMBER);
	Super::OnConstruction(Transform);


	DynamicZeroMat = SetDynamicMat(ZeroMatInstance, DynamicZeroMat, 0, this);	//Ballon
	DynamicOneMat = SetDynamicMat(OneMatInstance, DynamicOneMat, 1, this);	//Body
}

void AEnemyBomber::Tick(float DeltaTime)
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

	if (BoomMatTimeline.IsPlaying())
	{
		BoomMatTimeline.TickTimeline(DeltaTime);
	}
}

void AEnemyBomber::IgnitionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto Character = Cast<APlayerCharacter>(OtherActor);
	
	if (Character)
	{
		if (EnemyController)
		{
			EnemyController->GetBlackBoardComponent()->SetValueAsBool(FName("Ignition"), true);

			GetCharacterMovement()->MaxWalkSpeed = IgnitionSpeed;

			BoomMatTimeline.PlayFromStart();
		}
	}
}

float AEnemyBomber::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
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
				Knockback(Character, Character->GetCurrentSkillID());
				EnemyController->GetBlackBoardComponent()->SetValueAsBool(FName("bKnockBack"), true);

				auto ChangeStateLamb = this;


				FTimerDelegate ChangeState;				
				ChangeState.BindLambda([ChangeStateLamb]()
					{
						if (IsValid(ChangeStateLamb))
						{
							ChangeStateLamb->EnemyController->GetBlackBoardComponent()->SetValueAsBool(FName("bKnockBack"), false);
						}
					});

				GetWorldTimerManager().SetTimer(ChangeStateHandle, ChangeState, 0.1f, false);

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

void AEnemyBomber::Boom()
{
	TArray<AActor*>OverlappingActors;
	GetOverlappingActors(OverlappingActors, ACharacter::StaticClass());

	for (auto Actor : OverlappingActors)
	{
		auto Character = Cast<APlayerCharacter>(Actor);
		if (Character)
		{
			DoDamage(Character, BaseDamage, EnemyController);

			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BloodParticles, Player->GetTransform());
		}
	}
	if (ExplodeParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplodeParticles, GetActorLocation()+ ExplodeVector, FRotator(0.f), true);
	}

	if (BalloonBoomParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BalloonBoomParticles, GetActorLocation()+ BoomBallonVector, FRotator(0.f), true);
	}

	PlayFModEventSound(FMOD_Die);

	auto DestroyLamb = this;

	FTimerDelegate DestroyCallback;
	DestroyCallback.BindLambda([DestroyLamb]() {
		if (IsValid(DestroyLamb))
		{
			DestroyLamb->Destroy();
		}
		});

	GetWorldTimerManager().SetTimer(DestoryTimer, DestroyCallback, 0.1f, false);
}

void AEnemyBomber::BoomLine(float Value)
{
	SetMIDScalarParameterValue(DynamicOneMat, FName("TikTokColorSwitch"), Value);
}

void AEnemyBomber::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (DeathTimer.IsValid())
		GetWorldTimerManager().ClearTimer(DeathTimer);
	if (DestoryTimer.IsValid())
		GetWorldTimerManager().ClearTimer(DestoryTimer);
	if (MatTimer_0.IsValid())
		GetWorldTimerManager().ClearTimer(MatTimer_0);
	if (ChangeStateHandle.IsValid())
		GetWorldTimerManager().ClearTimer(ChangeStateHandle);

	Super::EndPlay(EndPlayReason);
}

void AEnemyBomber::Accept(VisitorPtr Visitor)
{
	Visitor->Visit(this);
}

void AEnemyBomber::Die()
{
	Super::Die();

	HideHealthBar();

	IgnitionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	auto DeathCallLamb = this;

	FTimerDelegate DeathCallback;
	DeathCallback.BindLambda([DeathCallLamb]() {
		if (IsValid(DeathCallLamb))
		{
			DeathCallLamb->PlayFModEventSound(DeathCallLamb->FMOD_BalloonBoom);

			if (DeathCallLamb->BalloonBoomParticles)
			{
				UGameplayStatics::SpawnEmitterAtLocation(DeathCallLamb->GetWorld(),
					DeathCallLamb->BalloonBoomParticles,
					DeathCallLamb->GetActorLocation() + DeathCallLamb->DieBallonVector, FRotator(0.f), true);
			}
		}
		});
	GetWorldTimerManager().SetTimer(DeathTimer, DeathCallback, DeathTimer_Time, false);
}