// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Core.h"
#include "UObject/NoExportTypes.h"
#include "Runtime/Engine/Classes/Particles/ParticleSystem.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Animation/AnimSequence.h"

#include "Enums.h"
#include "AbilityBase.h"
#include "Ability1.h"
#include "Ability2.h"
#include "Ability3.h"
#include "Ability4.h"
#include "Ability5.h"
#include "AbilityProjectile.h"
#include "Global.generated.h"

USTRUCT(BlueprintType)
struct FClassPreset
{
	GENERATED_USTRUCT_BODY(BlueprintType)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ClassPreset")
	TEnumAsByte<ECharacterClass> CharacterClass = ECCL_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ClassPreset")
	TArray<TSubclassOf<AAbilityBase>> Abilities = TArray<TSubclassOf<AAbilityBase>>();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ClassPreset")
	UTexture2D* Badge = NULL;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ClassPreset")
	FString Name = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ClassPreset")
	FString Description = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ClassPreset")
	USkeletalMesh* CharacterMesh = NULL;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ClassPreset")
	USkeletalMesh* LeftWeapon = NULL;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ClassPreset")
	USkeletalMesh* RightWeapon = NULL;
};

/**
 * 
 */
UCLASS(Config=Game, Blueprintable, BlueprintType)
class VFS_API UGlobal : public UObject
{
	GENERATED_BODY()

private:
	void LoadSprites(FString& BasePath);
	void LoadIcons(FString& BasePath);
	void LoadAbilities();

public:
	UGlobal(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(Config)
	FString ServerAddress;

	// ==================================================================================
	// ABILITIES
	// ==================================================================================

	TMap<int32, TSubclassOf<AAbilityBase>> Abilities;

	// ==================================================================================
	// SPRITES
	// ==================================================================================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Global")
	FString SpritePath;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Global")
	int32 TotalIcons;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Global")
	int32 TotalBadges;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Global")
	TMap<int32, UTexture2D*> Icons;

	// ==================================================================================
	// PARTICLES
	// ==================================================================================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Global")
	TMap<int32, UParticleSystem*> Particles;

	// ==================================================================================
	// PROJECTILES
	// ==================================================================================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Global")
	TMap<int32, TSubclassOf<class AAbilityProjectile>> Projectiles;

	// ==================================================================================
	// CLASSES
	// ==================================================================================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Global")
	TMap<TEnumAsByte<ECharacterClass>, FClassPreset> Classes;

	// ==================================================================================
	// WEAPONS
	// ==================================================================================
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Global")
	TMap<int32, USkeletalMesh*> Weapons;

	// ==================================================================================
	// ANIMATION SEQUENCES
	// ==================================================================================
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Global")
	TMap<int32, UAnimSequence*> AnimSequences;

};
