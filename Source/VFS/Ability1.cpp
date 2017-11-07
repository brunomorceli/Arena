// Fill out your copyright notice in the Description page of Project Settings.

#include "Ability1.h"
#include "GlobalLibrary.h"

AAbility1::AAbility1()
{
	PrimaryActorTick.bCanEverTick = true;

	CountdownTime = 2.0f;
	Name = "First Blood";
	Description = "Nothing yet.";

	ManaCost = 30.0f;
	MaxDistance = 3000.0f;

	AbilityType = ABST_Castable;
	CommitType = ABC_Projectile;
	CastTime = 2.0f;

	LoadIcon("/Game/Sprites/Icons/308.308");
}

void AAbility1::BeginPlay()
{
	Super::BeginPlay();

	CastParticle = UGlobalLibrary::GetParticle(1);
}

void AAbility1::SetupModifiers()
{
	Super::SetupModifiers();

	UWorld* World = GetWorld();
	if (!World) { return; }

	FDamageModifier Damage;
	Damage.AbilityOwner = this;
	Damage.Icon = Icon;
	Damage.Health = 500.0f;
	DamageModifiers.Add(Damage);

	Projectile = UGlobalLibrary::GetProjectile(1);
	ProjectileHitParticle = UGlobalLibrary::GetParticle(6);
}
