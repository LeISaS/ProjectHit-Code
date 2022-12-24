// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Boss/TrainKingCrown.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Character/Boss/TrainKingCrownGiftBox.h"
#include "../../../Public/Character/Enemy/EnemyController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

ATrainKingCrown::ATrainKingCrown():
	CurrentPrice(0),
	EndPrice(0),
	CurrentShotNumber(0),
	bReverse(false),
	FireSocket("TrainGiftFireSocket"),
	FireSocketScale(FVector(1.f,1.f,1.f))
{
	PrimaryActorTick.bCanEverTick = true;

	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("Spline Component"));
	SplineComponent->SetupAttachment(GetRootComponent());

	StartSplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("StartSpline Component"));
	StartSplineComponent->SetupAttachment(GetRootComponent());

	FinalSplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("FinalSplineComponent"));
	FinalSplineComponent->SetupAttachment(GetRootComponent());


	PointBox_1 = CreateDefaultSubobject<UBoxComponent>(TEXT("PointBox_1"));
	PointBox_1->SetupAttachment(GetRootComponent());

	PointBox_2 = CreateDefaultSubobject<UBoxComponent>(TEXT("PointBox_2"));
	PointBox_2->SetupAttachment(GetRootComponent());

	PointBox_3 = CreateDefaultSubobject<UBoxComponent>(TEXT("PointBox_3"));
	PointBox_3->SetupAttachment(GetRootComponent());

	PointBox_4 = CreateDefaultSubobject<UBoxComponent>(TEXT("PointBox_4"));
	PointBox_4->SetupAttachment(GetRootComponent());

	PointBox_5 = CreateDefaultSubobject<UBoxComponent>(TEXT("PointBox_5"));
	PointBox_5->SetupAttachment(GetRootComponent());

	PointBox_6 = CreateDefaultSubobject<UBoxComponent>(TEXT("PointBox_6"));
	PointBox_6->SetupAttachment(GetRootComponent());

	PointBox_7 = CreateDefaultSubobject<UBoxComponent>(TEXT("PointBox_7"));
	PointBox_7->SetupAttachment(GetRootComponent());
}

void ATrainKingCrown::BeginPlay()
{
	Super::BeginPlay();

	SetPullType(EPullType::EPT_DISALLOW);
	if (EnemyController)
	{
		SetTrainStateEnum(ETrainBossState::ETBS_START);

		EnemyController->GetBlackBoardComponent()->SetValueAsInt(FName("CurrentPrice"), 0);
		EnemyController->GetBlackBoardComponent()->SetValueAsInt(FName("CurrentShotNumber"), 0);
	}

	StartingSplineLocation = StartSplineComponent->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World);

	GetCapsuleComponent()->SetWorldLocation(StartingSplineLocation);

	ArrPointBox.Add(PointBox_1);
	ArrPointBox.Add(PointBox_2);
	ArrPointBox.Add(PointBox_3);
	ArrPointBox.Add(PointBox_4);
	ArrPointBox.Add(PointBox_5);
	ArrPointBox.Add(PointBox_6);
	ArrPointBox.Add(PointBox_7);

	RerunConstructionScripts();

	if (!MoveCurve && !StartCurve && !FinalCurve)
	{
		return;
	}

	FOnTimelineFloat StartFunction;
	StartFunction.BindUFunction(this, TEXT("StartMovingMesh"));
	StartTimeline.AddInterpFloat(StartCurve, StartFunction);

	FOnTimelineEvent StartFinishFunction;
	StartFinishFunction.BindUFunction(this, TEXT("StartEndMovingMesh"));
	StartTimeline.SetTimelineFinishedFunc(StartFinishFunction);

	StartTimeline.SetTimelineLengthMode(TL_LastKeyFrame);

	FOnTimelineFloat MoveFunction;
	MoveFunction.BindUFunction(this, TEXT("MovingMesh"));
	MoveTimeline.AddInterpFloat(MoveCurve, MoveFunction);

	FOnTimelineEvent MoveFinishFunction;
	MoveFinishFunction.BindUFunction(this, TEXT("EndMovingMesh"));
	MoveTimeline.SetTimelineFinishedFunc(MoveFinishFunction);

	MoveTimeline.SetTimelineLengthMode(TL_LastKeyFrame);

	FOnTimelineFloat FinalFunction;
	FinalFunction.BindUFunction(this, TEXT("FianlMovingMesh"));
	FinalTimeline.AddInterpFloat(FinalCurve, FinalFunction);



	//MoveTimeline.PlayFromStart();
	StartTimeline.PlayFromStart();

	if (EnemyController)
	{
		EnemyController->GetBlackBoardComponent()->SetValueAsBool("bIsStart", true);
	}
	
}

void ATrainKingCrown::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MoveTimeline.IsPlaying())
	{
		MoveTimeline.TickTimeline(DeltaTime);
	}

	if (StartTimeline.IsPlaying())
	{
		StartTimeline.TickTimeline(DeltaTime);
	}

	if (FinalTimeline.IsPlaying())
	{
		FinalTimeline.TickTimeline(DeltaTime);
	}

	if (GetTrainBossState() == ETrainBossState::ETBS_FOUR)
	{
		if (MoveTimeline.IsPlaying())
		{
			MoveTimeline.Stop();
			FinalSplineComponent->AddSplinePoint(GetMesh()->GetComponentLocation(),ESplineCoordinateSpace::World);
			FinalTimeline.ReverseFromEnd();
		}
	}

}

