// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Boss/KingCrownGiftBox.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/SkeletalMeshComponent.h"
#include "Character/PlayerCharacter/PlayerCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Character/Boss/KingCrown.h"
#include "Components/WidgetComponent.h"
#include "../../../Public/Character/PlayerCharacter/PlayerCharacter.h"
#include "Item/Sword.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMeshActor.h"

// Sets default values
AKingCrownGiftBox::AKingCrownGiftBox() :
	bLaunchStart(false),
	bAnimStart(false),
	bCurveStart(false),
	LaunchTime(1.0f),
	MinPower(1500.f),
	MaxPower(1500.f),
	MaxHealth(100.f),
	Health(100.f),
	GiftDamage(50.f),
	RandomPower(0.f)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GiftBoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("GiftBoxCollsiion"));
	SetRootComponent(GiftBoxCollision);

	GiftMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GiftMesh"));
	GiftMesh->SetupAttachment(GiftBoxCollision);

	TrailParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("TrailParticle"));
	TrailParticle->SetupAttachment(GiftMesh);
	
	HealthBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidget"));
	HealthBarWidget->SetupAttachment(GetRootComponent());

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->bRotationRemainsVertical = true;
}

// Called when the game starts or when spawned
void AKingCrownGiftBox::BeginPlay()
{
	Super::BeginPlay();

	FindPlayer();

	GiftBoxCollision->OnComponentBeginOverlap.AddUniqueDynamic(this, &AKingCrownGiftBox::GiftBoxOverlap);
	
	if (GiftMatCurve)
	{
		FOnTimelineFloat GiftFunction;
		GiftFunction.BindUFunction(this, FName("GiftLine"));
		GiftTimeline.AddInterpFloat(GiftMatCurve, GiftFunction);
		GiftTimeline.SetTimelineLengthMode(TL_LastKeyFrame);
	}

	HealthBarWidget->GetUserWidgetObject()->SetVisibility(ESlateVisibility::Hidden);

	ProjectileMovementComponent->Velocity = GetActorForwardVector() * RandomPower;

	GiftBoxCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GiftBoxCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);
	GiftBoxCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	GiftBoxCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel5, ECollisionResponse::ECR_Ignore);


	auto KingCrownGiftBoxPlay = this;


	FTimerDelegate VelocityCallback;
	VelocityCallback.BindLambda([KingCrownGiftBoxPlay]()
		{
			if (IsValid(KingCrownGiftBoxPlay))
			{
				KingCrownGiftBoxPlay->bLaunchStart = true;

				KingCrownGiftBoxPlay->GiftBoxCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
				KingCrownGiftBoxPlay->GiftBoxCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
			}
		});
	GetWorldTimerManager().SetTimer(LaunchTimer, VelocityCallback, LaunchTime, false);

}

// Called every frame
void AKingCrownGiftBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetVelocity().Size() != 0)
	{
		UAnimInstance* AnimInstance = GiftMesh->GetAnimInstance();
		if (AnimInstance && AnimInstance->Montage_IsPlaying(MoveMontage))
		{
			AnimInstance->Montage_Stop(0.f, MoveMontage);
			bAnimStart = false;
		}
	}

	if (bLaunchStart && !bAnimStart)
	{
		if (GetVelocity().Size()==0)
		{
			SpawnStart();
		}
	}

	if (GiftTimeline.IsPlaying())
	{
		GiftTimeline.TickTimeline(DeltaTime);
	}
}

void AKingCrownGiftBox::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	RandomPower = FMath::FRandRange(MinPower, MaxPower);

	ProjectileMovementComponent->InitialSpeed = RandomPower;

	if (ZeroMatInstance)
	{
		DynamicZeroMat = UMaterialInstanceDynamic::Create(ZeroMatInstance, this);
		GiftMesh->SetMaterial(0, DynamicZeroMat);
	}
}

void AKingCrownGiftBox::SpawnStart()
{
	GiftBoxCollision->SetSimulatePhysics(false);
	bAnimStart = true;
	UAnimInstance* AnimInstance = GiftMesh->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_Play(MoveMontage);
	}
}

void AKingCrownGiftBox::SpawnMonster()
{
	const USkeletalMeshSocket* DestorySocket = GiftMesh->GetSocketByName(FName("ParticleSocket"));
	const FTransform DestoryTransform = DestorySocket->GetSocketTransform(GiftMesh);

	if (DestroyParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DestroyParticle, DestoryTransform);
	}
	GiftBoxCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GiftBoxCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);
	GiftBoxCollision->OnComponentBeginOverlap.RemoveAll(this);
	Destroy();
	RandomSpawn();
}

void AKingCrownGiftBox::CurveStart()
{
	GiftTimeline.PlayFromStart();
	bCurveStart = true;
}

void AKingCrownGiftBox::GiftLine(float Value)
{
	DynamicZeroMat->SetScalarParameterValue(FName("TikTokColorSwitch"), Value);
}

void AKingCrownGiftBox::GiftBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto Sword = Cast<ASword>(OtherActor);

	if (Sword)
	{
		if (Player)
		{
			if (Health > 0.f)
			{
				auto Location = Sword->GetHitParticleLocation(GetActorLocation(), GiftBoxCollision->GetScaledBoxExtent().Size()/2);

				if (PS_CharSlash)
				{
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PS_CharSlash, Location);
					UFMODBlueprintStatics::PlayEvent2D(GetWorld(), FMOD_Back, true);
				}

				Health -= Player->GetCurrentDamage();

				

				DynamicZeroMat->SetScalarParameterValue(FName("DamageColorSwitch"), 1.0f);

				auto KingCrownGiftBox = this;


				FTimerDelegate OffDamageColor;
				OffDamageColor.BindLambda([KingCrownGiftBox]() {
					if (IsValid(KingCrownGiftBox))
					{
						KingCrownGiftBox->DynamicZeroMat->SetScalarParameterValue(FName("DamageColorSwitch"), 0.f);
					}
					});

				GetWorldTimerManager().SetTimer(OffDamageTimerHandle, OffDamageColor, 0.2f, false);

				if (Health <= 0.f)
				{
					Health = 0.f;

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
					
					auto KingCrownGiftBoxDelete = this;


					FTimerDelegate DeleteDelegate;
					DeleteDelegate.BindLambda([KingCrownGiftBoxDelete]() {
						if (IsValid(KingCrownGiftBoxDelete))
						{
							const USkeletalMeshSocket* DestorySocket = KingCrownGiftBoxDelete->GiftMesh->GetSocketByName(FName("ParticleSocket"));
							const FTransform DestoryTransform = DestorySocket->GetSocketTransform(KingCrownGiftBoxDelete->GiftMesh);

							if (KingCrownGiftBoxDelete->DeathParticle)
							{
								UFMODBlueprintStatics::PlayEvent2D(KingCrownGiftBoxDelete->GetWorld(), KingCrownGiftBoxDelete->FMOD_Death, true);
								UGameplayStatics::SpawnEmitterAtLocation(KingCrownGiftBoxDelete->GetWorld(), KingCrownGiftBoxDelete->DeathParticle, DestoryTransform);
							}
							KingCrownGiftBoxDelete->GiftBoxCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
							KingCrownGiftBoxDelete->GiftBoxCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);
							KingCrownGiftBoxDelete->GiftBoxCollision->OnComponentBeginOverlap.RemoveAll(KingCrownGiftBoxDelete);
							KingCrownGiftBoxDelete->Destroy();
						}
						});
					GetWorldTimerManager().SetTimer(DestoryTimerHandle, DeleteDelegate, 1.f, false);
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

void AKingCrownGiftBox::FindPlayer()
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
