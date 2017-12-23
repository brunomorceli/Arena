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
#include "Structs.h"
#include "AbilityProjectile.h"
#include "AbilityBase.generated.h"

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

	virtual void UpdateTimers(float DeltaTime);

public:

	TSubclassOf<AAbilityFXBase> CommitFX;
	TSubclassOf<AAbilityFXBase> CastFX;

	// Ability Owner.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	AActor* CharacterOwner;

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
	float ChannelingTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	float ChannelingTotalTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	bool bCastInMovement;

	AActor* CastTarget;

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
