// Fill out your copyright notice in the Description page of Project Settings.


#include "Production/multipleActor.h"

// Sets default values
AmultipleActor::AmultipleActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineMesh"));
	SplineComponent->SetupAttachment(GetRootComponent());

}

// Called when the game starts or when spawned
void AmultipleActor::BeginPlay()
{
	Super::BeginPlay();
	RerunConstructionScripts();

}

// Called every frame
void AmultipleActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AmultipleActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
	for (int i = 0; i < BridgeSpline.Num(); i++)
	{
		Mesh = BridgeSpline[i].Mesh;
		Divisor = BridgeSpline[i].Divisor;
		Tangent = BridgeSpline[i].Tangent;

		int Divide = UKismetMathLibrary::SafeDivide(SplineComponent->GetSplineLength(), Divisor);

		for (int SplineCount = 0; SplineCount < Divide; SplineCount++)
		{
			USplineMeshComponent* SplineMeshComponent = NewObject<USplineMeshComponent>(this, USplineMeshComponent::StaticClass());

			SplineMeshComponent->bCastDynamicShadow = false;

			float Temp = SplineCount * Divisor;

			DistanceA = Temp;

			DistanceB = Temp + Divisor;

			if (Tangent)
			{
				SplineMeshComponent->SetMobility(EComponentMobility::Movable);
				SplineMeshComponent->CreationMethod = EComponentCreationMethod::UserConstructionScript;
				SplineMeshComponent->RegisterComponentWithWorld(GetWorld());
				SplineMeshComponent->AttachToComponent(SplineComponent, FAttachmentTransformRules::KeepRelativeTransform);

				const FVector StartPoint = SplineComponent->GetLocationAtDistanceAlongSpline(DistanceA, ESplineCoordinateSpace::Local);
				const FVector StartTangent = SplineComponent->GetDirectionAtDistanceAlongSpline(DistanceA, ESplineCoordinateSpace::Local);

				const FVector EndPoint = SplineComponent->GetLocationAtDistanceAlongSpline(DistanceB, ESplineCoordinateSpace::Local);
				const FVector EndTangent = SplineComponent->GetDirectionAtDistanceAlongSpline(DistanceB, ESplineCoordinateSpace::Local);


				SplineMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

				SplineMeshComponent->SetStaticMesh(Mesh);

				auto StartTangentPlus = UKismetMathLibrary::Add_VectorFloat(StartTangent, TangentFloat);
				auto EndTangentPlus = UKismetMathLibrary::Add_VectorFloat(EndTangent, TangentFloat);

				SplineMeshComponent->SetStartAndEnd(StartPoint, StartTangentPlus, EndPoint, EndTangentPlus, true);
			}
			else
			{
				SplineMeshComponent->SetMobility(EComponentMobility::Movable);
				SplineMeshComponent->CreationMethod = EComponentCreationMethod::UserConstructionScript;
				SplineMeshComponent->RegisterComponentWithWorld(GetWorld());
				SplineMeshComponent->AttachToComponent(SplineComponent, FAttachmentTransformRules::KeepRelativeTransform);

				FVector myVec = SplineComponent->GetLocationAtDistanceAlongSpline(DistanceA, ESplineCoordinateSpace::Local);
				FRotator myRot = SplineComponent->GetRotationAtDistanceAlongSpline(DistanceA, ESplineCoordinateSpace::Local);

				auto myTransfrom = UKismetMathLibrary::MakeTransform(myVec, myRot, FVector(1.f, 1.f, 1.f));

				SplineMeshComponent->SetRelativeLocation(myVec);
				SplineMeshComponent->SetRelativeRotation(myRot);

				SplineMeshComponent->SetStaticMesh(Mesh);
			}
		}
	}
	
}

