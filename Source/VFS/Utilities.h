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

	UFUNCTION(BlueprintCallable, Category = "Utilities")
	static bool IsCritical(float PercentChance)
	{
		if (PercentChance >= 100.0f) { return true; }
		return PercentChance > 0.0f && FMath::RandRange(0.0f, 100.0f) <= PercentChance;
	}

	UFUNCTION(BlueprintCallable, Category = "Utilities")
	static float GetAmountPercentage(float Amount, float Percentage)
	{
		return Amount * ( Percentage / 100.0f);
	}
};
