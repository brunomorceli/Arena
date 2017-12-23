// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Core.h"
#include "Object.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Particles/ParticleSystem.h"
#include "Runtime/Engine/Classes/Particles/ParticleSystemComponent.h"
#include "Runtime/Engine/Classes/Engine/EngineTypes.h"
#include "Runtime/Core/Public/Misc/Guid.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Runtime/Engine/Classes/Engine/Blueprint.h"

#include "Enums.h"

#include "Structs.generated.h"

// ==================================================================================================================================================
// ABILITY INFO STRUCTURE
// ==================================================================================================================================================

USTRUCT(BlueprintType)
struct FAbilityInfo
{
	GENERATED_USTRUCT_BODY(BlueprintType)

public:

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
	int32 bDenied = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	float TimeRemaining = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	int32 Stacks = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	int32 bCritical = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	float Amount = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	float Absorbed = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	float Overkill = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	AActor* Causer = NULL;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	AActor* Target = NULL;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	AActor* Ability = NULL;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	AActor* Breaker = NULL;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	UTexture2D* ModifierIcon = NULL;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	FName ModifierName = "";

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	FString ModifierDescription = "";
};

// ==================================================================================================================================================
// STATUS STRUCTURE
// ==================================================================================================================================================

USTRUCT(BlueprintType)
struct FStatus
{
	GENERATED_USTRUCT_BODY(BlueprintType)

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	float Min = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	float MinBase = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	float Max = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	float MaxBase = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	float Value = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	float ValueBase = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	float Regeneration = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	float RegenerationBase;

	void Setup(float MinAmount, float MaxAmount, float ValueAmount = 0.0f, float RegenerationAmount = 0.0f)
	{
		MinBase = MinAmount;
		Min = MinBase;
		MaxBase = MaxAmount;
		Max = MaxBase;
		ValueBase = ValueAmount;
		Value = ValueBase;
		RegenerationBase = RegenerationAmount;
		Regeneration = RegenerationBase;
	}

	void Reset()
	{
		Max = MaxBase;
		Min = MinBase;
		Regeneration = RegenerationBase;
		Value = ValueBase;
	}

	void ResetLimits()
	{
		float OldValue = Value;

		Reset();
		Value = OldValue;
	}

	float GetAmountByPercent(float MaxVal, float Percent)
	{
		return (MaxVal / 100.0f) * FMath::Abs(Percent);
	}

	float GetPercentAmount(float Percent)
	{
		return (Max / 100.0f) * Percent;
	}

	bool GotAmount(float Amount, float Percent = false) {
		return (Percent ? GetPercentAmount(Amount) : Value) >= Amount;
	}

	float Clamp(float Amount, bool bIsPercent = false)
	{
		if (!bIsPercent) { Value = FMath::Clamp(Value + Amount, Min, Max); }
		else { Value = FMath::Clamp(Value + GetAmountByPercent(Max, Amount), Min, Max); }

		return Value;
	}

	float Damage(float Amount, bool bIsPercent = false) { return Clamp(-Amount, bIsPercent); }

	float Heal(float Amount, bool bIsPercent = false) { return Clamp(Amount, bIsPercent); }

	float GetPercent() { return (Value * 100.0f) / Max; }

	float GetPercentProgressBar() {
		float Percent = GetPercent();
		return Percent > 0.0f ? (Percent / 100.0f) : 0.0f;
	}

	FText GetPercentText() {
		int32 Percent = (int32)GetPercent();
		FString MyString = FString::FString();
		MyString.AppendInt(Percent);
		MyString.Append("%");

		return FText::AsCultureInvariant(MyString);
	}

	FText GetStatusText()
	{
		FString MyString = FString::FString();

		MyString.AppendInt((int32)Value);
		MyString.Append(" / ");
		MyString.AppendInt((int32)Max);

		return FText::AsCultureInvariant(MyString);
	}

	float GetModifierAmount(float Amount, bool bPercent = false)
	{
		return bPercent ? ((Max / 100.0f) * Amount) : Amount;
	}
};

// ==================================================================================================================================================
// MODIFIER
// ==================================================================================================================================================

USTRUCT(BlueprintType)
struct FModifierBase
{
	GENERATED_USTRUCT_BODY(BlueprintType)

public:

	typedef void(*HandlerPtr)(FAbilityInfo);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	FGuid ModifierId = FGuid();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	AActor* AbilityOwner;

	// Icon that will be shown.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	UTexture2D* Icon = NULL;

	// Display Name.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	FName Name = "";

	// Modifier Description.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	FString Description;

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
	int32 Stacks = 0;

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
	HandlerPtr OnDenied = NULL;
	HandlerPtr OnBeforeUseAbility = NULL;
	HandlerPtr OnUseAbility = NULL;

	bool AddStack()
	{
		if (Stacks >= MaxStacks) { return false; }
		Stacks++;

		HealthAmount = Health * Stacks;
		ManaAmount = Mana * Stacks;
		EnergyAmount = Energy * Stacks;
		SpeedAmount = Speed * Stacks;
		CriticalAmount = Critical * Stacks;
		MagicPowerAmount = MagicPower * Stacks;
		MagicDefenseAmount = MagicDefense * Stacks;
		PhysicalPowerAmount = PhysicalPower * Stacks;
		PhysicalDefenseAmount = PhysicalDefense * Stacks;

		return true;
	}
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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modifier")
	int32 bUntilUse = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modifier")
	float TimeRemaining = 60.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modifier")
	TEnumAsByte<ECharacterState> State = CS_Idle;
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

USTRUCT(BlueprintType)
struct FCharacterFX
{
	GENERATED_USTRUCT_BODY(BlueprintType)

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	int32 PlayAnimation = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	UParticleSystem* LeftHandTrail = NULL;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	UParticleSystem* RightHandTrail = NULL;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	float TrailWidth = 1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	float TrailDuration = 0.5f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	float TrailDelay = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	USoundBase* Sound = NULL;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	float SoundDelay = 0.0f;
};

UCLASS()
class VFS_API UStructs : public UObject
{
	GENERATED_BODY()

public:
	UStructs();
};
