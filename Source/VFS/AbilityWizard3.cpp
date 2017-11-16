// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityWizard3.h"
#include "GlobalLibrary.h"

AAbilityWizard3::AAbilityWizard3()
{
	PrimaryActorTick.bCanEverTick = true;

	Name = "Push";
	Description = "Nothing yet.";

	MaxDistance = 300.0f;

	AbilityType = ABST_Instant;

	AreaType = ABA_Target;

	bAllowEnemy = true;
	bAllowSelf = false;
	bAllowTeam = false;

	LoadIcon("/Game/Sprites/Icons/228.228");
}

void AAbilityWizard3::BeginPlay()
{
	Super::BeginPlay();

	CommitAnimation.AnimSequence = UGlobalLibrary::GetAnimSequence(3);
	CommitAnimation.bLoop = true;

	CommitAnimation.RightHandTrail = UGlobalLibrary::GetTrail(1);
	CommitAnimation.TrailDelay = 0.3f;
	CommitAnimation.TrailDuration = 1.0f;
}

void AAbilityWizard3::SetupModifiers()
{
	Super::SetupModifiers();

	UWorld* World = GetWorld();
	if (!World) { return; }

	FDamageModifier Damage;
	Damage.AbilityOwner = this;
	Damage.Icon = Icon;
	Damage.Health = 100.0f;
	Damage.StartParticle = UGlobalLibrary::GetParticle(4);
	DamageModifiers.Add(Damage);

	FBuffModifier Stun;
	Stun.AbilityOwner = this;
	Stun.Icon = Icon;
	Stun.State = CS_Stun;
	Stun.TimeRemaining = 2.0f;
	Stun.StartParticle = UGlobalLibrary::GetParticle(5);

	BuffModifiers.Add(Stun);
}
