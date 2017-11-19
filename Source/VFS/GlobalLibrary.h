// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Core.h"
#include "UObject/NoExportTypes.h"

#include "Enums.h"
#include "Global.h"
#include "GlobalLibrary.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class VFS_API UGlobalLibrary : public UObject
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable, Category = "GLobalLibrary")
	static FORCEINLINE UGlobal* GetGlobal()
	{
		UGlobal* Global = Cast<UGlobal>(GEngine->GameSingleton);
		if (!Global || !Global->IsValidLowLevel()) { return NULL; }

		return Global;
	}

	UFUNCTION(BlueprintCallable, Category = "GLobalLibrary")
	static FORCEINLINE TSubclassOf<AAbilityBase> GetAbility(int32 Slot)
	{
		UGlobal* Global = GetGlobal();
		if (!Global) { return NULL; }
		return Global->Abilities.Contains(Slot) ? Global->Abilities[Slot] : NULL;
	}

	UFUNCTION(BlueprintCallable, Category = "GLobalLibrary")
	static FORCEINLINE UTexture2D* GetIcon(int32 Slot)
	{
		UGlobal* Global = GetGlobal();
		if (!Global) { return NULL; }
		return Global->Icons.Contains(Slot) ? Global->Icons[Slot] : NULL;
	}

	UFUNCTION(BlueprintCallable, Category = "GLobalLibrary")
	static FORCEINLINE UParticleSystem* GetParticle(int32 Slot)
	{
		UGlobal* Global = GetGlobal();
		if (!Global) { return NULL; }
		return Global->Particles.Contains(Slot) ? Global->Particles[Slot] : NULL;
	}

	UFUNCTION(BlueprintCallable, Category = "GLobalLibrary")
	static FORCEINLINE UParticleSystem* GetTrail(int32 Slot)
	{
		UGlobal* Global = GetGlobal();
		if (!Global) { return NULL; }
		return Global->Trails.Contains(Slot) ? Global->Trails[Slot] : NULL;
	}

	UFUNCTION(BlueprintCallable, Category = "GLobalLibrary")
	static FORCEINLINE TSubclassOf<class AAbilityProjectile> GetProjectile(int32 Slot)
	{
		UGlobal* Global = GetGlobal();
		if (!Global) { return NULL; }
		return Global->Projectiles.Contains(Slot) ? Global->Projectiles[Slot] : NULL;
	}

	UFUNCTION(BlueprintCallable, Category = "GLobalLibrary")
	static FORCEINLINE FClassPreset GetClass(ECharacterClass CharacterClass)
	{
		UGlobal* Global = GetGlobal();
		if (!Global) { return FClassPreset(); }
		return Global->Classes.Contains(CharacterClass) ? Global->Classes[CharacterClass] : FClassPreset();
	}

	UFUNCTION(BlueprintCallable, Category = "GLobalLibrary")
	static FORCEINLINE USkeletalMesh* GetWeapon(int32 Slot)
	{
		UGlobal* Global = GetGlobal();
		if (!Global) { return NULL; }
		return Global->Weapons.Contains(Slot) ? Global->Weapons[Slot] : NULL;
	}

	UFUNCTION(BlueprintCallable, Category = "GLobalLibrary")
	static FORCEINLINE FString GetServerAddress()
	{
		UGlobal* Global = GetGlobal();
		if (!Global) { return ""; }

		return Global->ServerAddress;
	}

	UFUNCTION(BlueprintCallable, Category = "GLobalLibrary")
	static FORCEINLINE UAnimSequence* GetAnimSequence(int32 Slot)
	{
		UGlobal* Global = GetGlobal();
		if (!Global) { return NULL; }
		return Global->AnimSequences.Contains(Slot) ? Global->AnimSequences[Slot] : NULL;
	}

	UFUNCTION(BlueprintCallable, Category = "GLobalLibrary")
	static FORCEINLINE TSubclassOf<class AAbilityFXBase> GetAbilityUseFX(int32 Slot)
	{
		UGlobal* Global = GetGlobal();
		if (!Global) { return NULL; }
		return Global->AbilityUseFXs.Contains(Slot) ? Global->AbilityUseFXs[Slot] : NULL;
	}

	UFUNCTION(BlueprintCallable, Category = "GLobalLibrary")
	static FORCEINLINE TSubclassOf<class AAbilityFXBase> GetAbilityHitFX(int32 Slot)
	{
		UGlobal* Global = GetGlobal();
		if (!Global) { return NULL; }
		return Global->AbilityHitFXs.Contains(Slot) ? Global->AbilityHitFXs[Slot] : NULL;
	}
};
