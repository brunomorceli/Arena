// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityAssassin2.h"
#include "GlobalLibrary.h"
#include "ArenaCharacter.h"

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

	Damage.OnTickHandler = [](FAbilityInfo AbilityInfo)
	{
		if (AbilityInfo.Amount <= 0.0f) { return; }

		AArenaCharacter* Causer = Cast<AArenaCharacter>(AbilityInfo.Causer);
		if (!Causer) { return; }

		FHealModifier Heal;
		Heal.AbilityOwner = AbilityInfo.Ability;
		Heal.bAllowSelf = true;
		Heal.bAllowEnemy = false;
		Heal.bAllowTeam = false;
		Heal.Health = AbilityInfo.Amount;
		Heal.Icon = AbilityInfo.ModifierIcon;

		Causer->ApplyHealModifier(Heal);
	};

	DamageModifiers.Add(Damage);
}
