// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Core.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/Character.h"

#include "Enums.h"
#include "ArenaPlayerController.h"
#include "ArenaGameState.h"
#include "ArenaPlayerState.h"
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

	TSubclassOf<APawn> WarriorBPClass;
	TSubclassOf<APawn> MadEyeBPClass;
	TSubclassOf<APawn> WizardBPClass;
	TSubclassOf<APawn> OsamuBPClass;

	AArenaGameMode();

	virtual void PostLogin(APlayerController * NewPlayer) override;

	TSubclassOf<APawn> GetPawnClassByCharacterClass(TEnumAsByte<ECharacterClass> PlayerClass);
};
