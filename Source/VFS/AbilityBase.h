// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Core.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Particles/ParticleSystem.h"
#include "Runtime/Engine/Classes/Particles/ParticleSystemComponent.h"
#include "Runtime/Engine/Classes/Engine/EngineTypes.h"
#include "Runtime/Core/Public/Misc/Guid.h"

#include "Enums.h"
#include "CharacterBase.h"
#include "AbilityProjectile.h"
#include "AbilityBase.generated.h"

class AAbilityBase;

// ==================================================================================================================================================
// MODIFIER
// ==================================================================================================================================================

USTRUCT(BlueprintType)
struct FAbilityInfo
{
	GENERATED_USTRUCT_BODY(BlueprintType)

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	TEnumAsByte<EAbilityEvent> Event = EABE_Apply;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	TEnumAsByte<EAbilityInfoType> Type = EAIT_Damage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	int32 bIsHarmful = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	int32 bIsDispellable = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	int32 bExpires = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	float TimeRemaining = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	int32 bCritical = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	float Amount = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	float Absorbed = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	float Overkill = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	ACharacterBase* Causer = NULL;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	ACharacterBase* Target = NULL;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	ACharacterBase* Breaker = NULL;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	AAbilityBase* Ability = NULL;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	UTexture2D* ModifierIcon = NULL;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	FName ModifierName = "";
};

USTRUCT(BlueprintType)
struct FModifierBase
{
	GENERATED_USTRUCT_BODY(BlueprintType)

public:

	typedef void(*HandlerPtr)(FAbilityInfo);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	FGuid ModifierId = FGuid();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	AAbilityBase* AbilityOwner;

	// Icon that will be shown.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	UTexture2D* Icon = NULL;

	// Display Name.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	FName Name = "";

	// Modifier Description.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	FText Description;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	int32 bAllowSelf = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	int32 bAllowTeam = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	int32 bAllowEnemy = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	int32 bIsHarmful = true;

	// School modifier.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modifier")
	TEnumAsByte<EModifierSchool> School = EModifierSchool::MS_Physical;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modifier")
	TEnumAsByte<EModifierConstrain> Constrain = EModifierConstrain::MDCT_None;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	int32 bIsPercent = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modifier")
	int32 bIsDispellable = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modifier")
	int32 bIsStackable = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modifier")
	int32 MaxStacks = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modifier")
	int32 Stacks = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	float Health = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	float Energy = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	float Mana = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	float Speed = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	float Critical = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	float PhysicalPower = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	float MagicPower = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	float PhysicalDefense = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	float MagicDefense = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	float ModifierCritical = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	float ModifierPhysicalPower = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	float ModifierMagicPower = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	float ModifierPhysicalDefense = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	float ModifierMagicDefense = 0.0f;

	HandlerPtr OnApplyHandler = NULL;
	HandlerPtr OnDoDamageHandler = NULL;
	HandlerPtr OnTakeDamageHandler = NULL;
	HandlerPtr OnTickHandler = NULL;
	HandlerPtr OnDoHealHandler = NULL;
	HandlerPtr OnTakeHealHandler = NULL;
	HandlerPtr OnBreakHandler = NULL;
	HandlerPtr OnRenewHandler = NULL;
	HandlerPtr OnExpireHandler = NULL;
	HandlerPtr OnRemoveHandler = NULL;
	HandlerPtr OnCastHandler = NULL;
	HandlerPtr OnChangeStateHandler = NULL;
	HandlerPtr OnCommitAbility = NULL;
	HandlerPtr OnCritical = NULL;
};

USTRUCT(BlueprintType)
struct FAuraModifier : public FModifierBase {
	GENERATED_USTRUCT_BODY(BlueprintType)

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	float MinDistance = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	float MaxDistance = 0.0f;
};

USTRUCT(BlueprintType)
struct FBuffModifier : public FModifierBase {
	GENERATED_USTRUCT_BODY(BlueprintType)

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	float HealthAmount = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	float ManaAmount = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	float EnergyAmount = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	float SpeedAmount = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	float CriticalAmount = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	float MagicPowerAmount = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	float MagicDefenseAmount = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	float PhysicalPowerAmount = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	float PhysicalDefenseAmount = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modifier")
	int32 bUntilUse = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modifier")
	float TimeRemaining = 60.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modifier")
	TEnumAsByte<ECharacterState> State = CS_Idle;

	bool AddStack()
	{
		if (Stacks > MaxStacks) { return false; }

		HealthAmount = Health * Stacks;
		ManaAmount = Mana * Stacks;
		EnergyAmount = Energy * Stacks;
		SpeedAmount = Speed * Stacks;
		CriticalAmount = Critical * Stacks;
		MagicPowerAmount = MagicPower * Stacks;
		MagicDefenseAmount = MagicDefense * Stacks;
		PhysicalPowerAmount = PhysicalPower * Stacks;
		PhysicalDefenseAmount = PhysicalDefense * Stacks;

		Stacks++;

		return true;
	}
};

