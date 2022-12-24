#include "Character/Boss/KingCrownPunch.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Character/PlayerCharacter/PlayerCharacter.h"
#include "Engine/StaticMeshActor.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Character/Boss/OnePhaseKingCrown.h"
#include "Character/Boss/KingCrown.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Item/Sword.h"

// Sets default values
AKingCrownPunch::AKingCrownPunch() :
	PunchState(EPunchState::EPS_IDLE),
	Target(NULL),
	bNoTarget(false),
	bhasFinishedDelay(false),
	bSpline(false),
	PunchSplineValue(0),
	PunchDamage(100.f),
	playerTime(1.0f),
	GroundTime(1.0f),
	SplineTime(1.0f)
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComp"));
	RootComponent = CollisionComp;

	PunchMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PunchMesh"));
	PunchMesh->SetupAttachment(RootComponent);

	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineMesh"));
	SplineComponent->SetupAttachment(GetRootComponent());
	SplineComponent->SetEnableGravity(false);

	PS_WindParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("PS_WindParticle"));
	PS_WindParticle->SetupAttachment(PunchMesh, FName("WindParticle"));

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 1000.f;
	ProjectileMovement->MaxSpeed = 1000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bInitialVelocityInLocalSpace = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->bIsHomingProjectile = false;
	ProjectileMovement->HomingAccelerationMagnitude = 0.f;
	ProjectileMovement->ProjectileGravityScale = 0.f;
	ProjectileMovement->Velocity = FVector(0.f, 0.f, 0.f);
}

void AKingCrownPunch::BeginPlay() 
{
	Super::BeginPlay();

	FindPlayer();
	FindKingCrown();

	if (MovementCurve)
	{
		FOnTimelineFloat MovementLineFunction;
		MovementLineFunction.BindUFunction(this, FName("MovementLine"));
		MovementTimeline.AddInterpFloat(MovementCurve, MovementLineFunction);

		MovementTimeline.SetTimelineLengthMode(TL_LastKeyFrame);
	}

	CollisionComp->OnComponentBeginOverlap.AddUniqueDynamic(this, &AKingCrownPunch::CompBeginOverlap);

	CollisionComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Overlap);
	CollisionComp->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);

	PunchMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	CollisionComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	ProjectileMovement->Velocity = GetActorForwardVector()*100;

	if (kingCrown->GetPS_PunchDoor())
	{
		kingCrown->GetPS_PunchDoor()->Deactivate();
	}
	PS_WindParticle->Activate();
	UFMODBlueprintStatics::PlayEvent2D(GetWorld(), FMOD_Start, true);
}

