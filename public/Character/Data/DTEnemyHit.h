// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "DTEnemyHit.generated.h"

USTRUCT(BlueprintType)
struct FEnemyHitInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float KnockBackPower;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float KnockDownPower;
};


USTRUCT(BlueprintType)
struct FEnemyName : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FEnemyHitInfo Toymon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FEnemyHitInfo Forky;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FEnemyHitInfo Uder;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FEnemyHitInfo Bomber;	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FEnemyHitInfo Doggy;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FEnemyHitInfo KingCrown_2Phase;
};

UCLASS()
class PROJECTHIT_API UDTEnemyHit : public UDataTable
{
	GENERATED_BODY()

};
