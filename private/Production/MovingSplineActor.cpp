// Fill out your copyright notice in the Description page of Project Settings.


#include "Production/MovingSplineActor.h"

// Sets default values
AMovingSplineActor::AMovingSplineActor() :
	bRevers(false),
	RotatorCheck(false)

{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("Spline Component"));
	SetRootComponent(SplineComponent);

	TriggerComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger Component"));
	TriggerComponent->SetupAttachment(SplineComponent);

}

// Called when the game starts or when spawned
void AMovingSplineActor::BeginPlay()
{
	Super::BeginPlay();

	TriggerComponent->OnComponentBeginOverlap.AddUniqueDynamic(this, &AMovingSplineActor::TriggerBeginOverlap);
	TriggerComponent->OnComponentEndOverlap.AddUniqueDynamic(this, &AMovingSplineActor::TriggerEndOverlap);
	
	StartingSplineLocation = SplineComponent->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World);

	if (!MovementCurve)
	{
		return;
	}

	FOnTimelineFloat ProgressFunction;
	ProgressFunction.BindUFunction(this, TEXT("ProcessMovementTimeline"));
	MovementTimeline.AddInterpFloat(MovementCurve, ProgressFunction);

	FOnTimelineEvent OnTimelineFinishedFunction;
	OnTimelineFinishedFunction.BindUFunction(this, TEXT("OnEndMovementTimeline"));
	MovementTimeline.SetTimelineFinishedFunc(OnTimelineFinishedFunction);

	MovementTimeline.SetTimelineLengthMode(TL_LastKeyFrame);

	if (bAutoActivate)
	{
		MovementTimeline.PlayFromStart();
	}

	if (bAutoReverseOnEndTimeline)
	{
		TriggerComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

}

// Called every frame
void AMovingSplineActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MovementTimeline.IsPlaying())
	{
		MovementTimeline.TickTimeline(DeltaTime);
	}

}

void AMovingSplineActor::ProcessMovementTimeline(float Value)
{
	const float SplineLength = SplineComponent->GetSplineLength();

	CurrentSplineLocation = SplineComponent->GetLocationAtDistanceAlongSpline(Value * SplineLength, ESplineCoordinateSpace::World);
	CurrentSplineRotation = SplineComponent->GetRotationAtDistanceAlongSpline(Value * SplineLength, ESplineCoordinateSpace::World);

}

void AMovingSplineActor::OnEndMovementTimeline()
{
	//TODO manuel Reverse
	if (bReverseOnEndTimeline)
	{
		MovementTimeline.Reverse();
	}
	else if (bRestartOnEndTimeline)
	{
		MovementTimeline.PlayFromStart();
	}
	else if (bAutoReverseOnEndTimeline)
	{
		bRevers = !bRevers;
		if (bRevers)
		{
			MovementTimeline.Reverse();
		}
		else
		{
			MovementTimeline.PlayFromStart();
		}
	}
}

void AMovingSplineActor::TriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//MovementTimeline.PlayFromStart();
	MovementTimeline.Play();
}

void AMovingSplineActor::TriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!MovementTimeline.IsReversing())
	{
		MovementTimeline.Reverse();
	}
}

