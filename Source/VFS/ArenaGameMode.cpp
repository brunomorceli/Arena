// Fill out your copyright notice in the Description page of Project Settings.

#include "ArenaGameMode.h"
#include "UObject/ConstructorHelpers.h"

AArenaGameMode::AArenaGameMode()
{
	bNextIsRed = false;

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/DefaultCharacterBP.DefaultCharacterBP_C"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	PlayerStateClass = AArenaPlayerState::StaticClass();
	GameStateClass = AArenaGameState::StaticClass();
}

void AArenaGameMode::PostLogin(APlayerController * NewPlayer)
{
	Super::PostLogin(NewPlayer);

	UWorld* World = GetWorld();
	if (!World) { return; }

	ACharacterBase* CharacterBase = Cast<ACharacterBase>(NewPlayer->GetPawn());
	if (!CharacterBase) { return; }

	AArenaPlayerState* PlayerState = Cast<AArenaPlayerState>(NewPlayer->PlayerState);
	if (!PlayerState) { return; }

	TEnumAsByte<EPlayerTeam> NewTeam = bNextIsRed ? EPT_Red : EPT_Blue;

	PlayerState->ServerChangeTeam(NewTeam);
	CharacterBase->Team = NewTeam;

	bNextIsRed = !bNextIsRed;
}
