// Fill out your copyright notice in the Description page of Project Settings.


#include "Production/DistanceActor.h"

// Sets default values
ADistanceActor::ADistanceActor():
	Distance(100.f),
	Rotation(0.f),
	Scale(1.f),
	ForwardAxis(ESplineMeshAxis::Type::X)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineMesh"));
	SplineComponent->SetupAttachment(GetRootComponent());

}

// Called when the game starts or when spawned
void ADistanceActor::BeginPlay()
{
	Super::BeginPlay();
	
	RerunConstructionScripts();
}

// Called every frame
void ADistanceActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADistanceActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	int Divide = UKismetMathLibrary::SafeDivide(SplineComponent->GetSplineLength(), Distance);

	for (int SplineCount = 0; SplineCount < Divide; SplineCount++)
	{
		USplineMeshComponent* SplineMeshComponent = NewObject<USplineMeshComponent>(this, USplineMeshComponent::StaticClass());

		SplineMeshComponent->bCastDynamicShadow = false;

		SplineMeshComponent->SetMobility(EComponentMobility::Movable);
		SplineMeshComponent->CreationMethod = EComponentCreationMethod::UserConstructionScript;
		SplineMeshComponent->RegisterComponentWithWorld(GetWorld());
		SplineMeshComponent->AttachToComponent(SplineComponent, FAttachmentTransformRules::KeepRelativeTransform);

		auto SplineTransform = SplineComponent->GetLocationAtDistanceAlongSpline(Distance * SplineCount, ESplineCoordinateSpace::Local);
		auto myTransform = UKismetMathLibrary::MakeTransform(SplineTransform, Rotation, Scale);

		SplineMeshComponent->SetRelativeLocation(SplineTransform);
		SplineMeshComponent->SetRelativeRotation(Rotation);
		SplineMeshComponent->SetWorldScale3D(Scale);

		SplineMeshComponent->SetStaticMesh(Mesh);
		SplineMeshComponent->SetForwardAxis(ForwardAxis);
	}




}



