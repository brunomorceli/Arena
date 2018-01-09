// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AbilityBase.h"
#include "ArenaPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class VFS_API AArenaPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	TMap<int32, AAbilityBase*> Abilities;

	AArenaPlayerController();

	void Test()
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, "--------------- Hello World");
	}
};
