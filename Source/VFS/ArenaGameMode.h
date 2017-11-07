// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Core.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/Character.h"
#include "ArenaGameState.h"
#include "ArenaPlayerState.h"
#include "ArenaCharacter.h"
#include "ArenaGameMode.generated.h"

/**
 * 
 */
UCLASS()
class VFS_API AArenaGameMode : public AGameModeBase
{
	GENERATED_BODY()

	bool bNextIsRed;

public:
	AArenaGameMode();

	void CreateDefaultAbilities(AArenaCharacter* Character);

	virtual void PostLogin(APlayerController * NewPlayer) override;

	void StartAbility(AArenaCharacter* Character, int32 Slot);
	void StopAbility(AArenaCharacter* Character, int32 Slot);
	void CommitAbility(AArenaCharacter* Character, int32 Slot);
};
