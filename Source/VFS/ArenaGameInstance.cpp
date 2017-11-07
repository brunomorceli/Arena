// Fill out your copyright notice in the Description page of Project Settings.

#include "ArenaGameInstance.h"

// ============================================================================
// CLASS DEPENDENCIES
// ============================================================================

UArenaGameInstance::UArenaGameInstance(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// void
}

bool UArenaGameInstance::OwnProfile()
{
	UArenaSaveGame* ArenaSaveGame = Cast<UArenaSaveGame>(UGameplayStatics::CreateSaveGameObject(UArenaSaveGame::StaticClass()));
	return UGameplayStatics::DoesSaveGameExist(ArenaSaveGame->SaveSlotName, ArenaSaveGame->UserIndex);
}

void UArenaGameInstance::SaveProfile(FText PlayerName, ECharacterClass PlayerClass)
{
	UArenaSaveGame* ArenaSaveGame = Cast<UArenaSaveGame>(UGameplayStatics::CreateSaveGameObject(UArenaSaveGame::StaticClass()));
	ArenaSaveGame->PlayerName = PlayerName;
	ArenaSaveGame->PlayerClass = PlayerClass;

	UGameplayStatics::SaveGameToSlot(ArenaSaveGame, ArenaSaveGame->SaveSlotName, ArenaSaveGame->UserIndex);
}

FPlayerProfile UArenaGameInstance::GetProfile()
{
	FPlayerProfile PlayerProfile;
	UArenaSaveGame* ArenaSaveGame = Cast<UArenaSaveGame>(UGameplayStatics::CreateSaveGameObject(UArenaSaveGame::StaticClass()));

	if (UGameplayStatics::DoesSaveGameExist(ArenaSaveGame->SaveSlotName, ArenaSaveGame->UserIndex))
	{
		ArenaSaveGame = Cast<UArenaSaveGame>(UGameplayStatics::LoadGameFromSlot(ArenaSaveGame->SaveSlotName, ArenaSaveGame->UserIndex));

		PlayerProfile.PlayerName = ArenaSaveGame->PlayerName;
		PlayerProfile.PlayerClass = ArenaSaveGame->PlayerClass;
	}

	return PlayerProfile;
}
