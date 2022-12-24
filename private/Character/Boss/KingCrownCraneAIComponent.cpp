// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Boss/KingCrownCraneAIComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values for this component's properties
UKingCrownCraneAIComponent::UKingCrownCraneAIComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UKingCrownCraneAIComponent::BeginPlay()
{
	Super::BeginPlay();
	ACharacter* Character = Cast<ACharacter>(this->GetOwner());
	Character->GetCharacterMovement()->GravityScale = 0;
	
	// ...
	
}


// Called every frame
void UKingCrownCraneAIComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

