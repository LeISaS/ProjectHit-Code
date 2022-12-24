// Fill out your copyright notice in the Description page of Project Settings.

#include "../../Public/CharacterCore/AnimAttackNotifyState.h"
#include "../../Public/Character/PlayerCharacter/PlayerCharacter.h"
#include "../../Public/Character/Enemy/Enemy.h"

void UAnimAttackNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime);

	APlayerCharacter* Player = Cast<APlayerCharacter>(MeshComp->GetOwner());
	/*if (Player)
	{
		Player->AttackCheck();
	}*/
}