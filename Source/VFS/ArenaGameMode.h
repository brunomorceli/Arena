// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Core.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/Character.h"

#include "Enums.h"
#include "ArenaGameState.h"
#include "ArenaPlayerState.h"
#include "ArenaCharacter.h"
#include "ArenaGameMode.generated.h"

class AArenaCharacter;

/**
 * 
 */
UCLASS()
class VFS_API AArenaGameMode : public AGameModeBase
{
	GENERATED_BODY()

	bool bNextIsRed;

public:

	TSubclassOf<APawn> KhollBPClass;
	TSubclassOf<APawn> MadEyeBPClass;
	TSubclassOf<APawn> OrbusBPClass;
	TSubclassOf<APawn> OsamuBPClass;

	AArenaGameMode();

	virtual void PostLogin(APlayerController * NewPlayer) override;

	TSubclassOf<APawn> GetPawnClassByCharacterClass(TEnumAsByte<ECharacterClass> PlayerClass);
};
