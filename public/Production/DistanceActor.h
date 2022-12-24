// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "DistanceActor.generated.h"

UCLASS()
class PROJECTHIT_API ADistanceActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADistanceActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void OnConstruction(const FTransform& Transform) override;

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Class", meta = (AllowPrivateAccess = "true"))
	class USplineComponent* SplineComponent;
	
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float Distance;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite,meta = (AllowPrivateAccess = "true"))
	FRotator Rotation;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FVector Scale;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UStaticMesh* Mesh;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<ESplineMeshAxis::Type> ForwardAxis;

};
