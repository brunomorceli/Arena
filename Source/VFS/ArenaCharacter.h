// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "Runtime/Engine/Classes/GameFramework/GameStateBase.h"

#include "CharacterBase.h"
#include "AbilityBase.h"
#include "ArenaGameInstance.h"
#include "Utilities.h"
#include "ArenaCharacter.generated.h"

// ==================================================================================================================================================
// CLASS
// ==================================================================================================================================================

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNotifyAbilityInfoDelegate, FAbilityInfo, AbilityInfo);

UCLASS(config=Game)
class AArenaCharacter : public ACharacterBase
{
	GENERATED_BODY()

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	float GlobalCountdownTime;
	float GlobalCountdownTimeRemaining;

	float CastTimeRemaining;

	AAbilityBase* CastAbility;

	void UpdateTimers(float DeltaTime);

	void UpdateBuffModifiers(float DeltaTime);

	void RefreshBuffModifiersStatus();

	void UpdateOvertimeModifiers(float DeltaTime);

	void StartGlobalCountdown();
	void StopGlobalCountdown();
	bool IsGlobalCountdown();
	float GetGlobalCountdownRemaining();
	float GetGlobalCountdownPercent();

	// Start Cast
	void StartCast(int32 Slot);

	// Start Cast.
	void StopCast();

	void CommitAbility(int32 Slot);
	void CommitAbilityModifiers(AAbilityBase* Ability);

public:

	UPROPERTY(VisibleAnywhere, BlueprintAssignable, Category = "Notification")
	FNotifyAbilityInfoDelegate AbilityInfoDelegate;

	TMap<int32, AAbilityBase*> Abilities;

	AArenaCharacter();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void UpdateState() override;
	void ValidateCast();

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	bool IsCountdown(int32 Slot);

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	float GetCountdownRemaining(int32 Slot);

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	float GetCountdownPercent(int32 Slot);

	bool AddAbility(TSubclassOf<AAbilityBase> AbilityBase, int32 Slot);

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	AAbilityBase* GetAbility(int32 Slot);

	EAbilityValidation StartAbility(int32 Slot);

	void StopAbility(int32 Slot);

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	bool IsCasting();

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	float GetCastTimeRemaining();

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	float GetCastPercent();

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	bool IsValidCommitState();

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	bool IsValidModifierState();

	template<int32 Slot> void InputStartAbility();

	EAbilityValidation ValidateStartAbility(int32 Slot);
	EAbilityValidation ValidateStartCast(AAbilityBase* Ability);
	EAbilityValidation ValidateStartChanneling(AAbilityBase* Ability);
	EAbilityValidation ValidateStartInstant(AAbilityBase* Ability);
	EAbilityValidation ValidateCost(AAbilityBase* Ability);

	EAbilityValidation ValidateTarget(AAbilityBase* Ability, ACharacterBase* First, ACharacterBase* Second);

	bool IsEnemyCharacter(ACharacterBase* First, ACharacterBase* Second);

	FString GetErrorString(EAbilityValidation Validation);

	// Aura Modifiers Array.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mofidiers")
	TArray<FAuraModifier> AuraModifiers;

	// Buff Modifiers Array.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modifiers")
	TMap<int32, FBuffModifier> BuffModifiers;

	// Overtime Modifiers Array.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modifiers")
	TMap<int32, FOvertimeModifier> OvertimeModifiers;

	// Absorbing Modifier Array.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modifiers")
	TArray<FAbsorbingModifier> AbsorbingModifiers;

	UFUNCTION(Category = "Modifiers")
	void ApplyModifiers(AAbilityBase* Ability);

	UFUNCTION(Category = "Modifiers")
	void ApplyDelayedModifiers(AAbilityBase* Ability);

	void ApplyAuraModifier(FAuraModifier Modifier);
	void ApplyBuffModifier(FBuffModifier Modifier);
	void ApplyAbsorbingModifier(FAbsorbingModifier Modifier);
	void ApplyDamageModifier(FDamageModifier Modifier);
	void ApplyOvertimeModifier(FOvertimeModifier Modifier);
	void ApplyHealModifier(FHealModifier Modifier);
	void ApplyCosts(AAbilityBase* Ability);

	void TickOvertimeModifier(FOvertimeModifier Modifier);

	void CalculateDamage(FModifierBase Modifier, FAbilityInfo &AbilityInfo);

	void CalculateDamageByBuffModifiers(FModifierBase Modifier, FAbilityInfo &AbilityInfo);
	
	void CalculateDamageByAbsorbingModifiers(FModifierBase Modifier, FAbilityInfo &AbilityInfo);

	void CalculateHeal(FModifierBase Modifier, FAbilityInfo &AbilityInfo);

	void UpdateOnTakeDamage(FAbilityInfo AbilityInfo);
	void UpdateOnTakeHeal(FAbilityInfo AbilityInfo);
	void UpdateOnBreak(FAbilityInfo AbilityInfo);

	FAbilityInfo GetDefaultAbilityInfo(FModifierBase Modifier);
	FAbilityInfo GetAbilityInfo(FAuraModifier Modifier);
	FAbilityInfo GetAbilityInfo(FBuffModifier Modifier);
	FAbilityInfo GetAbilityInfo(FAbsorbingModifier Modifier);
	FAbilityInfo GetAbilityInfo(FDamageModifier Modifier);
	FAbilityInfo GetAbilityInfo(FOvertimeModifier Modifier);
	FAbilityInfo GetAbilityInfo(FHealModifier Modifier);

	// ==========================================================================================
	// NETWORK
	// ==========================================================================================

	virtual void OnRep_PlayerClass() override;

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStartAbility(int32 Slot);
	void ServerStartAbility_Implementation(int32 Slot);
	bool ServerStartAbility_Validate(int32 Slot) { return true; }

	UFUNCTION(Client, Reliable)
	void ClientStartAbility(int32 Slot);
	void ClientStartAbility_Implementation(int32 Slot);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSpawnProjectile(AArenaCharacter* Causer, AArenaCharacter* CharacterTarget, AAbilityBase* Ability);
	void MulticastSpawnProjectile_Implementation(AArenaCharacter* Causer, AArenaCharacter* CharacterTarget, AAbilityBase* Ability);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerAddAbility(TSubclassOf<AAbilityBase> AbilityBase, int32 Slot);
	void ServerAddAbility_Implementation(TSubclassOf<AAbilityBase> AbilityBase, int32 Slot);
	bool ServerAddAbility_Validate(TSubclassOf<AAbilityBase> AbilityBase, int32 Slot) { return true; }

	UFUNCTION(Client, Reliable)
	void ClientAddAbility(TSubclassOf<AAbilityBase> AbilityBase, int32 Slot);
	void ClientAddAbility_Implementation(TSubclassOf<AAbilityBase> AbilityBase, int32 Slot);

	UFUNCTION(Client, Reliable)
	void ClientSetPlayerProfile();
	void ClientSetPlayerProfile_Implementation();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetPlayerProfile(FPlayerProfile PlayerProfile);
	void ServerSetPlayerProfile_Implementation(FPlayerProfile PlayerProfile);
	bool ServerSetPlayerProfile_Validate(FPlayerProfile PlayerProfile) { return true; }

	UFUNCTION(NetMulticast, Unreliable, Category = "Network")
	void MulticastNotifyAbilityInfo(FAbilityInfo AbilityInfo);
	void MulticastNotifyAbilityInfo_Implementation(FAbilityInfo AbilityInfo);

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;
};