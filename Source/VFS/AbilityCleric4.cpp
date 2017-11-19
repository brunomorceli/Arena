// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityCleric4.h"
#include "GlobalLibrary.h"

AAbilityCleric4::AAbilityCleric4()
{
	PrimaryActorTick.bCanEverTick = true;

	Name = "Shield Slam";
	Description = "Nothing yet.";

	MaxDistance = 300.0f;

	AbilityType = ABST_Instant;

	AreaType = ABA_Target;

	bAllowEnemy = true;
	bAllowSelf = false;
	bAllowTeam = false;

	LoadIcon("/Game/Sprites/Icons/19.19");
}

void AAbilityCleric4::BeginPlay()
{
	Super::BeginPlay();
}

void AAbilityCleric4::SetupModifiers()
{
	Super::SetupModifiers();

	UWorld* World = GetWorld();
	if (!World) { return; }

	FDamageModifier Damage;
	Damage.AbilityOwner = this;
	Damage.Icon = Icon;
	Damage.Health = 120.0f;
	DamageModifiers.Add(Damage);
}