void AKingCrownPunch::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MovementTimeline.IsPlaying())
	{
		MovementTimeline.TickTimeline(DeltaTime);	
	}

	switch (PunchState)
	{
	case EPunchState::EPS_IDLE:
		if (!bhasFinishedDelay)
		{
			DelayLogic(DeltaTime);
		}
		break;
	case EPunchState::EPS_ATTACK:

		kingCrown->GetPS_PunchFire()->Activate();
		if (kingCrown != NULL)
		{
			if (kingCrown->IsValidLowLevel())
			{
				FVector CrownSocketLoc = kingCrown->GetMesh()->GetSocketLocation(FName("PunchSocket"));

				if (!bSpline)
				{
					bSpline = true;

					SplineComponent->AddSplinePoint(FVector(CrownSocketLoc.X, CrownSocketLoc.Y, CrownSocketLoc.Z), ESplineCoordinateSpace::World);
			
					SplineMeshComponent = NewObject<USplineMeshComponent>(this, USplineMeshComponent::StaticClass());
				
					SplineMeshComponent->RegisterComponentWithWorld(GetWorld());
					
					SplineMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
					const FVector StartPoint = SplineComponent->GetLocationAtSplinePoint(PunchSplineValue, ESplineCoordinateSpace::Local);
					const FVector StartTangent = SplineComponent->GetTangentAtSplinePoint(PunchSplineValue, ESplineCoordinateSpace::Local);
					const FVector EndPoint = SplineComponent->GetLocationAtSplinePoint(PunchSplineValue + 1, ESplineCoordinateSpace::Local);
					const FVector EndTangent = SplineComponent->GetTangentAtSplinePoint(PunchSplineValue + 1, ESplineCoordinateSpace::Local);

					MySpline.Add(SplineMeshComponent);
					MySpline[PunchSplineValue]->SetMobility(EComponentMobility::Movable);

					MySpline[PunchSplineValue]->AttachToComponent(SplineComponent, FAttachmentTransformRules::KeepRelativeTransform);
					
					MySpline[PunchSplineValue]->SetStartAndEnd(StartPoint, StartTangent, EndPoint, EndTangent, true);
					
					if (PunchSplineValue % 2 == 0)
					{
						MySpline[PunchSplineValue]->SetStaticMesh(Mesh);

					}
					MySpline[PunchSplineValue]->SetForwardAxis(ESplineMeshAxis::Y);
				

					auto SplineCallbackLam = this;

					FTimerDelegate SplineCallback;
					SplineCallback.BindLambda([SplineCallbackLam]()
						{
							if (IsValid(SplineCallbackLam))
							{
								SplineCallbackLam->bSpline = false;
								++SplineCallbackLam->PunchSplineValue;
							}
						});

					GetWorldTimerManager().SetTimer(SplineHandle, SplineCallback, SplineTime, false);
				}
				
			}
		}

		if (!bhasFinishedDelay)
		{
			DelayLogic(DeltaTime);
		}
		else
		{
			if (Player != NULL)
			{
				if (Player->IsValidLowLevel())
				{
					FVector Dir = ((Target->GetActorLocation() - FVector(0.f, 0.f, 50.f)) - GetActorLocation()).GetSafeNormal();
					Dir += Target->GetVelocity() * Dir.Size() / 15000.f;
					ProjectileMovement->Velocity += Dir * 6000.f * DeltaTime;
					ProjectileMovement->Velocity = ProjectileMovement->Velocity.GetSafeNormal() * 6000.f;
				}
			}
		}
		break;
	case EPunchState::EPS_PARRING:
		if (!bSpline)
		{
			bSpline = true;

			kingCrown->GetPS_PunchFire()->Activate();
			SplineComponent->AddSplinePoint(FVector(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z), ESplineCoordinateSpace::World);

			SplineMeshComponent = NewObject<USplineMeshComponent>(this, USplineMeshComponent::StaticClass());
			SplineMeshComponent->RegisterComponentWithWorld(GetWorld());
			SplineMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			const FVector StartPoint = SplineComponent->GetLocationAtSplinePoint(PunchSplineValue, ESplineCoordinateSpace::Local);
			const FVector StartTangent = SplineComponent->GetTangentAtSplinePoint(PunchSplineValue, ESplineCoordinateSpace::Local);
			const FVector EndPoint = SplineComponent->GetLocationAtSplinePoint(PunchSplineValue+1 , ESplineCoordinateSpace::Local);
			const FVector EndTangent = SplineComponent->GetTangentAtSplinePoint(PunchSplineValue+1, ESplineCoordinateSpace::Local);

			MySpline.Add(SplineMeshComponent);
			MySpline[PunchSplineValue]->SetMobility(EComponentMobility::Movable);
			MySpline[PunchSplineValue]->AttachToComponent(SplineComponent, FAttachmentTransformRules::KeepRelativeTransform);
			MySpline[PunchSplineValue]->SetStartAndEnd(StartPoint, StartTangent, EndPoint, EndTangent, true);

			if (SplineParringValue >= 2 &&PunchSplineValue %2==0)
			{
				MySpline[PunchSplineValue]->SetStaticMesh(Mesh);
			}
			else if (SplineParringValue == 3)
			{
				CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			}
			MySpline[PunchSplineValue]->SetForwardAxis(ESplineMeshAxis::Y);


			auto SplineCallbackLam = this;

			FTimerDelegate SplineCallback;

			SplineCallback.BindLambda([SplineCallbackLam]()
				{
					if (IsValid(SplineCallbackLam))
					{
						SplineCallbackLam->bSpline = false;
						++SplineCallbackLam->PunchSplineValue;
						++SplineCallbackLam->SplineParringValue;
					}
				});
			GetWorldTimerManager().SetTimer(SplineHandle, SplineCallback, SplineTime, false);
		}
		if (kingCrown != NULL)
		{
			if (kingCrown->IsValidLowLevel())
			{
				FVector Dir = ((Target->GetActorLocation()) - GetActorLocation()).GetSafeNormal();
				Dir += Target->GetVelocity() * Dir.Size() / 15000.f;
				ProjectileMovement->Velocity += Dir * 6000.f * DeltaTime;
				ProjectileMovement->Velocity = ProjectileMovement->Velocity.GetSafeNormal() * 6000.f;
			}
		}
		break;
	}
}


void AKingCrownPunch::UpdateTarget(EPunchState State)
{

	switch (State)
	{
	case EPunchState::EPS_IDLE:
		if (Player != NULL)
		{
			if (Player->IsValidLowLevel())
			{
				Target = Player;
				bNoTarget = false;
			}
		}
		else
		{
			Target = nullptr;
			bNoTarget = true;
		}
		SetPunchState(EPunchState::EPS_ATTACK);
		break;
	}
}