USTRUCT(BlueprintType)
struct FOvertimeModifier : public FModifierBase {
	GENERATED_USTRUCT_BODY(BlueprintType)

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modifier")
	float TimeRemaining = 10.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modifier")
	float ElapsedTime = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modifier")
	float TickTime = 2.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	float HealthAmount = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	float ManaAmount = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	float EnergyAmount = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	float SpeedAmount = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	float CriticalAmount = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	float MagicPowerAmount = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	float MagicDefenseAmount = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	float PhysicalPowerAmount = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	float PhysicalDefenseAmount = 0.0f;

	bool AddStack()
	{
		if (Stacks > MaxStacks) { return false; }

		HealthAmount = Health * Stacks;
		ManaAmount = Mana * Stacks;
		EnergyAmount = Energy * Stacks;
		SpeedAmount = Speed * Stacks;
		CriticalAmount = Critical * Stacks;
		MagicPowerAmount = MagicPower * Stacks;
		MagicDefenseAmount = MagicDefense * Stacks;
		PhysicalPowerAmount = PhysicalPower * Stacks;
		PhysicalDefenseAmount = PhysicalDefense * Stacks;

		Stacks++;

		return true;
	}
};

USTRUCT(BlueprintType)
struct FAbsorbingModifier : public FModifierBase {
	GENERATED_USTRUCT_BODY(BlueprintType)

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modifier")
	int32 bUntilUse = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modifier")
	float TimeRemaining = 20.0f;
};

USTRUCT(BlueprintType)
struct FHealModifier : public FModifierBase {
	GENERATED_USTRUCT_BODY(BlueprintType)

public:

};

USTRUCT(BlueprintType)
struct FDamageModifier : public FModifierBase {
	GENERATED_USTRUCT_BODY(BlueprintType)

public:

};

// ==================================================================================================================================================
// CLASS
// ==================================================================================================================================================

UCLASS(BlueprintType)
class VFS_API AAbilityBase : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	AAbilityBase();

protected:
	float CountdownTimeRemaining;

	float ChannelingTime;
	float ChannelingTotalTime;

	virtual void UpdateTimers(float DeltaTime);

public:

	TSubclassOf<AAbilityFXBase> CommitFX;
	TSubclassOf<AAbilityFXBase> CastFX;

	// Ability Owner.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	ACharacterBase* CharacterOwner;

	// Ability Slot.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	int32 Slot;

	UParticleSystem* CastParticle;

	// Ability Icon.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	UTexture2D* Icon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	FName Name;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	FString Description;

	// Countdown Time.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Countdown")
	float CountdownTime;

	// Cast Time.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	float CastTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	bool bCastInMovement;

	ACharacterBase* CastTarget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cost")
	float HealthCost;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cost")
	float ManaCost;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cost")
	float EnergyCost;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cost")
	float bIsPercent;

	// Min Distance to be started/commited.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Validation")
	float MinDistance;

	// Max distance to be started/commited.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Validation")
	float MaxDistance;

	// Max angle to be started/commited.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Validation")
	float MaxAngle;

	// Directional radius (if directional)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Validation")
	float DirectionalRadius;

	// Directional Range (if directional type)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Validation")
	float DirectionalRange;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	bool bAllowSelf;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	bool bAllowTeam;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	bool bAllowEnemy;

	// Ability School.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modifier")
	TEnumAsByte<EAbilitySchool> SchoolType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modifier")
	TEnumAsByte<EAbilityStartType> AbilityType;

	// Ability Area.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modifier")
	TEnumAsByte<EAbilityArea> AreaType;

	// Ability Commit Type.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modifier")
	TEnumAsByte<EAbilityCommit> CommitType;

	float ProjectileSpeed;

	TSubclassOf<class AAbilityProjectile> Projectile;
	UParticleSystem* ProjectileHitParticle;

	// Aura Modifiers Array.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mofidiers")
	TArray<FAuraModifier> AuraModifiers;

	// Buff Modifiers Array.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modifiers")
	TArray<FBuffModifier> BuffModifiers;

	// Absorbing Modifier Array.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modifiers")
	TArray<FAbsorbingModifier> AbsorbingModifiers;

	// Overtime Modifiers Array.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modifiers")
	TArray<FOvertimeModifier> OvertimeModifiers;

	// Damage Modifiers Array.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modifiers")
	TArray<FHealModifier> HealModifiers;

	// Damage Modifiers Array.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modifiers")
	TArray<FDamageModifier> DamageModifiers;

	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void SetupModifiers();

	// Load icon.
	void LoadIcon(const FString& Path);

	// Check if Is in Countdown.
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	bool IsCountdown();

	void StartCountdown();

	void StopCountdown();

	// Get Countdown Time Remaining;
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	float GetCountdownRemaining();

	// Get Countdown Percent Progress
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	float GetCountdownPercent();

	void ClearChanneling();

	bool UpdateChanneling(float DeltaTime);

	// Network Replication Props (override).
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;
};
