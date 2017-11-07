// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Core.h"
#include "GameFramework/SaveGame.h"

#include "ArenaSaveGame.generated.h"

UENUM(BlueprintType)
enum ECharacterClass
{
	ECCL_None UMETA(DisplayName = "None"),
	ECCL_Assassin UMETA(DisplayName = "Assassin"),
	ECCL_Wizard UMETA(DisplayName = "Wizard"),
	ECCL_Cleric UMETA(DisplayName = "Cleric"),
	ECCL_Warrior UMETA(DisplayName = "Warrior"),
};

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
