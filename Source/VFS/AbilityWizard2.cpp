// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityWizard2.h"
#include "GlobalLibrary.h"

AAbilityWizard2::AAbilityWizard2()
{
	PrimaryActorTick.bCanEverTick = true;

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

void AAbilityWizard2::BeginPlay()
{
	Super::BeginPlay();

	CommitAnimation.AnimSequence = UGlobalLibrary::GetAnimSequence(3);
	CommitAnimation.bLoop = true;

	CommitAnimation.RightHandTrail = UGlobalLibrary::GetTrail(1);
	CommitAnimation.TrailDelay = 0.3f;
	CommitAnimation.TrailDuration = 1.0f;
}

void AAbilityWizard2::SetupModifiers()
{
	Super::SetupModifiers();

	UWorld* World = GetWorld();
	if (!World) { return; }

	FDamageModifier Damage;
	Damage.AbilityOwner = this;
	Damage.Icon = Icon;
	Damage.Health = 250.0f;
	Damage.StartParticle = UGlobalLibrary::GetParticle(4);
	DamageModifiers.Add(Damage);
}
