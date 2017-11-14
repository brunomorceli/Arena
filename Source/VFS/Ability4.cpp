// Fill out your copyright notice in the Description page of Project Settings.

#include "Ability4.h"


AAbility4::AAbility4()
{
	PrimaryActorTick.bCanEverTick = true;

	CountdownTime = 2.0f;
	Name = "First Blood";
	Description = "Nothing yet.";

	ManaCost = 100.0f;
	MaxDistance = 1200.0f;

	bAllowSelf = true;
	bAllowEnemy = false;
	bAllowTeam = true;

	LoadIcon("/Game/Sprites/Icons/189.189");
}

void AAbility4::BeginPlay()
{
	Super::BeginPlay();
}

void AAbility4::SetupModifiers()
{
	Super::SetupModifiers();

	UWorld* World = GetWorld();
	if (!World) { return; }

	FBuffModifier Buff;
	Buff.AbilityOwner = this;
	Buff.Name = "Stamina";
	Buff.Icon = Icon;
	Buff.bAllowEnemy = false;
	Buff.bAllowTeam = true;
	Buff.bAllowSelf = true;
	Buff.Health = 20.0f;
	Buff.bIsPercent = true;
	Buff.TimeRemaining = 300.0f;

	Buff.bIsDispellable = true;
	Buff.bIsStackable = true;
	Buff.MaxStacks = 4;
	Buff.Stacks = 1;

	BuffModifiers.Push(Buff);
}