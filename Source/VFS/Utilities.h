// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Core.h"
#include "UObject/NoExportTypes.h"
#include "GameFramework/Character.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Utilities.generated.h"

UCLASS(Blueprintable, BlueprintType)
class VFS_API UUtilities : public UObject
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Utilities")
	static void PlayRandomSound(TArray<USoundBase*> Sounds, UWorld* World, FVector Location)
	{
		UGameplayStatics::PlaySoundAtLocation(World, Sounds[FMath::RandRange(0, Sounds.Num()-1)], Location);
	}
};
