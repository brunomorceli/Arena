// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityWarrior1.h"
#include "GlobalLibrary.h"

AAbilityWarrior1::AAbilityWarrior1()
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

void AAbilityWarrior1::BeginPlay()
{
	Super::BeginPlay();

	CommitAnimation.AnimSequence = UGlobalLibrary::GetAnimSequence(3);
	CommitAnimation.bLoop = true;

	CommitAnimation.RightHandTrail = UGlobalLibrary::GetTrail(1);
	CommitAnimation.TrailDelay = 0.3f;
	CommitAnimation.TrailDuration = 1.0f;
}

void AAbilityWarrior1::SetupModifiers()
{
	Super::SetupModifiers();

	UWorld* World = GetWorld();
	if (!World) { return; }

	FDamageModifier Damage;
	Damage.AbilityOwner = this;
	Damage.Icon = Icon;
	Damage.Health = 350.0f;
	Damage.StartParticle = UGlobalLibrary::GetParticle(4);
	DamageModifiers.Add(Damage);
}
