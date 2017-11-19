// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityWarrior5.h"
#include "GlobalLibrary.h"

AAbilityWarrior5::AAbilityWarrior5()
{
	PrimaryActorTick.bCanEverTick = true;

	Name = "Sword Strike";
	Description = "Nothing yet.";

	MaxDistance = 300.0f;

	AbilityType = ABST_Instant;

	AreaType = ABA_Target;

	bAllowEnemy = true;
	bAllowSelf = false;
	bAllowTeam = false;

	LoadIcon("/Game/Sprites/Icons/157.157");
}

void AAbilityWarrior5::BeginPlay()
{
	Super::BeginPlay();
	CommitFX = UGlobalLibrary::GetAbilityUseFX(5);
}

void AAbilityWarrior5::SetupModifiers()
{
	Super::SetupModifiers();

	UWorld* World = GetWorld();
	if (!World) { return; }

	FDamageModifier Damage;
	Damage.AbilityOwner = this;
	Damage.Icon = Icon;
	Damage.Health = 100.0f;
	Damage.bIsHarmful = true;
	DamageModifiers.Add(Damage);

	FBuffModifier PhysicalDebuff;
	PhysicalDebuff.AbilityOwner = this;
	PhysicalDebuff.PhysicalDefense = -15.0f;
	PhysicalDebuff.School = MS_Physical;
	PhysicalDebuff.bAllowSelf = false;
	PhysicalDebuff.bAllowTeam = false;
	PhysicalDebuff.bAllowEnemy = true;
	PhysicalDebuff.bIsHarmful = true;
	PhysicalDebuff.TimeRemaining = 10.0f;

	BuffModifiers.Add(PhysicalDebuff);
}
