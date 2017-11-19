// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityWarrior6.h"
#include "GlobalLibrary.h"

AAbilityWarrior6::AAbilityWarrior6()
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

	LoadIcon("/Game/Sprites/Icons/305.305");
}

void AAbilityWarrior6::BeginPlay()
{
	Super::BeginPlay();
}

void AAbilityWarrior6::SetupModifiers()
{
	Super::SetupModifiers();

	UWorld* World = GetWorld();
	if (!World) { return; }

	FDamageModifier Damage;
	Damage.AbilityOwner = this;
	Damage.Icon = Icon;
	Damage.Health = 100.0f;
	DamageModifiers.Add(Damage);
}