void AKingCrownPunch::DelayLogic(float DeltaTime)
{
	if (!bhasFinishedDelay)
	{
		DelayTimer += 1.f * DeltaTime;
		if (DelayTimer > 0.1f)
		{
			switch (PunchState)
			{
			case EPunchState::EPS_IDLE:
				UpdateTarget(EPunchState::EPS_IDLE);
				this->SetActorEnableCollision(true);
				bhasFinishedDelay = true;
				break;
			}
		}
	}
}

void AKingCrownPunch::CompBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	class APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor);
	class AStaticMeshActor* GroundActor = Cast<AStaticMeshActor>(OtherActor);
	class AKingCrown* Crown = Cast<AKingCrown>(OtherActor);

	PS_WindParticle->Deactivate();

	if (PlayerCharacter)
	{

		float PlayerDamage = UGameplayStatics::ApplyDamage(PlayerCharacter, PunchDamage, PlayerCharacter->GetInstigatorController(), this, UDamageType::StaticClass());

		//패링 변환
		if (PlayerDamage <= 0 && PlayerCharacter->GetCurrentState() == EPlayerState::EPS_PARRYING)
		{
			Target = kingCrown;

			SplineComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
			ProjectileMovement->Velocity = (GetActorUpVector() * 50.f ) + (GetActorRightVector() *-50.f);
			kingCrown->GetKingCrownMainCollision()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			SetPunchState(EPunchState::EPS_PARRING);
			FTransform PunchTransform = PunchMesh->GetSocketTransform(FName("WindParticle"));

			//if (PS_SuccessParring)
			//{
			//	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PS_SuccessParring, PunchTransform);
			//}
			PlayerCharacter->GetSword()->PPVBlur();
			UFMODBlueprintStatics::PlayEventAtLocation(GetWorld(), FMOD_PlayerParrying, GetActorTransform(), true);
		}
		//되돌아가기
		else
		{
			UFMODBlueprintStatics::PlayEventAtLocation(GetWorld(), FMOD_PlayerHit, GetActorTransform(), true);

			SetPunchState(EPunchState::EPS_HIT);
			ProjectileMovement->Velocity = FVector(0.f);
			CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			auto PlayerTimerLamb = this;

			FTimerDelegate PlayerTimerCallback;
			PlayerTimerCallback.BindLambda([PlayerTimerLamb]()
				{
					if (IsValid(PlayerTimerLamb))
					{
						PlayerTimerLamb->CollisionComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
						PlayerTimerLamb->CollisionComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel5, ECollisionResponse::ECR_Ignore);
						PlayerTimerLamb->PunchMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
						PlayerTimerLamb->PunchMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel5, ECollisionResponse::ECR_Ignore);

						PlayerTimerLamb->SetPunchState(EPunchState::EPS_Reverse);
						PlayerTimerLamb->bhasFinishedDelay = false;
						PlayerTimerLamb->CurveTimerStart();
					}
				});
			GetWorldTimerManager().SetTimer(PlayerHandle, PlayerTimerCallback, playerTime, false);
			SplineComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

			if (MySpline.IsValidIndex(0))
			{
				MySpline[0]->DestroyComponent();
				MySpline.RemoveAt(0);
			}
			
			kingCrown->GetPS_PunchFire()->Deactivate();

			FTransform PunchTransform = PunchMesh->GetSocketTransform(FName("WindParticle"));
			if (PS_PunchHit)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PS_PunchHit, PunchTransform);
			}

#pragma region AttackCameraReact
			CustomTimeDilation = 0.f;
			PlayerCharacter->CustomTimeDilation = 0.f;

			auto HitStopLamb = this;


			FTimerDelegate HitStopCallback;
			HitStopCallback.BindLambda([HitStopLamb, PlayerCharacter]() {
				if (IsValid(HitStopLamb))
				{
					HitStopLamb->CustomTimeDilation = 1.f;
					PlayerCharacter->CustomTimeDilation = 1.0f;
				}

				});
			GetWorldTimerManager().SetTimer(HitStopTimer, HitStopCallback, 0.2f, false);