void ATrainKingCrown::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void ATrainKingCrown::MovingMesh(float Value)
{
	const float SplineLength = SplineComponent->GetSplineLength();

	CurrentSplineLocation = SplineComponent->GetWorldLocationAtDistanceAlongSpline(Value * SplineLength);
	CurrentSplineRotation = SplineComponent->GetWorldRotationAtDistanceAlongSpline(Value * SplineLength);

	GetMesh()->SetWorldLocation(CurrentSplineLocation);

}

void ATrainKingCrown::StartMovingMesh(float Value)
{
	const float SplineLength = StartSplineComponent->GetSplineLength();

	CurrentSplineLocation = StartSplineComponent->GetWorldLocationAtDistanceAlongSpline(Value * SplineLength);
	CurrentSplineRotation = StartSplineComponent->GetWorldRotationAtDistanceAlongSpline(Value * SplineLength);

	GetMesh()->SetWorldLocation(CurrentSplineLocation);
	
}

void ATrainKingCrown::EndMovingMesh()
{
	MoveTimeline.PlayFromStart();
}

void ATrainKingCrown::StartEndMovingMesh()
{
	StartingSplineLocation = SplineComponent->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World);

	GetCapsuleComponent()->SetWorldLocation(StartingSplineLocation);
	MoveTimeline.PlayFromStart();
}

void ATrainKingCrown::SetEndPrice(int Price)
{
	EndPrice = Price;

	if (EnemyController)
	{
		EnemyController->GetBlackBoardComponent()->SetValueAsInt(FName("EndPrice"), EndPrice);
	}

}

void ATrainKingCrown::FianlMovingMesh(float Value)
{
	const float SplineLength = FinalSplineComponent->GetSplineLength();

	CurrentSplineLocation = FinalSplineComponent->GetWorldLocationAtDistanceAlongSpline(Value * SplineLength);
	CurrentSplineRotation = FinalSplineComponent->GetWorldRotationAtDistanceAlongSpline(Value * SplineLength);

	GetMesh()->SetWorldLocation(CurrentSplineLocation);
}

void ATrainKingCrown::SetTrainStateEnum(ETrainBossState State)
{
	if (EnemyController)
	{
		SetTrainBossState(State);
		EnemyController->GetBlackBoardComponent()->SetValueAsEnum(FName("ETrainBossState"), uint8(State));
	}
}

void ATrainKingCrown::SetBoolGiftPatternStart()
{
	if (EnemyController)
	{
		EnemyController->GetBlackBoardComponent()->SetValueAsEnum("ETrainBossState", uint8(ETrainBossState::ETBS_ONE));
	}
}

FVector ATrainKingCrown::GetBoxPoint(int Index)
{
	FVector Origin = ArrPointBox[Index]->GetComponentTransform().GetLocation();

	return Origin;
}

void ATrainKingCrown::ShotGiftBox(int StartNumber, int EndNumber,bool FireSelected)
{
	if (EnemyController)
	{
		EnemyController->GetBlackBoardComponent()->SetValueAsInt(FName("CurrentShotNumber"), ++CurrentShotNumber);
		GiftStartNumber = StartNumber;
		GiftEndNumber = EndNumber;
		bFireSelected = FireSelected;
	}

	if (TrainGiftBoxClass)
	{
		if (GetWorld())
		{
			FRotator FireSocketRot = GetMesh()->GetSocketRotation(FireSocket);
			FVector FireSocketLoc = GetMesh()->GetSocketLocation(FireSocket);

			FActorSpawnParameters Spawnparams;
			Spawnparams.Owner = this;
			TrainGiftBox = GetWorld()->SpawnActor<ATrainKingCrownGiftBox>(TrainGiftBoxClass, FireSocketLoc, FireSocketRot, Spawnparams);

			if (FMOD_GiftFire)
			{
				PlayFModEventSound(FMOD_GiftFire);
			}

			if (GetPS_GiftFireParticles())
			{
				UGameplayStatics::SpawnEmitterAttached(
					GetPS_GiftFireParticles(),
					GetMesh(),
					TEXT("None"),
					FireSocketLoc,
					FireSocketRot,
					FireSocketScale,
					EAttachLocation::KeepWorldPosition);
			}


		}
	}
}

void ATrainKingCrown::CurrentPricePlus(int number)
{
	CurrentPrice+=number;

	if (EnemyController)
	{
		EnemyController->GetBlackBoardComponent()->SetValueAsInt(FName("CurrentPrice"), CurrentPrice);
	}
}

void ATrainKingCrown::ChildCurrentPricePlus()
{
	CurrentPrice++;

	if (EnemyController)
	{
		EnemyController->GetBlackBoardComponent()->SetValueAsInt(FName("CurrentPrice"), CurrentPrice);
	}
}

void ATrainKingCrown::PlayGiftFireMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance)
	{
		AnimInstance->Montage_Play(Train_GiftFire);
	}
}

void ATrainKingCrown::PlayGroggyMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance)
	{
		AnimInstance->Montage_Play(Train_Groggy);
	}
}

void ATrainKingCrown::PlayTrainReverseTimeline()
{
	StartTimeline.ReverseFromEnd();
}

void ATrainKingCrown::Accept(VisitorPtr Visitor)
{
	Visitor->Visit(this);
}
