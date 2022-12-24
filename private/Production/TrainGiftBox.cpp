// Fill out your copyright notice in the Description page of Project Settings.


#include "Production/TrainGiftBox.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SkeletalMeshComponent.h"
#include "../../Public/Item/DontStopProjectileMovement.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstance.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "../../Public/Item/Sword.h"
#include "Components/WidgetComponent.h"


ATrainGiftBox::ATrainGiftBox() :
	Train(nullptr),
	MaxHp(5.f),
	NowHp(5.f),
	FlyingSpeed(3000.f),
	bFalling(false),
	bGround(false),
	LerpScale(0.0f),
	SpwanTime(4.f),
	Duration(4.f),
	DurationRatio(0.25f),
	StartZOffset(0.f),
	DirectionDistance(600.f),
	FlyingZLength(50.f),
	FlyingZSpeed(2.f),
	ZCosOffSet(0.0f)

{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GiftBoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("GiftBoxCollsiion"));
	SetRootComponent(GiftBoxCollision);

	GiftMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GiftMesh"));
	GiftMesh->SetupAttachment(GiftBoxCollision);

	BalloonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BalloonMesh"));
	BalloonMesh->SetupAttachment(GiftBoxCollision);

	HealthBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidget"));
	HealthBarWidget->SetupAttachment(GetRootComponent());

	ProjectileMovementComponent = CreateDefaultSubobject<UDontStopProjectileMovement>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->bRotationRemainsVertical = true;
	ProjectileMovementComponent->ProjectileGravityScale = 0.f;
	ProjectileMovementComponent->MaxSpeed = 10000.f;
	ProjectileMovementComponent->Velocity = FVector(0.f, 0.f, 0.f);
}

void ATrainGiftBox::OnConstruction(const FTransform& Transform)
{
	if (BallonMatInstance)
	{
		DynamicBallonMatInstance = UMaterialInstanceDynamic::Create(BallonMatInstance, this);
		BalloonMesh->SetMaterial(0, DynamicBallonMatInstance);
	}

	if (ZeroMatInstance)
	{
		DynamicZeroMat = UMaterialInstanceDynamic::Create(ZeroMatInstance, this);
		GiftMesh->SetMaterial(0, DynamicZeroMat);
	}

}

void ATrainGiftBox::BeginPlay()
{
	Super::BeginPlay();

	if (GiftMatCurve)
	{
		FOnTimelineFloat GiftFunction;
		GiftFunction.BindUFunction(this, FName("GiftLine"));
		GiftTimeline.AddInterpFloat(GiftMatCurve, GiftFunction);

		GiftTimeline.SetTimelineLengthMode(TL_LastKeyFrame);
	}


	HealthBarWidget->GetUserWidgetObject()->SetVisibility(ESlateVisibility::Hidden);
	GiftBoxCollision->OnComponentBeginOverlap.AddUniqueDynamic(this, &ATrainGiftBox::SwordOverlap);

	DurationRatio = (1.f / Duration);
	//UE_LOG(LogTemp, Warning, TEXT("DurationRatio = %f"), DurationRatio);
	NowHp = MaxHp;

	if (Train)
	{
		FVector EndVector = GetActorLocation() + (GetActorForwardVector() * DirectionDistance);
		DirectionVector = GetActorLocation() - EndVector;
		StartZOffset = GetActorLocation().Z - Train->GetActorLocation().Z;
		DirectionVector.Z = 0;

		StartYaw = Train->GetActorRotation().Yaw;

		EndOffset = EndVector - Train->GetActorLocation();
	}	
}

void ATrainGiftBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (GiftTimeline.IsPlaying())
	{
		GiftTimeline.TickTimeline(DeltaTime);
	}

	if (Train && NowHp >= 0.f)
	{
		float distance = FVector::DistXY(Train->GetActorLocation(), GetActorLocation());

		FVector EndVec = Train->GetActorLocation();
		FVector DirVec = DirectionVector;
		FRotator TrainRot = Train->GetActorRotation();
		

		float cos = FMath::Cos(FMath::DegreesToRadians(TrainRot.Yaw - StartYaw));
		float sin = FMath::Sin(FMath::DegreesToRadians(TrainRot.Yaw - StartYaw));

		ZCosOffSet += DeltaTime * FlyingZSpeed;
		EndVec.Z += StartZOffset + (FlyingZLength * FMath::Cos(ZCosOffSet));

		if (LerpScale >= 1.0f)	LerpScale = 1.0f;
		else LerpScale += DeltaTime * DurationRatio;

		if (LerpScale >= 0.97f && bFalling == false)
		{
			ProjectileMovementComponent->ProjectileGravityScale = 5.f;
			DirectionVector.Z = 0;

			OffBallon();

			bFalling = true;
			bGround = true;
			FTimerDelegate SpawnDelegate;
			SpawnDelegate.BindLambda([this]() {
				UAnimInstance* AnimInstance = GiftMesh->GetAnimInstance();
				if (AnimInstance) AnimInstance->Montage_Play(MoveMontage);
				});
			GetWorldTimerManager().SetTimer(SpawnTimerHandle, SpawnDelegate, SpwanTime + 0.3f, false);

		}
		
		float OffX = EndOffset.X;
		float OffY = EndOffset.Y;
		EndVec.X += (OffX * cos - OffY * sin);
		EndVec.Y += (OffX * sin + OffY * cos);

		DirVec.X *= sin;
		DirVec.Y *= cos;

		LerpLocation = FMath::Lerp(EndVec + DirectionVector, EndVec, LerpScale);

		if (bFalling) LerpLocation.Z = GetActorLocation().Z;
		SetActorLocation(LerpLocation);
	}
}