#pragma endregion
		}
	}
	//땅바닥
	else if (GroundActor)
	{
		UFMODBlueprintStatics::PlayEventAtLocation(GetWorld(), FMOD_GroundHit, GetActorTransform(), true);

		//kingCrown->ClearFoucsSetting();
		SetPunchState(EPunchState::EPS_NOHIT);
		ProjectileMovement->Velocity = FVector(0.f);

		CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		auto GroundActorLamb = this;

		FTimerDelegate GroundActorCallback;
		GroundActorCallback.BindLambda([GroundActorLamb]()
			{
				if (IsValid(GroundActorLamb))
				{
					GroundActorLamb->CollisionComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
					GroundActorLamb->CollisionComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel5, ECollisionResponse::ECR_Ignore);
					GroundActorLamb->PunchMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
					GroundActorLamb->PunchMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel5, ECollisionResponse::ECR_Ignore);

					GroundActorLamb->SetPunchState(EPunchState::EPS_Reverse);
					GroundActorLamb->bhasFinishedDelay = false;
					GroundActorLamb->CurveTimerStart();
				}

			});
		GetWorldTimerManager().SetTimer(GroundHandle, GroundActorCallback, GroundTime, false);
		SplineComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

		if (MySpline.IsValidIndex(0))
		{
			MySpline[0]->DestroyComponent();
			MySpline.RemoveAt(0);
		}
		kingCrown->GetPS_PunchFire()->Deactivate();
	}
	//패링 맞을때
	else if (Crown && PunchState ==EPunchState::EPS_PARRING)
	{
		SetPunchState(EPunchState::EPS_DESTROY);
		//가드 포인트
		if (MySpline.IsValidIndex(SplineComponent->GetNumberOfSplinePoints()-1))
		{
			MySpline[SplineComponent->GetNumberOfSplinePoints()-1]->DestroyComponent();
			MySpline.RemoveAt(SplineComponent->GetNumberOfSplinePoints()-1);
		}
		SplineComponent->RemoveSplinePoint(SplineComponent->GetNumberOfSplinePoints() );
		SplineComponent->RemoveSplinePoint(SplineComponent->GetNumberOfSplinePoints() - 1);

		CollisionComp->DestroyComponent();
		CurveTimerStart();

		kingCrown->GetPS_PunchFire()->Activate();

		FTransform PunchTransform = PunchMesh->GetSocketTransform(FName("WindParticle"));

		if (PS_PunchHitBoss)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PS_PunchHitBoss, PunchTransform);
		}
		kingCrown->GroggyPatternStart_120();
		kingCrown->SetGP(0);
		kingCrown->PunchParringChangeMat();
		UFMODBlueprintStatics::PlayEvent2D(GetWorld(), FMOD_KingCrownHit,  true);
	}

	if (PlayerCharacter || GroundActor || (Crown && PunchState == EPunchState::EPS_PARRING))
	{
		if(AttackCameraShake)
			GetWorld()->GetFirstPlayerController()->PlayerCameraManager->StartMatineeCameraShake(AttackCameraShake);
	}
}

void AKingCrownPunch::MovementLine(float Value)
{	

	kingCrown->GetPS_PunchFire()->Activate();
	const float SplineLength = SplineComponent->GetSplineLength();

	FVector CurrentSplineLoc = SplineComponent->GetLocationAtDistanceAlongSpline(Value * SplineLength, ESplineCoordinateSpace::World);

	FRotator KingRoc = kingCrown->GetActorRotation();

	SetActorRotation(KingRoc);
	this->SetActorLocation(CurrentSplineLoc);

	int removeScale = Value* MySpline.Num();

	if (removeScale < MySpline.Num())
	{
		for (int i = 0; i <= removeScale; ++i)
		{
			if (MySpline[i] != nullptr)
			{
				if (i % 5 == 0)
				{
					UFMODBlueprintStatics::PlayEvent2D(GetWorld(), FMOD_Broken, true);
				}

				if (PS_PunchBroken && i%2== 0)
				{
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PS_PunchBroken, CurrentSplineLoc);
				}
				MySpline[i]->DestroyComponent();
				MySpline[i] = nullptr;
			}
		}
	}

	if (removeScale == SplineComponent->GetNumberOfSplinePoints()-2)
	{
		SetPunchState(EPunchState::EPS_MAX);
		MySpline.Empty();
		kingCrown->PunchPatternEnd();
		kingCrown->GetPS_PunchFire()->Deactivate();
		Destroy();
	}
}

void AKingCrownPunch::CurveTimerStart()
{	
	MovementTimeline.PlayFromStart();
}

#pragma region FindActor

void AKingCrownPunch::FindPlayer()
{
	class UWorld* const world = GetWorld();
	if (world)
		for (TObjectIterator<APlayerCharacter> Itr; Itr; ++Itr)
			if (Itr->IsA(APlayerCharacter::StaticClass()))
				Player = *Itr;
}

void AKingCrownPunch::FindKingCrown()
{
	class UWorld* const world = GetWorld();
	if (world)
		for (TObjectIterator<AKingCrown> Itr; Itr; ++Itr)
			if (Itr->IsA(AKingCrown::StaticClass()))
				kingCrown = *Itr;
}
#pragma endregion