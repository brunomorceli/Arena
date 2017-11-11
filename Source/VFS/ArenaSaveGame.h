// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Core.h"
#include "GameFramework/SaveGame.h"

#include "Enums.h"
#include "ArenaSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class VFS_API UArenaSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, Category = Basic)
	FText PlayerName;

	UPROPERTY(VisibleAnywhere, Category = Basic)
	TEnumAsByte<ECharacterClass> PlayerClass;

	UPROPERTY(VisibleAnywhere, Category = Basic)
	FString SaveSlotName;

	UPROPERTY(VisibleAnywhere, Category = Basic)
	uint32 UserIndex;

	UPROPERTY(VisibleAnywhere, Category = Basic)
	int32 LeftWeaponSlot = -1;

	UPROPERTY(VisibleAnywhere, Category = Basic)
	int32 RightWeaponSlot = -1;

	UArenaSaveGame();
};
