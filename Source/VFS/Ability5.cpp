// Fill out your copyright notice in the Description page of Project Settings.

#include "Ability5.h"
#include "GlobalLibrary.h"

AAbility5::AAbility5()
{
	PrimaryActorTick.bCanEverTick = true;

	Name = "Channeling Attack";
	Description = "Nothing yet.";

	MaxDistance = 300.0f;

	AbilityType = ABST_Channeling;
	CastTime = 1.0f;

	bCastInMovement = true;
	ChannelingTime = 0.333f;
	ChannelingTotalTime = 0.333f;

	DirectionalRadius = 50.0f;
	DirectionalRange = 180.0f;

	AreaType = ABA_Directional;

	LoadIcon("/Game/Sprites/Icons/168.168");
}

void AAbility5::BeginPlay()
{
	Super::BeginPlay();
	
	CastAnimation.AnimSequence = UGlobalLibrary::GetAnimSequence(3);
	CastAnimation.bLoop = true;

	CastAnimation.RightHandTrail = UGlobalLibrary::GetTrail(1);
	CastAnimation.TrailDelay = 0.3f;
	CastAnimation.TrailDuration = 1.0f;
}

void AAbility5::SetupModifiers()
{
	Super::SetupModifiers();

	UWorld* World = GetWorld();
	if (!World) { return; }

	FDamageModifier Damage;
	Damage.AbilityOwner = this;
	Damage.Icon = Icon;
	Damage.Health = 100.0f;
	Damage.StartParticle = UGlobalLibrary::GetParticle(5);
	DamageModifiers.Add(Damage);
}
