// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Boss/TwoPhaseJumpActor.h"
#include "Components/BoxComponent.h"
// Sets default values
ATwoPhaseJumpActor::ATwoPhaseJumpActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	SetRootComponent(BoxComponent);
	
}

