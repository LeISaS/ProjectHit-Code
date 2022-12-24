// Fill out your copyright notice in the Description page of Project Settings.


#include "Production/SplineActor.h"

// Sets default values
ASplineActor::ASplineActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SplineComponent = CreateDefaultSubobject<USplineComponent>("Spline");

	if (SplineComponent)
	{
		SetRootComponent(SplineComponent);
	}


}

void ASplineActor::OnConstruction(const FTransform& Transform)
{
	if (SplineMeshMap.Num() > 0)
	{
		FSplineMeshDetails* DefaultMeshDetails = nullptr;
		if (SplineMeshMap.Contains(ESplineMeshType::DEFAULT))
		{
			DefaultMeshDetails = SplineMeshMap.Find(ESplineMeshType::DEFAULT);
		}
		else
		{
			return;
		}

		FSplineMeshDetails* StartMeshDetails = nullptr;
		if (SplineMeshMap.Contains(ESplineMeshType::START))
		{
			StartMeshDetails = SplineMeshMap.Find(ESplineMeshType::START);
		}

		FSplineMeshDetails* EndMeshDetails = nullptr;
		if (SplineMeshMap.Contains(ESplineMeshType::END))
		{
			EndMeshDetails = SplineMeshMap.Find(ESplineMeshType::END);
		}


		for (int SplineCount = 0; SplineCount < (SplineComponent->GetNumberOfSplinePoints() - 1); SplineCount++)
		{
			USplineMeshComponent* SplineMeshComponent = NewObject<USplineMeshComponent>(this, USplineMeshComponent::StaticClass());
			
			SplineMeshComponent->bCastDynamicShadow = false;

			SplineMeshComponent->SetMobility(EComponentMobility::Movable);
			SplineMeshComponent->CreationMethod = EComponentCreationMethod::UserConstructionScript;
			SplineMeshComponent->RegisterComponentWithWorld(GetWorld());
			SplineMeshComponent->AttachToComponent(SplineComponent, FAttachmentTransformRules::KeepRelativeTransform);

			const FVector StartPoint = SplineComponent->GetLocationAtSplinePoint(SplineCount, ESplineCoordinateSpace::Local);
			const FVector StartTangent = SplineComponent->GetTangentAtSplinePoint(SplineCount, ESplineCoordinateSpace::Local);
			const FVector EndPoint = SplineComponent->GetLocationAtSplinePoint(SplineCount + 1, ESplineCoordinateSpace::Local);
			const FVector EndTangent = SplineComponent->GetTangentAtSplinePoint(SplineCount + 1, ESplineCoordinateSpace::Local);

			SplineMeshComponent->SetStartAndEnd(StartPoint, StartTangent, EndPoint, EndTangent, true);

			SplineMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

			if (StartMeshDetails && StartMeshDetails->Mesh && SplineCount == 0)	//Start
			{
				SplineMeshComponent->SetStaticMesh(StartMeshDetails->Mesh);
				SplineMeshComponent->SetForwardAxis(StartMeshDetails->ForwardAxis);
			}
			//end
			else if (EndMeshDetails && EndMeshDetails->Mesh && SplineComponent->GetNumberOfSplinePoints() > 2 && SplineCount == (SplineComponent->GetNumberOfSplinePoints() - 2))
			{
				SplineMeshComponent->SetStaticMesh(EndMeshDetails->Mesh);
				SplineMeshComponent->SetForwardAxis(EndMeshDetails->ForwardAxis);
			}
			//juicy middle
			else
			{
				if (DefaultMeshDetails->Mesh)
				{
					SplineMeshComponent->SetStaticMesh(DefaultMeshDetails->Mesh);
					SplineMeshComponent->SetForwardAxis(DefaultMeshDetails->ForwardAxis);

					if (DefaultMeshDetails->AlternativeMaterial && SplineCount > 0 && SplineCount % 2 == 0)
					{
						SplineMeshComponent->SetMaterial(0, DefaultMeshDetails->AlternativeMaterial);
					}
					else if (DefaultMeshDetails->DefaultMaterial)
					{
						SplineMeshComponent->SetMaterial(0, DefaultMeshDetails->DefaultMaterial);
					}
				}
			}
		}
	}
}

// Called when the game starts or when spawned
void ASplineActor::BeginPlay()
{
	Super::BeginPlay();
	RerunConstructionScripts();
	SplineComponent->AddSplinePoint(FVector(200.f, 200.f, 200.f), ESplineCoordinateSpace::Local);
}

// Called every frame
void ASplineActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

