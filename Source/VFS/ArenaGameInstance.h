// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Core.h"
#include "Engine/GameInstance.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

#include "ArenaSaveGame.h"

#include "ArenaGameInstance.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStartFindSessionDelegate);


USTRUCT(BlueprintType)
struct FPlayerProfile
{
	GENERATED_USTRUCT_BODY(BlueprintType)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Profile")
	FText PlayerName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Profile")
	TEnumAsByte<ECharacterClass> PlayerClass = ECCL_None;
};

UCLASS()
class VFS_API UArenaGameInstance : public UGameInstance
{
	GENERATED_BODY()

// =========================================================================================
// CLASS DEPENDENCIES
// =========================================================================================

public:
	// Default constructor.
	UArenaGameInstance(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "LocalProfile")
	bool OwnProfile();

	UFUNCTION(BlueprintCallable, Category = "LocalProfile")
	void SaveProfile(FText PlayerName, ECharacterClass PlayerClass);

	UFUNCTION(BlueprintCallable, Category = "LocalProfile")
	FPlayerProfile GetProfile();
};