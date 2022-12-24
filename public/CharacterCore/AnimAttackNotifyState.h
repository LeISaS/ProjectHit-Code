// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimAttackNotifyState.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTHIT_API UAnimAttackNotifyState : public UAnimNotifyState
{
	GENERATED_BODY()
public:
	
private:
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime) override;


};
