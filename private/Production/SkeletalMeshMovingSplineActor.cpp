// Fill out your copyright notice in the Description page of Project Settings.


#include "Production/SkeletalMeshMovingSplineActor.h"
#include "Components/SkeletalMeshComponent.h"

ASkeletalMeshMovingSplineActor::ASkeletalMeshMovingSplineActor()
{
	PrimaryActorTick.bCanEverTick = true;

	MySkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	MySkeletalMesh->SetupAttachment(SplineComponent);

	TriggerComponent->SetupAttachment(MySkeletalMesh);
}

void ASkeletalMeshMovingSplineActor::BeginPlay()
{
	Super::BeginPlay();
	MySkeletalMesh->SetWorldLocation(StartingSplineLocation);
}

void ASkeletalMeshMovingSplineActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASkeletalMeshMovingSplineActor::ProcessMovementTimeline(float Value)
{
	Super::ProcessMovementTimeline(Value);

	FRotator SplineRotation = CurrentSplineRotation;
	SplineRotation.Pitch = 0.f;
	if (RotatorCheck)
	{
		SplineRotation.Yaw = 0.f;
	}

	MySkeletalMesh->SetWorldLocationAndRotation(CurrentSplineLocation, SplineRotation);
}
