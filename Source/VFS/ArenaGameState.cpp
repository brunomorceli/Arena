// Fill out your copyright notice in the Description page of Project Settings.

#include "ArenaGameState.h"

AArenaGameState::AArenaGameState()
{

}

void AArenaGameState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME(AArenaGameState, CharacterClass);
}