// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Enemy/EnemyRangeToymonBlock.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Character/Enemy/EnemyRangeToymon.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Character/PlayerCharacter/PlayerCharacter.h"
#include "Engine/StaticMeshActor.h"
#include "Character/Boss/TwoPhaseKingCrown.h"
#include "Particles/ParticleSystemComponent.h"


AEnemyRangeToymonBlock::AEnemyRangeToymonBlock():
	DestroyBlockScale(1.f,1.f,1.f),
	Damage(50.f),
	bAttackStart(false),
	BlockDestroyTime(1.5f),
	Dissolve(1.f),
	bDeathDissolve(false),
	StartDissolve(0.f)
{
	PrimaryActorTick.bCanEverTick = true;

	AttackBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("AttackBoxComponent"));
	AttackBoxComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);
	//AttackBoxComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	SetRootComponent(AttackBoxComponent);
	AttackBoxComponent->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::AttackBoxBeginOverlap);

	BlockMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh"));
	BlockMesh->SetSimulatePhysics(false);
	BlockMesh->SetupAttachment(AttackBoxComponent);
	BlockMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BlockMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	BlockMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel5, ECollisionResponse::ECR_Ignore);

	TrailParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("TrailParticle"));
	TrailParticle->SetupAttachment(BlockMesh);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovement->UpdatedComponent = AttackBoxComponent;
	ProjectileMovement->InitialSpeed = 2000.f;
	ProjectileMovement->MaxSpeed = 5000.f;
	//ProjectileMovement->Velocity = FVector(1.f, 1.f, 1.f);
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->bAutoActivate = false;
}

void AEnemyRangeToymonBlock::BeginPlay()
{
	Super::BeginPlay();

	BlockMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BlockMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	BlockMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel5, ECollisionResponse::ECR_Ignore);
	
	if (SpawnBlockParticle)
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SpawnBlockParticle, GetParticleSpawnPos() + GetActorLocation(), GetBlockMesh()->GetComponentRotation());

	if (DynamicZeroMat)
		DynamicZeroMat->GetScalarParameterValue(FName("Dissolve_Con"), bStartDissolve);

	if (bStartDissolve == -1.f)
	{
		auto StartDissolveLamb = this;

		FTimerDelegate StartDissolveCoolDown;

		StartDissolveCoolDown.BindLambda([StartDissolveLamb]() {
			if (IsValid(StartDissolveLamb))
			{
				if(StartDissolveLamb->DynamicZeroMat)
					StartDissolveLamb->DynamicZeroMat->SetScalarParameterValue(FName("Dissolve_Con"), 1.f);
			}
			});

		GetWorldTimerManager().SetTimer(StartDissolveTimerHandle, StartDissolveCoolDown, 0.35f, false);
	}

	if (TrailParticle)
	{
		TrailParticle->Activate();
	}
}

void AEnemyRangeToymonBlock::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (ZeroMatInstance)
	{
		DynamicZeroMat = UMaterialInstanceDynamic::Create(ZeroMatInstance, this);
		BlockMesh->SetMaterial(0, DynamicZeroMat);
	}	
}

void AEnemyRangeToymonBlock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bDeathDissolve)
	{
		float DissolveTime = 0.8f * DeltaTime;

		if (Dissolve <= -1)
		{
			Dissolve = -1;
		}

		if (DynamicZeroMat)
		{
			DynamicZeroMat->SetScalarParameterValue(FName("Dissolve_Con"), Dissolve -= DissolveTime);
		}
	}
}

void AEnemyRangeToymonBlock::StartFire(AActor* Actor,float Speed,float Arc)
{
	if (Actor)
	{
		if (GetWorld()->GetFirstPlayerController()->GetCharacter())
		{
			FVector StartLoc = Actor->GetActorLocation();
			FVector TargetLoc = GetWorld()->GetFirstPlayerController()->GetCharacter()->GetMesh()->GetComponentLocation();

			float arcValue = Arc;
			FVector outVelocity = FVector::ZeroVector;

			DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			UGameplayStatics::SuggestProjectileVelocity_CustomArc(this, outVelocity, StartLoc, TargetLoc, GetWorld()->GetGravityZ(), arcValue);

			outVelocity.Normalize();

			ProjectileMovement->Velocity = outVelocity * Speed;
			ProjectileMovement->UpdateComponentVelocity();

			bAttackStart = true;
			ProjectileMovement->Activate(true);
		}
	}
}

void AEnemyRangeToymonBlock::HitDetach()
{
	BlockMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	BlockMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel5, ECollisionResponse::ECR_Ignore);
	BlockMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	BlockMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);
	BlockMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	//BlockMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	bDeathDissolve = true;

	BlockMesh->SetSimulatePhysics(true);
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	GetWorldTimerManager().SetTimer(DestoryTimeHandle, this, &ThisClass::DestroyBlock, BlockDestroyTime, false);

}

void AEnemyRangeToymonBlock::AttackBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto PlayerCharacter = Cast<APlayerCharacter>(OtherActor);
	class AStaticMeshActor* GroundActor = Cast<AStaticMeshActor>(OtherActor);

	if (PlayerCharacter && bAttackStart)
	{
		UGameplayStatics::ApplyDamage(PlayerCharacter, Damage, PlayerCharacter->GetController(), this, UDamageType::StaticClass());
		AttackBoxComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		BlockMesh->SetVisibility(false);

		if (BlockPlayerParticle)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BlockPlayerParticle, GetActorLocation() + DestroyBlockPosition,FRotator(0.f), true);
		}
		if (FMOD_BlockPlayerSound)
		{
			UFMODBlueprintStatics::PlayEventAtLocation(GetWorld(), FMOD_BlockPlayerSound, GetActorTransform(), true);
		}
		if (TrailParticle)
		{
			TrailParticle->Deactivate();
		}
		SetLifeSpan(2.f);
	}

	if (GroundActor)
	{
		AttackBoxComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		BlockMesh->SetVisibility(false);
		if (BlockGroundParticle)
		{
			FHitResult HitResult;
			const FVector Start = GetActorLocation();
			const FVector End = Start + FVector(0.f, 0.f, -300.f);
			FCollisionQueryParams QueryParams;
			QueryParams.bReturnPhysicalMaterial = true;

			bool bResult = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_GameTraceChannel1, QueryParams);

			if (bResult)
			{
				if (BlockGroundParticle)
				{		
					UGameplayStatics::SpawnEmitterAttached(BlockGroundParticle,
						HitResult.Actor->GetRootComponent(),
						TEXT("None"),
						HitResult.Location + FVector(0.f,0.f,5.f),
						FRotator(0.f),
						EAttachLocation::KeepWorldPosition);
				}
					
			}
		}
		if (FMOD_BlockGroundSound)
		{
			UFMODBlueprintStatics::PlayEventAtLocation(GetWorld(), FMOD_BlockGroundSound, GetActorTransform(), true);
		}
		if (TrailParticle)
		{
			TrailParticle->Deactivate();
		}
		SetLifeSpan(2.5f);
	}
}

void AEnemyRangeToymonBlock::DestroyBlock()
{
	Destroy();
}
