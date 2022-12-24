// Fill out your copyright notice in the Description page of Project Settings.

#include "../../../Public/Character/Enemy/EnemyShark.h"
#include "../../../Public/Character/Enemy/SharkBullet.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"
AEnemyShark::AEnemyShark()
{
	PrimaryActorTick.bCanEverTick = true;

	Visitors.insert(std::make_pair("Die",std::make_shared<DieVisitor>()));

}

void AEnemyShark::BeginPlay()
{
	Super::BeginPlay();


}

void AEnemyShark::Die()
{
	Super::Die();
}

void AEnemyShark::Fire()
{
	UWorld* world = GetWorld();

	if (world)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		FRotator rotator;

		const USkeletalMeshSocket* FireSocket = GetMesh()->GetSocketByName(FName("FireSocket"));
		FVector FireSocketVector = GetMesh()->GetSocketLocation(FName("FireSocket"));

		auto Bullet = world->SpawnActor<ASharkBullet>(SharkBullets, FireSocketVector, rotator, SpawnParams);
		Bullet->SetSharkLocation(GetActorLocation());

		const FTransform FireSocketTransform = FireSocket->GetSocketTransform(GetMesh());

		if (FireParticles)
		{
			UGameplayStatics::SpawnEmitterAtLocation(world, FireParticles, FireSocketTransform);
	
		}
	}

}

//void AEnemyShark::Knockback(FVector dir, float knockPower)
//{
//	//LaunchCharacter(dir * knockPower,false,false);
//}


void AEnemyShark::Accept(VisitorPtr visitor)
{
	visitor->Visit(this);
}
