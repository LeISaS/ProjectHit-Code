// Fill out your copyright notice in the Description page of Project Settings.


#include "../../../Public/Character/Enemy/SharkBullet.h"
#include "../../../Public/Character/Enemy/EnemyController.h"
#include "../../../Public/Character/Enemy/Enemy.h"
#include "../../../Public/Character/PlayerCharacter/PlayerCharacter.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
#include "Math/Vector.h"
#include "Components/SphereComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DrawDebugHelpers.h"

// Sets default values
ASharkBullet::ASharkBullet() :
	Speed(3000.f),
	ProjectileSpeed(2000.f),
	BaseDamage(10),
	ParryingSplashDamage(60.f),
	AttackRadius(50.f)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BulletMesh"));
	SetRootComponent(BulletMesh);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->bRotationFollowsVelocity = true;

	ExplositionCollision = CreateDefaultSubobject<USphereComponent>(TEXT("ExplositionCollision"));
	ExplositionCollision->SetupAttachment(RootComponent);

	BoomDamageCollision = CreateDefaultSubobject<USphereComponent>(TEXT("BoomDamageCollision"));
	BoomDamageCollision->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void ASharkBullet::BeginPlay()
{
	Super::BeginPlay();

	ExplositionCollision->OnComponentBeginOverlap.AddUniqueDynamic(this, &ASharkBullet::ExplositionSphereOverlap);

	//TODO BP -> CPP
	//SuggestFire();
	
}

// Called every frame
void ASharkBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASharkBullet::ExplositionSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//TODO FMOD Sound
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor);
	if (PlayerCharacter)
	{
		if (ExplositionParticles)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplositionParticles, this->GetActorLocation(), FRotator(0.f), true);
		}
		UGameplayStatics::ApplyDamage(PlayerCharacter, BaseDamage, PlayerCharacter->GetInstigatorController(), this, UDamageType::StaticClass());
		Destroy();
	}

	//TODO Add Collision & Enemy Damaged
	/*
	AEnemy* Enemy = Cast<AEnemy>(OtherActor);

	AEnemyController* EnemyController = Cast<AEnemyController>(Enemy->GetController());

	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors, ACharacter::StaticClass());

	for (auto Actor : OverlappingActors)
	{
		if (PlayerCharacter)
		{
			UGameplayStatics::ApplyDamage(Actor, BaseDamage, Actor->GetInstigatorController(), this, UDamageType::StaticClass());
		}
		if (Enemy)
		{
			UGameplayStatics::ApplyDamage(Actor, BaseDamage, Actor->GetInstigatorController(), this, UDamageType::StaticClass());
			if (EnemyController)
			{
				EnemyController->GetBlackBoardComponent()->SetValueAsObject(FName("Target"), PlayerCharacter);
		UE_LOG(LogTemp, Warning, TEXT("Actor damaged by Explosive : %s"), *Actor->GetName());
			}
		}
	}*/
}

void ASharkBullet::SuggestFire()
{

	//if (PlayerCharacter)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("SuggestFire"));

	//	FVector outVelocity;
	//	
	//	FVector StartLocation = GetActorLocation();
	//	//TODO Distance Fix
	//	float Distance = GetDistanceTo();
	//	FVector Velocity= GetVelocity();

	//	FVector DistanceTo = Distance * Velocity;

	//	FVector ActorToDistance = DistanceTo + StartLocation;

	//	UGameplayStatics::SuggestProjectileVelocity(
	//		this,
	//		outVelocity,
	//		StartLocation,
	//		ActorToDistance,
	//		ProjectileSpeed,
	//		false,
	//		0.0f,
	//		0.0f,
	//		ESuggestProjVelocityTraceOption::DoNotTrace);

	//	ProjectileMovementComponent->Velocity = outVelocity;
	//}

}

TArray<FHitResult> ASharkBullet::PlayerParrying()
{
	TArray<FHitResult> HitResults;

	bool bResult = GetWorld()->SweepMultiByChannel(
		OUT HitResults,
		GetActorLocation(),
		GetActorLocation(),
		FQuat::Identity,
		ECollisionChannel::ECC_Visibility,
		FCollisionShape::MakeSphere(AttackRadius));

	FColor DrawColor;
	if (bResult)
		DrawColor = FColor::Green;
	else
		DrawColor = FColor::Red;

	DrawDebugSphere(GetWorld(), GetActorLocation(), AttackRadius, 10, DrawColor, false, 2.f);
	
	return HitResults;
}
