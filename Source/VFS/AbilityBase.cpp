// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityBase.h"
#include "GlobalLibrary.h"

// Sets default values
AAbilityBase::AAbilityBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	bAlwaysRelevant = true;
	bNetUseOwnerRelevancy = true;
	bNetLoadOnClient = true;

	CommitFX = NULL;
	CastFX = NULL;

	Slot = 0;
	Name = FName("No Name");
	Description = "No Description";
	CountdownTime = 0.0f;
	CountdownTimeRemaining = 0.0;
	CastTime = 0.0f;
	bCastInMovement = false;

	HealthCost = 0.0f;
	ManaCost = 0.0f;
	EnergyCost = 0.0f;
	bIsPercent = false;

	ChannelingTime = 0.5f;
	ChannelingTotalTime = 0.0f;
	MinDistance = 0.0f;
	MaxDistance = 800.0f;
	MaxAngle = 180.0f;

	DirectionalRadius = 50.0f;
	DirectionalRange = 100.0f;

	bAllowSelf = false;
	bAllowTeam = false;
	bAllowEnemy = true;

	SchoolType = ABS_Physical;
	AbilityType = ABST_Instant;
	AreaType = ABA_Target;
	CommitType = ABC_Instant;
	ProjectileSpeed = 1200.0f;
	AuraModifiers = TArray<FAuraModifier>();
	BuffModifiers = TArray<FBuffModifier>();
	AbsorbingModifiers = TArray<FAbsorbingModifier>();
	OvertimeModifiers = TArray<FOvertimeModifier>();
	HealModifiers = TArray<FHealModifier>();
	DamageModifiers = TArray<FDamageModifier>();

	// Default texture.
	LoadIcon("/Game/Sprites/Icons/0.0");
}

void AAbilityBase::BeginPlay()
{
	Super::BeginPlay();
	SetupModifiers();
}

// Called every frame
void AAbilityBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateTimers(DeltaTime);
}

void AAbilityBase::UpdateTimers(float DeltaTime)
{
	if (!IsCountdown()) { return; }
	CountdownTimeRemaining = FMath::Clamp(CountdownTimeRemaining - DeltaTime, 0.0f, CountdownTime);
}

void AAbilityBase::SetupModifiers() {}

void AAbilityBase::StartCountdown()
{
	CountdownTimeRemaining = CountdownTime;
}

void AAbilityBase::StopCountdown()
{
	CountdownTimeRemaining = 0.0f;
}

void AAbilityBase::LoadIcon(const FString& Path)
{
	Icon = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, *(Path)));
}

bool AAbilityBase::IsCountdown()
{
	return CountdownTimeRemaining > 0.0f;
}

float AAbilityBase::GetCountdownRemaining()
{
	return CountdownTimeRemaining;
}

float AAbilityBase::GetCountdownPercent()
{
	float Elapsed = CountdownTime - CountdownTimeRemaining;
	return (Elapsed * 100.0f) / CountdownTime;
}

void AAbilityBase::ClearChanneling()
{
	ChannelingTotalTime = 0.0f;
}

bool AAbilityBase::UpdateChanneling(float DeltaTime)
{
	ChannelingTotalTime += DeltaTime;

	if (ChannelingTotalTime >= ChannelingTime)
	{
		ChannelingTotalTime = FMath::Fmod(ChannelingTotalTime, ChannelingTime);
		return true;
	}

	return false;
}

void AAbilityBase::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}
