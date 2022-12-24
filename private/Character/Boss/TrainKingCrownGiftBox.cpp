// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Boss/TrainKingCrownGiftBox.h"
#include "DrawDebugHelpers.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Character/Boss/TrainKingCrown.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Character/PlayerCharacter/PlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Curves/CurveVector.h"
#include "Particles/ParticleSystemComponent.h"
#include "Item/Sword.h"


// Sets default values
ATrainKingCrownGiftBox::ATrainKingCrownGiftBox():
	bAnimStart(false),
	AnimCount(0),
	MaxGiftHp(100.f),
	GiftHp(100.f),
	CurveTimer(0.f),
	ArriveEnd(false),
	bFireSelected(false)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GiftBoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("GiftBoxCollsiion"));
	SetRootComponent(GiftBoxCollision);

	GiftMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GiftMesh"));
	GiftMesh->SetupAttachment(GiftBoxCollision);

	BalloonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BalloonMesh"));
	BalloonMesh->SetupAttachment(GiftBoxCollision);

	TrailParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("TrailParticle"));
	TrailParticle->SetupAttachment(GiftMesh);

	HealthBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidget"));
	HealthBarWidget->SetupAttachment(GetRootComponent());

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->bRotationRemainsVertical = true;
	ProjectileMovementComponent->bShouldBounce = false;
	ProjectileMovementComponent->ProjectileGravityScale = 0.f;
	ProjectileMovementComponent->MaxSpeed = 10000.f;
	ProjectileMovementComponent->Velocity = FVector(0.f, 0.f, 0.f);


}

// Called when the game starts or when spawned
void ATrainKingCrownGiftBox::BeginPlay()
{
	Super::BeginPlay();
	FindKingCrown();
	FindPlayer();
	GiftBoxCollision->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::GiftBoxOverlap);

	GiftBoxCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GiftBoxCollision->SetCollisionObjectType(ECollisionChannel::ECC_Pawn);
	GiftBoxCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);
	GiftBoxCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	GiftBoxCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel5, ECollisionResponse::ECR_Ignore);

	AttachToActor(KingCrown, FAttachmentTransformRules::KeepWorldTransform);

	SocketControlAnchor.Add(KingCrown->GetMesh()->GetSocketLocation(FName("GiftFireAnchorSocket_1")));
	SocketControlAnchor.Add(KingCrown->GetMesh()->GetSocketLocation(FName("GiftFireAnchorSocket_2")));
	SocketControlAnchor.Add(KingCrown->GetMesh()->GetSocketLocation(FName("GiftFireAnchorSocket_3")));
	
	AnchorNumber = FMath::FRandRange(0, 2);

	bFireSelected = KingCrown->GetTrainFireSelected();

	Start = KingCrown->GetMesh()->GetSocketLocation(FName("GiftFireSocket"));

	ControlAnchor = SocketControlAnchor[AnchorNumber];

	EndIndex = FMath::RandRange(KingCrown->GetTrainGiftStartNumber(), KingCrown->GetTrainGiftEndNumber());
	
	End = KingCrown->GetBoxPoint(EndIndex);

	if (!CurveFloat)
	{
		return;
	}

	FOnTimelineFloat TimelineProgress;
	TimelineProgress.BindUFunction(this, FName("TimelineProgress"));
	CurveFTimeline.AddInterpFloat(CurveFloat, TimelineProgress);

	FOnTimelineEvent TimelineEndProgress;
	TimelineEndProgress.BindUFunction(this, FName("EndTimelineProgress"));
	CurveFTimeline.SetTimelineFinishedFunc(TimelineEndProgress);


	CurveFTimeline.SetTimelineLengthMode(TL_LastKeyFrame);

	if (bFireSelected)
	{
		CurveFTimeline.PlayFromStart();
		if (TrailParticle)
			TrailParticle->Activate();
	}

	GiftHp = MaxGiftHp;

	StartToEndPosOffset = Start - End;
	StartZOffset = StartToEndPosOffset.Z;
	StartToEndPosOffset.Z = 0.f;

	if (Train) TrainStartYaw = Train->GetActorRotation().Yaw;
	
}

void ATrainKingCrownGiftBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	EndTemp = KingCrown->GetBoxPoint(EndIndex);

	if (bFireSelected)
	{
		if (CurveFTimeline.IsPlaying())
		{
			CurveFTimeline.TickTimeline(DeltaTime);
		}
	}
	else
	{
		CurveTimer += DeltaTime;
		if (CurveTimer <= 2.5f && Train)
		{
			FRotator TrainRot = Train->GetActorRotation();

			const float cos = FMath::Cos(FMath::DegreesToRadians(TrainRot.Yaw - TrainStartYaw));
			const float sin = FMath::Sin(FMath::DegreesToRadians(TrainRot.Yaw - TrainStartYaw));

			const auto CurveValue = CurveVector->GetVectorValue(CurveTimer);

			FVector DeltaOffset = StartToEndPosOffset * CurveValue;
			
			DeltaOffset.Z = CurveValue.Z + (StartZOffset * CurveValue.X);

			float OffX = DeltaOffset.X;
			float OffY = DeltaOffset.Y;
			DeltaOffset.X = (OffX * cos - OffY * sin);
			DeltaOffset.Y = (OffX * sin + OffY * cos);

			SetActorLocation(EndTemp + DeltaOffset);
		}
		else if (!ArriveEnd)
		{
			ArriveEnd = true;
			EndTimelineProgress();
			ProjectileMovementComponent->Velocity.Z = -150.f;
		}
	}
	
	//Spawn Anim
	if (bAnimStart)
	{
		if (AnimCount >= 2)
			SpawnStart();
		if (GetVelocity().Z == 0)
			AnimCount++;
	}

}

void ATrainKingCrownGiftBox::TimelineProgress(float Value)
{	
	Start = KingCrown->GetMesh()->GetSocketLocation(FName("GiftFireSocket"));
	ControlAnchor = SocketControlAnchor[AnchorNumber];

	MyLocation = GetLocationBezier(Start, ControlAnchor, EndTemp, Value);

	FRotator MyRotation = GetActorRotation();
	MyRotation.Pitch = 0.f;
	MyRotation.Yaw = 0.f;
	MyRotation.Roll = 0.f;

	SetActorLocationAndRotation(MyLocation, MyRotation);

}

void ATrainKingCrownGiftBox::EndTimelineProgress()
{
	ProjectileMovementComponent->ProjectileGravityScale = 1.f;
	bAnimStart = true;
	DestroyBallonMesh();
	GiftBoxCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	
}

FVector ATrainKingCrownGiftBox::GetLocationBezier(FVector StartPoint, FVector Anchor, FVector EndPoint, float Time)
{
	FVector TempA = FMath::Lerp(StartPoint, Anchor, Time);
	FVector TempB = FMath::Lerp(Anchor, EndPoint, Time);

	FVector Result = FMath::Lerp(TempA, TempB, Time);


	return Result;
}

void ATrainKingCrownGiftBox::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (ZeroMatInstance)
	{
		DynamicZeroMat = UMaterialInstanceDynamic::Create(ZeroMatInstance, this);
		GiftMesh->SetMaterial(0, DynamicZeroMat);
	}

	if (OneMatInstance)
	{
		DynamicOneMat = UMaterialInstanceDynamic::Create(OneMatInstance, this);
		BalloonMesh->SetMaterial(0, DynamicOneMat);
	}
}

void ATrainKingCrownGiftBox::DestroyBallonMesh()
{
	if (DynamicOneMat)
	{
		DynamicOneMat->SetScalarParameterValue(TEXT("Opacity"), 0);
	}

	if (BoomParticle)
	{
		UGameplayStatics::SpawnEmitterAttached(
			BoomParticle,
			GiftMesh,
			TEXT("None"),
			GetActorLocation() + FVector(BallonVector.X, BallonVector.Y, BallonVector.Z),
			GetActorRotation(),
			EAttachLocation::KeepWorldPosition);

		UFMODBlueprintStatics::PlayEventAtLocation(GetWorld(), FMOD_Balloon, GetActorTransform(), true);
	}
	if (TrailParticle)
		TrailParticle->DeactivateSystem();
}

void ATrainKingCrownGiftBox::SpawnMonster()
{
	GiftBoxCollision->OnComponentBeginOverlap.RemoveAll(this);

	KingCrown->CurrentPricePlus(1);
	Destroy();
	auto actor =  RandomSpawn();
	if (IsValid(actor))
	{
		FScriptDelegate del;
		del.BindUFunction(KingCrown, FName("ChildCurrentPricePlus"));
		actor->OnEndPlay.Add(del);

		AEnemy* SpawnEnemy = CastChecked<AEnemy>(actor);

		SpawnEnemy->GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
		SpawnEnemy->GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel5, ECollisionResponse::ECR_Ignore);
		SpawnEnemy->GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
		SpawnEnemy->GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel5, ECollisionResponse::ECR_Ignore);
	}
	else
	{
		KingCrown->CurrentPricePlus(1);
		UE_LOG(LogTemp, Warning, TEXT("CurrentpricePlus NullValue "));
	}
}

