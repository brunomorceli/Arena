// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityWarrior2.h"
#include "GlobalLibrary.h"

AAbilityWarrior2::AAbilityWarrior2()
{
	PrimaryActorTick.bCanEverTick = true;

	Name = "Slash Attack";
	Description = "Nothing yet.";

	AbilityType = ABST_Instant;
	AreaType = ABA_Directional;
	DirectionalRadius = 100.0f;
	DirectionalRange = 200.0f;

	bAllowEnemy = true;
	bAllowSelf = false;
	bAllowTeam = false;

	LoadIcon("/Game/Sprites/Icons/452.452");
}

void AAbilityWarrior2::BeginPlay()
{
	Super::BeginPlay();
	CommitFX = UGlobalLibrary::GetAbilityUseFX(2);
}

void AAbilityWarrior2::SetupModifiers()
{
	Super::SetupModifiers();

	UWorld* World = GetWorld();
	if (!World) { return; }

	FDamageModifier Damage;
	Damage.AbilityOwner = this;
	Damage.Icon = Icon;
	Damage.Health = 100.0f;

	Damage.OnApplyHandler = [](FAbilityInfo AbilityInfo) {
		AbilityInfo.Target->ServerKnockBack(AbilityInfo.Causer->GetActorLocation(), 1200.0f);
		AbilityInfo.Target->MulticastPlayFX(UGlobalLibrary::GetAbilityHitFX(2));
	};

	DamageModifiers.Add(Damage);

	FOvertimeModifier Overtime;
	Overtime.AbilityOwner = this;
	Overtime.Icon = Icon;
	Overtime.Health = 50.0f;
	Overtime.TickTime = 1.0f;
	Overtime.TimeRemaining = 10.0f;
	Overtime.bIsHarmful = true;

	Overtime.OnTickHandler = [](FAbilityInfo AbilityInfo) {
		AbilityInfo.Target->MulticastPlayFX(UGlobalLibrary::GetAbilityHitFX(2));
	};

	OvertimeModifiers.Add(Overtime);
}
