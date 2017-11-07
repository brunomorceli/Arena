// Fill out your copyright notice in the Description page of Project Settings.

#include "Ability3.h"
#include "GlobalLibrary.h"

AAbility3::AAbility3()
{
	PrimaryActorTick.bCanEverTick = true;

	CountdownTime = 3.0f;
	Name = "Leap";
	Description = "Nothing yet.";
	MaxDistance = 1200.0f;
	LoadIcon("/Game/Sprites/Icons/304.304");
}

void AAbility3::SetupModifiers()
{
	Super::SetupModifiers();

	UWorld* World = GetWorld();
	if (!World) { return; }

	FOvertimeModifier Overtime;
	Overtime.AbilityOwner = this;
	Overtime.Icon = Icon;
	Overtime.Health = 150.0f;
	Overtime.StartParticle = UGlobalLibrary::GetParticle(5);

	Overtime.TickTime = 2.0f;
	Overtime.TimeRemaining = 20.0f;
	OvertimeModifiers.Add(Overtime);
}

void AAbility3::OnStart(ACharacterBase* Target)
{
	Super::OnStart(Target);
	CharacterOwner->ServerJumpTo();
}
