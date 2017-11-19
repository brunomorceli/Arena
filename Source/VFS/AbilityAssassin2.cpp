// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityAssassin2.h"
#include "GlobalLibrary.h"

AAbilityAssassin2::AAbilityAssassin2()
{
	PrimaryActorTick.bCanEverTick = true;

	CountdownTime = 5.0f;
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

void AAbilityAssassin2::BeginPlay()
{
	Super::BeginPlay();
}

void AAbilityAssassin2::SetupModifiers()
{
	Super::SetupModifiers();

	UWorld* World = GetWorld();
	if (!World) { return; }

	FDamageModifier Damage;
	Damage.AbilityOwner = this;
	Damage.Icon = Icon;
	Damage.Health = 250.0f;
	DamageModifiers.Add(Damage);
}
