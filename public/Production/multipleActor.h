// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "multipleActor.generated.h"

USTRUCT(BlueprintType)
struct FBridgeSplineStruct : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMesh* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "Distance"))
	float Divisor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "DistanceOnOFF"))
	bool Tangent;
};

UCLASS()
class PROJECTHIT_API AmultipleActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AmultipleActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void OnConstruction(const FTransform& Transform) override;

public:
	UPROPERTY(VisibleAnywhere, Category = "Spline")
	USplineComponent* SplineComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MyData")
	TArray<FBridgeSplineStruct> BridgeSpline;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UStaticMesh* Mesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Divisor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool Tangent;
	
	float DistanceA;

	float DistanceB;
	
	float TangentFloat;
};
