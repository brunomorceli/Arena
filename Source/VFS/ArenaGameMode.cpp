// Fill out your copyright notice in the Description page of Project Settings.

#include "ArenaGameMode.h"
#include "UObject/ConstructorHelpers.h"
#include "ArenaCharacter.h"

AArenaGameMode::AArenaGameMode()
{
	bNextIsRed = false;

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/Characters/DefaultCharacterBP.DefaultCharacterBP_C"));
	if (PlayerPawnBPClass.Class != NULL) { DefaultPawnClass = PlayerPawnBPClass.Class; }

	ConstructorHelpers::FClassFinder<APawn> KhollClass(TEXT("/Game/Blueprints/Characters/KhollCharacterBP.KhollCharacterBP_C"));
	if (KhollClass.Succeeded()) { KhollBPClass = KhollClass.Class; }

	ConstructorHelpers::FClassFinder<APawn> MadEyeClass(TEXT("/Game/Blueprints/Characters/MadEyeCharacterBP.MadEyeCharacterBP_C"));
	if (MadEyeClass.Succeeded()) { MadEyeBPClass = MadEyeClass.Class; }

	ConstructorHelpers::FClassFinder<APawn> OrbusClass(TEXT("/Game/Blueprints/Characters/OrbusCharacterBP.OrbusCharacterBP_C"));
	if (OrbusClass.Succeeded()) { OrbusBPClass = OrbusClass.Class; }

	ConstructorHelpers::FClassFinder<APawn> OsamuClass(TEXT("/Game/Blueprints/Characters/OsamuCharacterBP.OsamuCharacterBP_C"));
	if (OsamuClass.Succeeded()) { OsamuBPClass = OsamuClass.Class; }

	PlayerStateClass = AArenaPlayerState::StaticClass();
	GameStateClass = AArenaGameState::StaticClass();
}

void AArenaGameMode::PostLogin(APlayerController * NewPlayer)
{
	Super::PostLogin(NewPlayer);

	AArenaCharacter* Character = Cast<AArenaCharacter>(NewPlayer->GetPawn());
	if (!Character) { return; }

	AArenaPlayerState* PlayerState = Cast<AArenaPlayerState>(Character->PlayerState);
	if (!PlayerState) { return; }

	TEnumAsByte<EPlayerTeam> NewTeam = bNextIsRed ? EPT_Red : EPT_Blue;
	PlayerState->ServerChangeTeam(NewTeam);

	bNextIsRed = !bNextIsRed;
}

TSubclassOf<APawn> AArenaGameMode::GetPawnClassByCharacterClass(TEnumAsByte<ECharacterClass> PlayerClass)
{	
	switch (PlayerClass)
	{
	case ECCL_Assassin:
		return MadEyeBPClass;
	case ECCL_Wizard:
		return OrbusBPClass;
	case ECCL_Cleric:
		return OsamuBPClass;
	case ECCL_Warrior:
		return KhollBPClass;
	default:
		return NULL;
	}

	/*TEnumAsByte<EPlayerTeam> NewTeam = bNextIsRed ? EPT_Red : EPT_Blue;

	PlayerState->ServerChangeTeam(NewTeam);
	PlayerCharacter->Team = NewTeam;
	//PlayerCharacter->Name = PlayerProfile.PlayerName.ToString();

	AArenaPlayerState* ArenaPlayerState = Cast<AArenaPlayerState>(PlayerState);
	if (!ArenaPlayerState) { return; }

	ArenaPlayerState->ServerSetClass(PlayerProfile.PlayerClass);
	ArenaPlayerState->ServerSetPlayerName(PlayerProfile.PlayerName.ToString());

	bNextIsRed = !bNextIsRed;*/
}
