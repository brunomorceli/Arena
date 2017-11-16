// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityCleric1.h"
#include "GlobalLibrary.h"

AAbilityCleric1::AAbilityCleric1()
{
	PrimaryActorTick.bCanEverTick = true;

	CountdownTime = 2.0f;
	Name = "Heal or Damage";
	Description = "Nothing yet.";

	MaxDistance = 3000.0f;

	AbilityType = ABST_Channeling;
	CastTime = 2.0f;
	ChannelingTime = 0.5f;

	AreaType = ABA_Target;

	bAllowEnemy = true;
	bAllowTeam = true;
	bAllowSelf = true;

	LoadIcon("/Game/Sprites/Icons/285.285");
}

void AAbilityCleric1::BeginPlay()
{
	Super::BeginPlay();

	CommitAnimation.AnimSequence = UGlobalLibrary::GetAnimSequence(3);
	CommitAnimation.bLoop = true;

	CommitAnimation.RightHandTrail = UGlobalLibrary::GetTrail(1);
	CommitAnimation.TrailDelay = 0.3f;
	CommitAnimation.TrailDuration = 1.0f;
}

void AAbilityCleric1::SetupModifiers()
{
	Super::SetupModifiers();

	UWorld* World = GetWorld();
	if (!World) { return; }

	FDamageModifier Damage;
	Damage.AbilityOwner = this;
	Damage.Icon = Icon;
	Damage.Health = 150.0f;
	Damage.StartParticle = UGlobalLibrary::GetParticle(5);
	Damage.bAllowEnemy = true;
	Damage.bAllowTeam = false;
	Damage.bAllowSelf = false;

	DamageModifiers.Add(Damage);

	FHealModifier Heal;
	Heal.AbilityOwner = this;
	Heal.Icon = Icon;
	Heal.Health = 150.0f;
	Heal.StartParticle = UGlobalLibrary::GetParticle(7);
	Heal.bAllowEnemy = false;
	Heal.bAllowTeam = true;
	Heal.bAllowSelf = true;

	HealModifiers.Add(Heal);
}