void ATrainKingCrownGiftBox::GiftBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto Sword = Cast<ASword>(OtherActor);

	if (Sword)
	{
		if (Player)
		{
			if (GiftHp > 0.f)
			{
				auto Location = Sword->GetHitParticleLocation(GetActorLocation(), GiftBoxCollision->GetScaledBoxExtent().Size() / 2);

				if (PS_CharSlash)
				{
					UGameplayStatics::SpawnEmitterAttached(PS_CharSlash,GiftMesh,
						TEXT("None"),
						GetActorLocation(),
						GetActorRotation(),
						EAttachLocation::KeepWorldPosition);

				}

				GiftHp -= Player->GetCurrentDamage();

				if(FMOD_GiftHit)
					UFMODBlueprintStatics::PlayEvent2D(GetWorld(), FMOD_GiftHit, true);
				

				DynamicZeroMat->SetScalarParameterValue(FName("DamageColorSwitch"), 1.0f);

				auto OffDamageLamb = this;

				FTimerDelegate OffDamageColor;
				OffDamageColor.BindLambda([OffDamageLamb]() {
					if (IsValid(OffDamageLamb))
					{
						OffDamageLamb->DynamicZeroMat->SetScalarParameterValue(FName("DamageColorSwitch"), 0.f);

					}
					});

				GetWorldTimerManager().SetTimer(OffDamageTimerHandle, OffDamageColor, 0.2f, false);

				if (GiftHp <= 0.f)
				{
					GiftHp = 0.f;
					
					SetActorEnableCollision(false);

					UAnimInstance* AnimInstance = GiftMesh->GetAnimInstance();
					if (AnimInstance)
						if (AnimInstance->Montage_IsPlaying(MoveMontage))
							AnimInstance->Montage_Stop(0.f, MoveMontage);

					HealthBarWidget->GetUserWidgetObject()->SetVisibility(ESlateVisibility::Hidden);
					HealthBarWidget->Deactivate();

					ProjectileMovementComponent->SetUpdatedComponent(GiftBoxCollision);
					ProjectileMovementComponent->ProjectileGravityScale = 0.f;

					FVector FlyingDir = GetActorLocation() - OtherActor->GetActorLocation();
					FlyingDir.Normalize();
					FlyingDir.Z = 0.7f;

					ProjectileMovementComponent->Velocity = FlyingDir * 3000.f;
					ProjectileMovementComponent->UpdateComponentVelocity();

					auto DeleteGiftLamb = this;

					FTimerDelegate DeleteGiftDelegate;
					DeleteGiftDelegate.BindLambda([DeleteGiftLamb]() {
						if (IsValid(DeleteGiftLamb))
						{
							const USkeletalMeshSocket* DestorySocket = DeleteGiftLamb->GiftMesh->GetSocketByName(FName("ParticleSocket"));
							const FTransform DestoryTransform = DestorySocket->GetSocketTransform(DeleteGiftLamb->GiftMesh);

							if (DeleteGiftLamb->DeathParticle)
							{
								UFMODBlueprintStatics::PlayEvent2D(DeleteGiftLamb->GetWorld(), DeleteGiftLamb->FMOD_Death, true);

								UGameplayStatics::SpawnEmitterAttached(DeleteGiftLamb->DeathParticle, DeleteGiftLamb->GiftMesh,
									TEXT("None"),
									DeleteGiftLamb->GetActorLocation(),
									DeleteGiftLamb->GetActorRotation(),
									EAttachLocation::KeepWorldPosition);

							}
							DeleteGiftLamb->GiftBoxCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
							DeleteGiftLamb->GiftBoxCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);
							DeleteGiftLamb->GiftBoxCollision->OnComponentBeginOverlap.RemoveAll(DeleteGiftLamb);
							DeleteGiftLamb->Destroy();
							DeleteGiftLamb->KingCrown->CurrentPricePlus(2);
						}

						});
					GetWorldTimerManager().SetTimer(DestoryTimerHandle, DeleteGiftDelegate, 1.f, false);
				}
				else
				{
					UFMODBlueprintStatics::PlayEventAtLocation(GetWorld(), FMOD_Back, GetActorTransform(), true);
					HealthBarWidget->GetUserWidgetObject()->SetVisibility(ESlateVisibility::Visible);
				}
			}
		}
	}
}

void ATrainKingCrownGiftBox::FindKingCrown()
{
	class UWorld* const world = GetWorld();
	if (world)
		for (TObjectIterator<ATrainKingCrown> Itr; Itr; ++Itr)
			if (Itr->IsA(ATrainKingCrown::StaticClass()))
				KingCrown = *Itr;
}

void ATrainKingCrownGiftBox::FindPlayer()
{
	if (GetWorld())
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

void ATrainKingCrownGiftBox::ActorDestroyPlus(AActor* actor, const EEndPlayReason::Type EndPlayReason)
{
	KingCrown->CurrentPricePlus(1);
}

void ATrainKingCrownGiftBox::SpawnStart()
{
	GiftBoxCollision->SetSimulatePhysics(false);
	bAnimStart = false;
	GiftBoxCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GiftBoxCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	UAnimInstance* AnimInstance = GiftMesh->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_Play(MoveMontage);
	}
}
