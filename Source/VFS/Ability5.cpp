// Fill out your copyright notice in the Description page of Project Settings.

#include "Ability5.h"
#include "GlobalLibrary.h"

AAbility5::AAbility5()
{
	PrimaryActorTick.bCanEverTick = true;

	Name = "Channeling Attack";
	Description = "Nothing yet.";

	MaxDistance = 300.0f;

	AbilityType = ABST_Instant;

	AreaType = ABA_Target;

	bAllowEnemy = true;
	bAllowSelf = false;
	bAllowTeam = false;

	LoadIcon("/Game/Sprites/Icons/168.168");
}

void AAbility5::BeginPlay()
{
	Super::BeginPlay();
	
	CommitAnimation.AnimSequence = UGlobalLibrary::GetAnimSequence(3);
	CommitAnimation.bLoop = true;

	CommitAnimation.RightHandTrail = UGlobalLibrary::GetTrail(1);
	CommitAnimation.TrailDelay = 0.3f;
	CommitAnimation.TrailDuration = 1.0f;
}

void AAbility5::SetupModifiers()
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


	FBuffModifier Slow;
	Slow.AbilityOwner = this;
	Slow.Icon = Icon;
	Slow.Speed = 300.0f;
	Slow.bIsPercent = false;
	Slow.bIsStackable = false;
	Slow.TimeRemaining = 6.0f;
	Slow.bAllowEnemy = true;
	Slow.bAllowSelf = false;
	Slow.bAllowTeam = false;

	BuffModifiers.Add(Slow);
}
