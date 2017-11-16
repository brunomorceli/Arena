// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityWizard4.h"
#include "GlobalLibrary.h"

AAbilityWizard4::AAbilityWizard4()
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

void AAbilityWizard4::BeginPlay()
{
	Super::BeginPlay();
}

void AAbilityWizard4::SetupModifiers()
{
	Super::SetupModifiers();

	UWorld* World = GetWorld();
	if (!World) { return; }

	FDamageModifier Damage;
	Damage.AbilityOwner = this;
	Damage.Icon = Icon;
	Damage.Health = 120.0f;
	Damage.StartParticle = UGlobalLibrary::GetParticle(4);
	DamageModifiers.Add(Damage);
}
