// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Production/MovingSplineActor.h"
#include "SkeletalMeshMovingSplineActor.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTHIT_API ASkeletalMeshMovingSplineActor : public AMovingSplineActor
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	ASkeletalMeshMovingSplineActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void ProcessMovementTimeline(float Value) override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spline", meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* MySkeletalMesh;


};
