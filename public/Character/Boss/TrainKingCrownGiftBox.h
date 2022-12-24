// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/Actor.h"
#include "Core/Trigger/EnemySpawner.h"
#include "TrainKingCrownGiftBox.generated.h"

UCLASS()
class PROJECTHIT_API ATrainKingCrownGiftBox : public AEnemySpawner
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATrainKingCrownGiftBox();

	UFUNCTION()
	void TimelineProgress(float Value);

	UFUNCTION()
	void EndTimelineProgress();

	FVector GetLocationBezier(FVector StartPoint, FVector Anchor, FVector EndPoint, float Time);

	virtual void OnConstruction(const FTransform& Transform) override;

	void DestroyBallonMesh();

	UFUNCTION(BlueprintCallable)
	void SpawnMonster();

	UFUNCTION()
	void GiftBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void FindPlayer();

	//UFUNCTION()
	void ActorDestroyPlus(AActor* actor, const EEndPlayReason::Type EndPlayReason);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FTimeline CurveFTimeline;

	UPROPERTY(EditAnywhere,Category = "Timeline")
	class UCurveFloat* CurveFloat;

	UPROPERTY(EditAnywhere, meta = (MakeEditWidget = "true"))
	FVector Start;

	UPROPERTY(EditAnywhere, meta = (MakeEditWidget = "true"))
	float StartZOffset;

	UPROPERTY(EditAnywhere, meta = (MakeEditWidget = "true"))
	FVector ControlAnchor;

	UPROPERTY(EditAnywhere, meta = (MakeEditWidget = "true"))
	FVector End;

	FVector EndTemp;

	FVector EndResult;

	FVector MyLocation;

	UPROPERTY(EditAnywhere, Category = "Timeline")
		class UCurveVector* CurveVector;

	FVector StartToEndPosOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<AActor> Train;

	float TrainStartYaw;

	void FindKingCrown();

	void SpawnStart();


private:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Class", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* GiftBoxCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Class", meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* GiftMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Class", meta = (AllowPrivateAccess = "true"))
	class ATrainKingCrown* KingCrown;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	class UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "class", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* BalloonMesh;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "class", meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* HealthBarWidget;

	class UMaterialInstanceDynamic* DynamicZeroMat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials ", meta = (AllowPrivateAccess = "true"))
	class UMaterialInstance* ZeroMatInstance;

	 UMaterialInstanceDynamic* DynamicOneMat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials ", meta = (AllowPrivateAccess = "true"))
	UMaterialInstance* OneMatInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particles", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* BoomParticle;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Particles", meta = (AllowPrivateAccess = "true"))
	class UFMODEvent* FMOD_Balloon;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Particles", meta = (AllowPrivateAccess = "true"))
	class UFMODEvent* FMOD_Back;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Particles", meta = (AllowPrivateAccess = "true"))
		class UFMODEvent* FMOD_Death;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particles", meta = (AllowPrivateAccess = "true"))
	FVector BallonVector;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Montage", meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* MoveMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particles", meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* DestroyParticle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particles", meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* DeathParticle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particles", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* PS_CharSlash;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Class", meta = (AllowPrivateAccess = "true"))
	class APlayerCharacter* Player;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particles", meta = (AllowPrivateAccess = "true"))
	UParticleSystemComponent* TrailParticle;

	int EndIndex;

	TArray<FVector> SocketControlAnchor;
	int AnchorNumber;	

	UPROPERTY()
	bool bAnimStart;

	int AnimCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties", meta = (AllowPrivateAccess = "true"))
	float MaxGiftHp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties", meta = (AllowPrivateAccess = "true"))
	float GiftHp;
	
	float CurveTimer;

	bool ArriveEnd;

	FTimerHandle DestoryTimerHandle;

	FTimerHandle OffDamageTimerHandle;

	bool bFireSelected;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "FMOD", meta = (AllowPrivateAccess = "true"))
	class UFMODEvent* FMOD_GiftHit;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	FORCEINLINE void SetStartPoint(FVector Start_Point) { Start = Start_Point; }
	FORCEINLINE void SetAnchorPoint(FVector AnchorPoint) { ControlAnchor = AnchorPoint; }
	FORCEINLINE void SetEndPoint(FVector End_Point) { End = End_Point; }
	FORCEINLINE FVector GetEndPoint() const{ return End; }
};
