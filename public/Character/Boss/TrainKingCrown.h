// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Boss/KingCrown.h"
#include "Components/SplineComponent.h"
#include "Components/TimelineComponent.h"
#include "TrainKingCrown.generated.h"

UENUM(BlueprintType)
enum class ETrainBossState :uint8
{
	ETBS_START		UMETA(DisplayName = "Start"),
	ETBS_ONE		UMETA(DisplayName = "One"),  //트리거 
	ETBS_TWO		UMETA(DisplayName = "Two"),
	ETBS_THREE		UMETA(DisplayName = "Three"),
	ETBS_FOUR		UMETA(DisplayName = "Four"),
	ETBS_FIVE		UMETA(DisplayName = "Five"),
	ETBS_SIX		UMETA(DisplayName = "Six"),
	ETBS_SEVEN		UMETA(DisplayName = "Seven"),
	ETBS_END		UMETA(DisplayName = "End"),    //트리거 
						  
	ETBS_MAX		UMETA(DisplayName = "Max")
};


/**
 * 
 */
UCLASS()
class PROJECTHIT_API ATrainKingCrown : public AKingCrown
{
	GENERATED_BODY()


public:
	ATrainKingCrown();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void OnConstruction(const FTransform& Transform) override;


	UFUNCTION()
	FVector GetBoxPoint(int index);

	UFUNCTION(BlueprintCallable)
	void ShotGiftBox(int StartNumber, int EndNumber, bool FireSelected);

	void Accept(VisitorPtr Visitor) override;

	void CurrentPricePlus(int number);

	UFUNCTION()
	void ChildCurrentPricePlus();

	UFUNCTION(BlueprintCallable)
	void PlayGiftFireMontage();

	UFUNCTION(BlueprintCallable)
	void PlayGroggyMontage();

	UFUNCTION(BlueprintCallable)
	void PlayTrainReverseTimeline();
protected:
	UFUNCTION()
	void MovingMesh(float Value);

	UFUNCTION()
	void StartMovingMesh(float Value);

	UFUNCTION()
	void EndMovingMesh();

	UFUNCTION()
	void StartEndMovingMesh();

	UFUNCTION(BlueprintCallable)
	void SetEndPrice(int Price);

	UFUNCTION()
	void FianlMovingMesh(float Value);

	
	
private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Properties", meta = (AllowPrivateAccess = "true"))
	int CurrentPrice;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Properties", meta = (AllowPrivateAccess = "true"))
	int EndPrice;

	int CurrentShotNumber;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Train | Class", meta = (AllowPrivateAccess = "true"))
	class USplineComponent*SplineComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Train | Class", meta = (AllowPrivateAccess = "true"))
	USplineComponent* StartSplineComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Train | Class", meta = (AllowPrivateAccess = "true"))
	USplineComponent* FinalSplineComponent;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Train | Class", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* PointBox_1;
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Train | Class", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* PointBox_2;
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Train | Class", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* PointBox_3;
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Train | Class", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* PointBox_4;
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Train | Class", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* PointBox_5;
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Train | Class", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* PointBox_6;
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Train | Class", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* PointBox_7;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Train | Spline", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* MoveCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Train | Spline", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* StartCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Train | Spline", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* FinalCurve;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Train | Spline", meta = (AllowPrivateAccess = "true"))
	bool bReverse;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Train | Spline", meta = (AllowPrivateAccess = "true"))
	FName FireSocket;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Train | Class", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class ATrainKingCrownGiftBox> TrainGiftBoxClass;

	ATrainKingCrownGiftBox* TrainGiftBox;

	FTimerHandle TestTimer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Train | Class", meta = (AllowPrivateAccess = "true"))
	TArray<UBoxComponent*> ArrPointBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Train | Enum", meta = (AllowPrivateAccess = "true"))
	ETrainBossState TrainBossState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Train | Montage", meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* Train_GiftFire;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Train | Montage", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* Train_Groggy;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Train | FMOD", meta = (AllowPrivateAccess = "true"))
		class UFMODEvent* FMOD_GiftFire;

protected:
	FVector StartingSplineLocation;
	FVector CurrentSplineLocation;
	FRotator CurrentSplineRotation;

	FTimeline MoveTimeline;	
	FTimeline StartTimeline;
	FTimeline FinalTimeline;


	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Properties", meta = (AllowPrivateAccess = "true"))
	FVector FireSocketScale;

	int GiftStartNumber;
	int GiftEndNumber;
	bool bFireSelected;

	FVector FinalPatternPos;
public:
	UFUNCTION(BlueprintCallable)
	void SetBoolGiftPatternStart();

	UFUNCTION(BlueprintCallable)
	void SetTrainStateEnum(ETrainBossState State);

	FORCEINLINE void SetTrainBossState(ETrainBossState State) { TrainBossState = State; }
	FORCEINLINE ETrainBossState GetTrainBossState() const { return TrainBossState; }
	
	FORCEINLINE int GetTrainGiftStartNumber() const { return GiftStartNumber; }
	FORCEINLINE int GetTrainGiftEndNumber() const { return GiftEndNumber; }
	FORCEINLINE bool GetTrainFireSelected() const { return bFireSelected; }
};