void ATrainGiftBox::SwordOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto HitSword = Cast<ASword>(OtherActor);

	if (HitSword)
	{
		NowHp -= 10.f;
		HealthBarWidget->GetUserWidgetObject()->SetVisibility(ESlateVisibility::Visible);
		
		UFMODBlueprintStatics::PlayEventAtLocation(GetWorld(), FMOD_Back, GetActorTransform(), true);

		auto Location = HitSword->GetHitParticleLocation(GetActorLocation(), GiftBoxCollision->GetScaledBoxExtent().Size() / 2);

		if (PS_CharSlash)
		{
			UGameplayStatics::SpawnEmitterAttached(
				PS_CharSlash,
				GiftMesh,
				TEXT("None"),
				Location,
				GetActorRotation(),
				EAttachLocation::KeepWorldPosition);
		}

		DynamicZeroMat->SetScalarParameterValue(FName("DamageColorSwitch"), 1.0f);

		FTimerDelegate OffDamageColor;
		OffDamageColor.BindLambda([this]() {
			DynamicZeroMat->SetScalarParameterValue(FName("DamageColorSwitch"), 0.f);
			});

		GetWorldTimerManager().SetTimer(OffDamageTimerHandle, OffDamageColor, 0.2f, false);

		if (NowHp < 0.f)
		{
			UAnimInstance* AnimInstance = GiftMesh->GetAnimInstance();
			if (AnimInstance)
				if(AnimInstance->Montage_IsPlaying(MoveMontage))
					AnimInstance->Montage_Stop(0.f, MoveMontage);

			FVector FlyingDir = GetActorLocation() - OtherActor->GetActorLocation();
			FlyingDir.Z = 0.f;
			FlyingDir.Normalize();
			FlyingDir.Z = 0.7f;

			ProjectileMovementComponent->Velocity = FlyingDir * FlyingSpeed;
			SetActorEnableCollision(false);
			GetWorldTimerManager().ClearTimer(SpawnTimerHandle);

			FTimerDelegate DeleteDelegate;

			DeleteDelegate.BindLambda([this]() {
				const USkeletalMeshSocket* DestorySocket = GiftMesh->GetSocketByName(FName("ParticleSocket"));
				const FTransform DestoryTransform = DestorySocket->GetSocketTransform(GiftMesh);

				if (DeathParticle)
				{
					UFMODBlueprintStatics::PlayEvent2D(GetWorld(), FMOD_Death, true);
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DeathParticle, DestoryTransform);
				}
				GiftBoxCollision->OnComponentBeginOverlap.RemoveAll(this);
				Destroy();
				});
			GetWorldTimerManager().SetTimer(DestoryTimerHandle, DeleteDelegate, 1.f, false);

			//FlyingLocation = FlyingDir * 5.f;
		}
	}
}

void ATrainGiftBox::SpawnMonster()
{
	const USkeletalMeshSocket* DestorySocket = GiftMesh->GetSocketByName(FName("ParticleSocket"));
	const FTransform DestoryTransform = DestorySocket->GetSocketTransform(GiftMesh);

	FHitResult HitResult;
	const FVector Start = GetActorLocation();
	const FVector End = Start + FVector(0.f, 0.f, -600.f);
	FCollisionQueryParams QueryParams;
	QueryParams.bReturnPhysicalMaterial = true;

	bool bResult = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_GameTraceChannel1, QueryParams);

	if (DestroyParticle && bResult)
	{

		if(HitResult.Actor->GetRootComponent())
			UGameplayStatics::SpawnEmitterAttached(
			DestroyParticle,
			HitResult.Actor->GetRootComponent(),
			TEXT("None"),
			DestoryTransform.GetLocation(),
			DestoryTransform.GetRotation().Rotator(),
			EAttachLocation::KeepWorldPosition);
	}
	GiftBoxCollision->OnComponentBeginOverlap.RemoveAll(this);
	Destroy();
	RandomSpawn();
}

void ATrainGiftBox::OffBallon()
{
	if (DynamicBallonMatInstance)
	{
		DynamicBallonMatInstance->SetScalarParameterValue(TEXT("Opacity"), 0);
		
	}

	if (BoomParticle)
	{
		UGameplayStatics::SpawnEmitterAttached(
			BoomParticle,
			GiftMesh,
			TEXT("None"),
			GetActorLocation() + FVector(0.f, 0.f, 200.f),
			GetActorRotation(),
			EAttachLocation::KeepWorldPosition);

			UFMODBlueprintStatics::PlayEventAtLocation(GetWorld(), FMOD_Balloon,GetActorTransform(),true);
	}


}

void ATrainGiftBox::CurveStart()
{
	GiftTimeline.PlayFromStart();
}

void ATrainGiftBox::GiftLine(float Value)
{
	DynamicZeroMat->SetScalarParameterValue(FName("TikTokColorSwitch"), Value);
}




