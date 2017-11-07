// Fill out your copyright notice in the Description page of Project Settings.

#include "ArenaGameMode.h"
#include "UObject/ConstructorHelpers.h"

AArenaGameMode::AArenaGameMode()
{
	bNextIsRed = false;

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/ArenaCharacterBP.ArenaCharacterBP_C"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	PlayerStateClass = AArenaPlayerState::StaticClass();
	GameStateClass = AArenaGameState::StaticClass();
}

void AArenaGameMode::CreateDefaultAbilities(AArenaCharacter* Character)
{
}

void AArenaGameMode::PostLogin(APlayerController * NewPlayer)
{
	Super::PostLogin(NewPlayer);

	UWorld* World = GetWorld();
	if (!World) { return; }

	AArenaPlayerState* PlayerState = Cast<AArenaPlayerState>(NewPlayer->PlayerState);
	if (!PlayerState) { return; }

	PlayerState->ServerChangeTeam(bNextIsRed ? EPT_Red : EPT_Blue);

	bNextIsRed = !bNextIsRed;
}

void AArenaGameMode::StartAbility(AArenaCharacter* Character, int32 Slot)
{

}

void AArenaGameMode::StopAbility(AArenaCharacter* Character, int32 Slot)
{
	/*AArenaCharacter* MyCharacter = Cast<AArenaCharacter>(Character);
	if (!MyCharacter)
	{
		return;
	}*/
}

void AArenaGameMode::CommitAbility(AArenaCharacter* Character, int32 Slot)
{
	/*AArenaCharacter* MyCharacter = Cast<AArenaCharacter>(Character);
	if (!MyCharacter)
	{
		return;
	}*/
}