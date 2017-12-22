// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Core.h"
#include "GameFramework/GameStateBase.h"
#include "Net/UnrealNetwork.h"

#include "ArenaPlayerState.h"
#include "Enums.h"
#include "ArenaGameState.generated.h"

UCLASS()
class VFS_API AArenaGameState : public AGameStateBase
{
	GENERATED_BODY()

public:

	AArenaGameState();

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;
};
