// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Boss/KingCrown.h"
#include "CinematicKingCrown.generated.h"

UENUM(BlueprintType)
enum class ECinematicKingCrown : uint8
{
	ECKC_IDLE				UMETA(DisplayName = "Idle"),
	ECKC_PUNCH				UMETA(DisplayName = "Punch"),
	ECKC_PUNCHING				UMETA(DisplayName = "Punching"),
	ECKC_HITTING				UMETA(DisplayName = "Hitting"),
	ECKC_MAX				UMETA(DisplayName = "Max"),
	
};


UCLASS()
class PROJECTHIT_API ACinematicKingCrown : public AKingCrown
{
	GENERATED_BODY()

public:
	ACinematicKingCrown();

	void Accept(VisitorPtr Visitor) override;

	virtual void PunchPatternStart() override;

	virtual void Tick(float DeltaTime);

	UFUNCTION(BlueprintCallable)
	void SetBoolPunchStart();

	//void TimeChecking();

	float ReturnPunchTimer();

	float ReturnPunchDistance();
	
protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	virtual void PunchSpawn() override;

	virtual void PunchPatternEnd() override;

	virtual void GroggyPatternStart_120() override;
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CinematicBoss | Enum", meta = (AllowPrivateAccess = "true"))
	ECinematicKingCrown CinematicKingCrown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CinematicBoss | Montage", meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* CinematicPunch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CinematicBoss | Montage", meta = (AllowPrivateAccess = "true"))
	 UAnimMontage* CinematicCrash;



	// FTimerDelegate TestBinding;
public:
	FORCEINLINE void SetCinematicKingCrown(ECinematicKingCrown Phase) { CinematicKingCrown = Phase; }
	FORCEINLINE ECinematicKingCrown GetCinematicKingCrown() const { return CinematicKingCrown; }

	void SetCinematicPatternEnum(ECinematicKingCrown Pattern);
};
