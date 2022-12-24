// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TwoPhaseJumpActor.generated.h"

UCLASS()
class PROJECTHIT_API ATwoPhaseJumpActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ATwoPhaseJumpActor();

protected:
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* BoxComponent;
public:	
	FORCEINLINE UBoxComponent* GetBoxComponent(){return BoxComponent;}

};
