// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityAssassin8.h"
#include "GlobalLibrary.h"

AAbilityAssassin8::AAbilityAssassin8()
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

	LoadIcon("/Game/Sprites/Icons/401.401");
}

void AAbilityAssassin8::BeginPlay()
{
	Super::BeginPlay();
}

void AAbilityAssassin8::SetupModifiers()
{
	Super::SetupModifiers();

	UWorld* World = GetWorld();
	if (!World) { return; }

	FDamageModifier Damage;
	Damage.AbilityOwner = this;
	Damage.Icon = Icon;
	Damage.Health = 350.0f;
	DamageModifiers.Add(Damage);
}
