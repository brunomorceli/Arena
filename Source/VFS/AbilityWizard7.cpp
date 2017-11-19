// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityWizard7.h"
#include "GlobalLibrary.h"

AAbilityWizard7::AAbilityWizard7()
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

	LoadIcon("/Game/Sprites/Icons/475.475");
}

void AAbilityWizard7::BeginPlay()
{
	Super::BeginPlay();
}

void AAbilityWizard7::SetupModifiers()
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
