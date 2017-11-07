// Fill out your copyright notice in the Description page of Project Settings.

#include "ArenaPlayerState.h"
#include "GlobalLibrary.h"
#include "ArenaCharacter.h"

AArenaPlayerState::AArenaPlayerState(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	CharacterClass = ECCL_None;
	Team = EPT_Blue;
	Kills = 0;
	Deaths = 0;
	DamageDone = 0.0f;
	DamageTaken = 0.0f;
	HealingDone = 0.0f;
	HealingTaken = 0.0f;
	MaxDamage = 0.0f;
}

void AArenaPlayerState::BeginPlay()
{
	Super::BeginPlay();
}

void AArenaPlayerState::ServerChangeTeam_Implementation(EPlayerTeam NewTeam)
{
	if (NewTeam != Team) { Team = NewTeam; }
}

void AArenaPlayerState::ServerSetClass_Implementation(ECharacterClass NewClass)
{
	CharacterClass = NewClass;
}

void AArenaPlayerState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AArenaPlayerState, CharacterClass);
	DOREPLIFETIME(AArenaPlayerState, Team);
	DOREPLIFETIME(AArenaPlayerState, Kills);
	DOREPLIFETIME(AArenaPlayerState, Deaths);
	DOREPLIFETIME(AArenaPlayerState, DamageDone);
	DOREPLIFETIME(AArenaPlayerState, DamageTaken);
	DOREPLIFETIME(AArenaPlayerState, HealingDone);
	DOREPLIFETIME(AArenaPlayerState, HealingTaken);
	DOREPLIFETIME(AArenaPlayerState, MaxDamage);
}
