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
	Damage.ModifierCritical = 50.0f;
	DamageModifiers.Add(Damage);

	FBuffModifier PhysicalDebuff;
	PhysicalDebuff.AbilityOwner = this;
	PhysicalDebuff.Icon = Icon;
	PhysicalDebuff.Name = "Debuff";
	PhysicalDebuff.MaxStacks = 2;
	PhysicalDebuff.PhysicalDefense = 20.0f;
	PhysicalDebuff.School = MS_Physical;
	PhysicalDebuff.bAllowSelf = false;
	PhysicalDebuff.bAllowTeam = false;
	PhysicalDebuff.bAllowEnemy = true;
	PhysicalDebuff.bIsHarmful = true;
	PhysicalDebuff.TimeRemaining = 5.0f;

	BuffModifiers.Add(PhysicalDebuff);

	FBuffModifier PhysicalBuff;
	PhysicalBuff.AbilityOwner = this;
	PhysicalBuff.Icon = UGlobalLibrary::GetIcon(453);
	PhysicalBuff.Name = "Buff";
	PhysicalBuff.MaxStacks = 2;
	PhysicalBuff.PhysicalPower = 20.0f;
	PhysicalBuff.School = MS_Physical;
	PhysicalBuff.bAllowSelf = true;
	PhysicalBuff.bAllowTeam = false;
	PhysicalBuff.bAllowEnemy = false;
	PhysicalBuff.bIsHarmful = false;
	PhysicalBuff.bIsDispellable = false;
	PhysicalBuff.TimeRemaining = 5.0f;

	BuffModifiers.Add(PhysicalBuff);
}
