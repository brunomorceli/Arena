// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Core.h"
#include "CharacterBase.h"
#include "Runtime/Engine/Classes/Engine/EngineTypes.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilityProjectile.h"
#include "AbilityCharacter.generated.h"

UENUM(BlueprintType)
enum EAbilitySchool
{
	ASC_Physical UMETA(DisplayName = "Physical"),
	ASC_Magic UMETA(DisplayName = "Magic"),
	ASC_Elemental UMETA(DisplayName = "Fire"),
	ASC_Bloody UMETA(DisplayName = "Bloody"),
};

UENUM(BlueprintType)
enum EAbilityUseType
{
	AUT_Instant UMETA(DisplayName = "Instant"),
	AUT_Castable UMETA(DisplayName = "Castable"),
};

UENUM(BlueprintType)
enum EAbilityArea
{
	AA_Directional UMETA(DisplayName = "Directional"),
	AA_AreaOnEffect UMETA(DisplayName = "Radial"),
	AA_Target UMETA(DisplayName = "Target"),
};

UENUM(BlueprintType)
enum EAbilityScope
{
	AS_Self UMETA(DisplayName = "Self"),
	AS_FirstAlly UMETA(DisplayName = "FirstAlly"),
	AS_Allies UMETA(DisplayName = "Allies"),
	AS_FirstEnemy UMETA(DisplayName = "FirstEnemy"),
	AS_Enemies UMETA(DisplayName = "Enemies"),
};

UENUM(BlueprintType)
enum EAbilityValidation
{
	AV_Allowed UMETA(DisplayName = "Allowed"),
	AV_InvalidAbility UMETA(DisplayName = "InvalidAbility"),
	AV_NotReady UMETA(DisplayName = "NotReady"),
	AV_Health UMETA(DisplayName = "NotEnoughtHealth"),
	AV_Mana UMETA(DisplayName = "NotEnoughtMana"),
	AV_Energy UMETA(DisplayName = "NotEnoughEnergy"),
	AV_Speed UMETA(DisplayName = "NotEnoughtSpeed"),
	AV_Target UMETA(DisplayName = "YouNeedTarget"),
	AV_TooFar UMETA(DisplayName = "IsTooFarAway"),
	AV_TooClose UMETA(DisplayName = "IsToClose"),
	AV_InMovement UMETA(DisplayName = "InMovement"),
	AV_OutOfSight UMETA(DisplayName = "OutOfSight"),
	AV_Incapacitated UMETA(DisplayName = "Incapacitated"),
};

UENUM(BlueprintType)
enum EAbilityCommitType
{
	ACT_Instant UMETA(DisplayName = "Instant"),
	ACT_Projectile UMETA(DisplayName = "Projectile"),
};

USTRUCT(BlueprintType)
struct FAbility
{
	GENERATED_BODY()

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	UTexture2D* Icon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	int32 Slot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	FName Name;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	FText Description;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbilitySystem")
	float LastUseTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Countdown")
	float CountdownTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Countdown")
	float CastTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	FModifierStatus HealthCost;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	FModifierStatus ManaCost;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	FModifierStatus EnergyCost;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	TEnumAsByte<EAbilityArea> Area;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	float MinDistance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	float MaxDistance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	TEnumAsByte<EAbilityUseType> UseType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	float CastMaxAngle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	TEnumAsByte<EAbilityCommitType> CommitType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	TEnumAsByte<EProjectileType> ProjectileType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	TEnumAsByte<EAbilityScope> Scope;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modifiers")
	TMap<int32, FBuffModifier> BuffModifiers;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modifiers")
	TMap<int32, FOvertimeModifier> OvertimeModifiers;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modifiers")
	TMap<int32, FInstantModifier> InstantModifiers;

	FAbility()
	{
		LastUseTime = -3600.0f;
		CountdownTime = 0.0f;
		CastTime = 0.0f;
		Area = EAbilityArea::AA_Directional;
		MinDistance = 0.0f;
		MaxDistance = 1500.0f;
		UseType = EAbilityUseType::AUT_Instant;
		CastMaxAngle = 180.0f;
		CommitType = EAbilityCommitType::ACT_Instant;
		ProjectileType = EProjectileType::PTP_Frost;
		Scope = EAbilityScope::AS_Enemies;
	}

	void LoadIcon(const FString& Path)
	{
		Icon = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, *(Path)));
	}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSpawnProjectileDelegate, ACharacterBase*, TargetCharacter, EProjectileType, ProjectileType);

/**
 * 
 */
UCLASS()
class VFS_API AAbilityCharacter : public ACharacterBase
{
	GENERATED_BODY()
	
private:
	float GlobalCountdownTime;
	float GlobalCountdownLastUse;

	int32 CastAbilitySlot;
	float CastTimeRemaining;
	ACharacterBase* AbilityTarget;

	void StartGlobalCountdown();
	void StopGlobalCountdown();

	bool IsGlobalCountdown();
	float GetGlobalCountdownTimerRemaining();
	float GetGlobalCountdownCountdownPercent();

	bool IsAbilityCountdown(int32 Slot);
	bool IsAbilityCountdown(FAbility Ability);
	float GetAbilityCountdownTimerRemaining(int32 Slot);
	float GetAbilityCountdownTimerRemaining(FAbility Ability);
	float GetAbilityCountdownCountdownPercent(int32 Slot);
	float GetAbilityCountdownCountdownPercent(FAbility Ability);

	bool MightCast(int32 Slot);
	void StartCast(int32 CastAbilitySlot);
	void StopCast();

	void AddAbility(FAbility Ability, int32 Slot);
	void RemoveAbility(int32 Slot);

	EAbilityValidation ValidateAbility(int32 Slot);

	UFUNCTION()
	void CommitAbility(int32 Slot);

public:

	UPROPERTY(VisibleAnywhere, BlueprintAssignable, Category = "Abilities")
	FSpawnProjectileDelegate SpawnProjectileDelegate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Abilities")
	TMap<int32, FAbility> Abilities;

	AAbilityCharacter();

	void BeginPlay() override;

	void Tick(float DeltaTime) override;

	void SetupAbilities();

	void UpdateAbilityTimers(float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void AddAbilityModifiers(ACharacterBase* TargetCharacter, FAbility Ability);

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void AddDelayedAbilityModifiers(ACharacterBase* TargetCharacter, FAbility Ability);


	UFUNCTION(BlueprintCallable, Category = "Abilities")
	bool IsCountdown(int32 Slot);

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	bool IsCasting() { return CastAbilitySlot != -1; }

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	float GetCastTimeRemaining();

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	float GetCastPercent();

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	float GetCountdownRemaining(int32 Slot);

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	float GetCountdownPercent(int32 Slot);

	EAbilityValidation StartAbility(int32 Slot);
	void StopAbility(int32 Slot);

	bool GetDirectionalHitTest(TArray<FHitResult> &OutResults, float Radius, float Range);

	FString GetValidationErrorText(EAbilityValidation Validation);

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetNormalState();
	void ServerSetNormalState_Implementation();

	UFUNCTION(client, Reliable)
	void ClientSetNormalState();
	void ClientSetNormalState_Implementation() { State = ECharacterState::CS_Idle; }
	bool ServerSetNormalState_Validate() { return true; }

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSpawnProjectile(ACharacterBase* TargetCharacter, EProjectileType ProjectileType);
	void MulticastSpawnProjectile_Implementation(ACharacterBase* TargetCharacter, EProjectileType ProjectileType);
};
