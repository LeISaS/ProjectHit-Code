// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Engine/DataTable.h"
#include "GameFramework/Actor.h"
#include "SplineActor.generated.h"

UENUM(BlueprintType)
enum class ESplineMeshType : uint8
{
	DEFAULT			UMETA(DisplayName = "Default Mesh"),
	START				UMETA(DisplayName = "Starting Mesh"),
	END					UMETA(DisplayName = "Ending Mesh"),
};

USTRUCT(BlueprintType)
struct FSplineMeshDetails : public FTableRowBase
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UStaticMesh* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TEnumAsByte<ESplineMeshAxis::Type> ForwardAxis;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		class UMaterialInterface* DefaultMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		class UMaterialInterface* AlternativeMaterial;

	FSplineMeshDetails() : ForwardAxis(ESplineMeshAxis::Type::X)
	{
	}
};

UCLASS()
class PROJECTHIT_API ASplineActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASplineActor();

	void OnConstruction(const FTransform& Transform) override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(VisibleAnywhere, Category = "Spline")
		USplineComponent* SplineComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Spline")
		TMap<ESplineMeshType, FSplineMeshDetails> SplineMeshMap;
};
