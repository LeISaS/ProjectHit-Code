#include "Character/Boss/KingCrown.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "../../../Public/Character/Enemy/EnemyController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/Boss/KingCrownPunch.h"

AKingCrown::AKingCrown() :
	DeathParticleZ(2500.f),
	bPatternInterpRot(false),
	GlassTwoHp(300.f),
	GlassThreeHp(120.f)
{
	PrimaryActorTick.bCanEverTick = true;

	MainCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("MainCollision"));
	MainCollision->SetupAttachment(GetMesh(), FName("MainCollision"));

	RocketParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("RocketParticle"));
	RocketParticle->SetupAttachment(GetMesh(), FName("WindCollision"));

	PS_Wind = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("PS_Wind"));
	PS_Wind->SetupAttachment(GetMesh(), FName("WindCollision"));

	PS_KingCrownCrash = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("PS_KingCrownCrash"));
	PS_KingCrownCrash->SetupAttachment(GetRootComponent());

	PS_PunchFire = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("PS_PunchFire"));
	PS_PunchFire->SetupAttachment(GetMesh(), FName("PunchFireSocket"));

	PS_PunchDoor = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("PS_PunchDoor"));
	PS_PunchDoor->SetupAttachment(GetMesh(), FName("PunchFireSocket"));
	bAttackRotChar = false;
}


void AKingCrown::BeginPlay()
{
	Super::BeginPlay();

	SetPullType(EPullType::EPT_DISALLOW);

	MainCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	MainCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
	MainCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel5, ECollisionResponse::ECR_Block);
	MainCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Block);
	MainCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);
	MainCollision->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel3);

	RocketParticle->Activate();
	PS_Wind->Activate();

	PS_KingCrownCrash->Deactivate();

	SetLEDMat(1.f, 1.f, 0.f, 0.f, 0.f);
	SetFacialMat(0);
	SetMIDScalarParameterValue(DynamicFourMat, FName("Facial Expression Selector"), 0);

}

void AKingCrown::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bPatternInterpRot)
	{
		FRotator LookAtYaw = GetLookAtRotationYaw(GetPlayerLocation());
		auto InterpRot = FMath::RInterpTo(GetActorRotation(), LookAtYaw, DeltaTime, 5.0f);
		SetActorRotation(InterpRot);
	}
}

void AKingCrown::OnConstruction(const FTransform& Transform)
{
	//Super::OnConstruction(Transform);

	DynamicZeroMat = SetDynamicMat(ZeroMatInstance, DynamicZeroMat, 0, this);		//KingCrown_CraneBox
	DynamicOneMat = SetDynamicMat(OneMatInstance, DynamicOneMat, 1, this);			//kingCrown_Glass	
	DynamicTwoMat = SetDynamicMat(TwoMatInstance, DynamicTwoMat, 2, this);			//KingCronw_LED	
	DynamicThreeMat = SetDynamicMat(ThreeMatInstance, DynamicThreeMat, 4, this);			//KingCrown_Body
	DynamicFourMat = SetDynamicMat(FourMatInstance, DynamicFourMat, 5, this);			//KingCrown_Face	
}

float AKingCrown::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	OnDamageColorMat(DynamicZeroMat, FName("DamageColorSwitch"), MatTimer_0);
	OnDamageColorMat(DynamicThreeMat, FName("DamageColorSwitch"), MatTimer_0);
	OnDamageColorMat(DynamicFourMat, FName("DamageColorSwitch"), MatTimer_0);

	SetLEDMat(1.f, 1.f, 1.f, 0.f, 1.f);

	auto KingCrownLED = this;

	FTimerDelegate LEDTimerCallback;
	LEDTimerCallback.BindLambda([KingCrownLED]() {
		if(IsValid(KingCrownLED))
			KingCrownLED->SetLEDMat(1.f, 1.f, 1.f, 1.f, 0.f);
		});

	GetWorldTimerManager().SetTimer(LEDTimer, LEDTimerCallback, 0.35f, false);


	if (GetHp() <= GlassTwoHp && GetHp() > GlassThreeHp)
	{
		DynamicOneMat->SetVectorParameterValue("GlassCrack Mask", FLinearColor(0.f, 1.f, 1.f, 0.f));

		UGameplayStatics::SpawnEmitterAtLocation(this, PS_Glass_2,
			FVector(GetActorLocation().X + PS_Glass_2Location.X, GetActorLocation().Y + PS_Glass_2Location.Y, GetActorLocation().Z + PS_Glass_2Location.Z),
			FRotator(GetActorRotation().Pitch, GetActorRotation().Yaw, GetActorRotation().Roll), FVector(1.f, 1.f, 1.f));

	}
	else if (GetHp() <= GlassThreeHp)
	{
		DynamicOneMat->SetVectorParameterValue("GlassCrack Mask", FLinearColor(1.f, 1.f, 1.f, 0.f));

		UGameplayStatics::SpawnEmitterAtLocation(this, PS_Glass_3,
			FVector(GetActorLocation().X + PS_Glass_3Location.X, GetActorLocation().Y + PS_Glass_3Location.Y, GetActorLocation().Z + PS_Glass_3Location.Z),
			FRotator(GetActorRotation().Pitch, GetActorRotation().Yaw, GetActorRotation().Roll), FVector(1.f, 1.f, 1.f));
	}

	return DamageAmount;
}

void AKingCrown::SetLEDMat(float Idle, float On, float Anger, float HeRong, float Attacked)
{
	SetMIDScalarParameterValue(DynamicTwoMat, FName("0.7Cinema1Idle"), Idle);
	SetMIDScalarParameterValue(DynamicTwoMat, FName("0LEDOff1On"), On);
	SetMIDScalarParameterValue(DynamicTwoMat, FName("0Idle1Anger"), Anger);
	SetMIDScalarParameterValue(DynamicTwoMat, FName("0Idle1HeRong"), HeRong);
	SetMIDScalarParameterValue(DynamicTwoMat, FName("0Idle1Attacked"), Attacked);

}

void AKingCrown::SetFacialMat(float Value)
{
	SetMIDScalarParameterValue(DynamicFourMat, FName("Facial Expression Selector"), Value);
}

void AKingCrown::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (MatTimer_0.IsValid())
		GetWorldTimerManager().ClearTimer(MatTimer_0);

	if (LEDTimer.IsValid())
		GetWorldTimerManager().ClearTimer(LEDTimer);

	Super::EndPlay(EndPlayReason);
}


void AKingCrown::GroggyPatternStart_120()
{	
	if (EnemyController)
	{
		SetLEDMat(1.f, 1.f, 1.f, 1.f, 0.f);
		SetFacialMat(2);
		RocketParticle->Deactivate();
	}
}

void AKingCrown::NotifyCrash()
{
	PS_KingCrownCrash->Activate();
}

void AKingCrown::UnNotifyCrash()
{
	RocketParticle->Activate();
	PS_KingCrownCrash->Deactivate();
}

void AKingCrown::GiftPatternStart()
{	
	SetLEDMat(1.f, 1.f, 1.f, 0.f, 0.f);
	SetFacialMat(1);

	PlayHighPriorityMontage(KingCrown_GiftFireMontage, FName("Start"), 1.0f);
}

void AKingCrown::PunchPatternStart()
{
	SetLEDMat(1.f, 1.f, 1.f, 0.f, 0.f);
	SetFacialMat(1);

	PlayHighPriorityMontage(KingCrown_BeforePunchAnim, FName("Start"), 1.0f);
	
}

void AKingCrown::PunchSpawn()
{
	ClearFoucsSetting();

	if (PunchClass != nullptr)
	{
		if (GetWorld())
		{
			FRotator FireSocketRot = GetMesh()->GetSocketRotation(FName("PunchSocket"));
			FVector FireSocketLoc = GetMesh()->GetSocketLocation(FName("PunchSocket"));

			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;

			Punch = GetWorld()->SpawnActor<AKingCrownPunch>(PunchClass, FireSocketLoc, FireSocketRot, SpawnParams);
		}
	}
}

void AKingCrown::PunchPatternEnd()
{

	if (EnemyController)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

		if (AnimInstance)
		{
			AnimInstance->Montage_Play(GetBeforePunchAnim(), 1.0f);
			AnimInstance->Montage_JumpToSection(FName("PunchEnd"), GetBeforePunchAnim());
		}
	}
}

void AKingCrown::PunchDoorEffect()
{
	GetPS_PunchDoor()->Activate();
	UFMODBlueprintStatics::PlayEvent2D(GetWorld(), FMOD_PunchEnergy, true);
}

void AKingCrown::Die()
{
	Super::Die();
	SetLEDMat(1.f, 1.f, 0.f, 0.f, 1.f);
	SetFacialMat(2);
}


void AKingCrown::DeathStart()
{
	bPatternInterpRot = false;

	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel5, ECollisionResponse::ECR_Block);
}
void AKingCrown::ClearFoucsSetting()
{
	EnemyController->ClearFocus(EAIFocusPriority::LastFocusPriority);
}


void AKingCrown::OnDeathParticle()
{
	if (PS_Death)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, PS_Death,
			FVector(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z + DeathParticleZ),
			FRotator(GetActorRotation().Pitch, GetActorRotation().Yaw, GetActorRotation().Roll), FVector(1.f, 1.f, 1.f));
	}
}

void AKingCrown::PunchParringChangeMat()
{
	OnDamageColorMat(DynamicZeroMat, FName("DamageColorSwitch"), MatTimer_0);
	OnDamageColorMat(DynamicOneMat, FName("DamageColorSwitch"), MatTimer_0);
	OnDamageColorMat(DynamicTwoMat, FName("DamageColorSwitch"), MatTimer_0);
}

void AKingCrown::onInterpRotNotify()
{
	bPatternInterpRot = true;
}
void AKingCrown::offInterpRotNotify()
{
	bPatternInterpRot = false;
}
