// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Runtime/Engine/Classes/Engine/Blueprint.h"
#include "Runtime/Engine/Classes/GameFramework/GameModeBase.h"
#include "Runtime/Engine/Classes/GameFramework/PlayerState.h"
#include "Runtime/Engine/Classes/Particles/ParticleSystem.h"
#include "Runtime/Engine/Classes/Particles/ParticleSystemComponent.h"
#include "Runtime/Engine/Classes/GameFramework/PlayerStart.h"
#include "Runtime/Engine/Classes/Animation/AnimSequence.h"
#include "Runtime/Engine/Public/TimerManager.h"

#include "Enums.h"
#include "ArenaPlayerState.h"
#include "ArenaSaveGame.h"
#include "CharacterBase.generated.h"

USTRUCT(BlueprintType)
struct FStatus
{
	GENERATED_USTRUCT_BODY(BlueprintType)

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	float Min;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	float Max;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	float MaxBase;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	float Value;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	float Regeneration;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	float RegenerationBase;

	TMap<int32, float> MaxMultipliers;
	TMap<int32, float> RegenerationMultipliers;

	void Setup(float MaxAmount, float RegenerationAmount=0.0f)
	{
		Min = 0.0f;
		MaxBase = MaxAmount;
		Max = MaxBase;
		Value = Max;

		RegenerationBase = RegenerationAmount;
		Regeneration = RegenerationBase;
	}

	void Reset()
	{
		MaxMultipliers.Empty();
		RegenerationMultipliers.Empty();

		Max = MaxBase;
		Regeneration = RegenerationBase;
		Value = Max;
	}

	float GetAmountByPercent(float MaxVal, float Percent)
	{
		return (MaxVal / 100.0f) * FMath::Abs(Percent);
	}

	float GetPercentAmount(float Percent)
	{
		return (Max / 100.0f) * Percent;
	}

	bool GotAmount(float Amount, float Percent=false) {
		return (Percent ? GetPercentAmount(Amount) : Value) >= Amount;
	}

	float Clamp(float Amount, float bIsPercent=false)
	{
		if (!bIsPercent) { Value = FMath::Clamp(Value + Amount, Min, Max); }
		else { Value = FMath::Clamp(Value + GetAmountByPercent(Value, Amount), Min, Max); }

		return Value;
	}

	float Damage(float Amount, float bIsPercent=false) { return Clamp(-Amount, bIsPercent); }

	float Heal(float Amount, float bIsPercent=false) { return Clamp(Amount, bIsPercent); }

	void AddMaxMultiplier(int32 UniqueID, float Amount, bool bIsPercent = false)
	{
		if (!MaxMultipliers.Contains(UniqueID)) { MaxMultipliers.Add(UniqueID, 0.0f); }

		float MultipliedAmount = bIsPercent ? GetAmountByPercent(Max, Amount) : Amount;
		MaxMultipliers[UniqueID] += MultipliedAmount;
		Max += MultipliedAmount;
	}

	void RemoveMaxMultiplier(int32 UniqueID)
	{
		if (!MaxMultipliers.Contains(UniqueID)) { return; }

		Max -= MaxMultipliers[UniqueID];
		MaxMultipliers.Remove(UniqueID);
		Value = FMath::Clamp(Value, Min, Max);
	}

	void AddRegenerationMultiplier(int32 UniqueID, float Amount)
	{
		if (!MaxMultipliers.Contains(UniqueID)) { MaxMultipliers.Add(UniqueID, 0.0f); }
	}

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

	float GetModifierAmount(float Amount, bool bPercent=false)
	{
		return bPercent ? ((Max / 100.0f) * Amount) : Amount;
	}
};

USTRUCT(BlueprintType)
struct FAnimation
{
	GENERATED_USTRUCT_BODY(BlueprintType)

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	int32 bLoop = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	float Duration = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	UAnimSequence* AnimSequence = NULL;

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
};

// ============================================================================================================================
// CLASS
// ============================================================================================================================

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNotificationDelegate, FString, Message);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FChangeAnimStateDelegate, ECharacterAnimationState, NewAnimState, FAnimation, Animation);

UCLASS(config = Game)
class ACharacterBase : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	void UpdateAuraParticle();

	UFUNCTION()
	virtual void OnRep_PlayerClass();

public:

	UPROPERTY(VisibleAnywhere, BlueprintAssignable, Category = "Notification")
	FNotificationDelegate NotificationDelegate;

	UPROPERTY(VisibleAnywhere, BlueprintAssignable, Category = "Notification")
	FChangeAnimStateDelegate ChangeAnimStateDelegate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	class UParticleSystemComponent* ChestParticle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	class UParticleSystemComponent* LeftHandParticle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	class UParticleSystemComponent* RightHandParticle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	class UParticleSystemComponent* AuraParticle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	class UParticleSystemComponent* CharacterTrail;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	class USkeletalMeshComponent* LeftHandWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	class UParticleSystemComponent* LeftHandWeaponTrail;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	class USkeletalMeshComponent* RightHandWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	class UParticleSystemComponent* RightHandWeaponTrail;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	class USkeletalMeshComponent* BackWeapon;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "State")
	TEnumAsByte<ECharacterState> State;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "State")
	TEnumAsByte<ECharacterAnimationState> AnimationState;
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "State")
	FString Name;

	UPROPERTY(ReplicatedUsing = OnRep_PlayerClass, VisibleAnywhere, BlueprintReadOnly, Category = "State")
	TEnumAsByte<ECharacterClass> PlayerClass;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	TEnumAsByte<EPlayerTeam> Team;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	FStatus Health;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	FStatus Mana;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	FStatus Energy;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	FStatus Speed;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	float Critical;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	FStatus Power;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	FStatus Defense;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	FStatus MagicAbsorbing;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	FStatus PhyicalAbsorbing;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	ACharacterBase* Target;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	float TargetMaxDistance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	float DeathDelay;

	ACharacterBase();

	void Tick(float DeltaTime) override;

	virtual void BeginPlay() override;

protected:
	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/**
	* Called via input to turn at a given rate.
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void TurnAtRate(float Rate);

	/**
	* Called via input to turn look up/down at a given rate.
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void LookUpAtRate(float Rate);

	virtual void Jump() override;
	virtual void StopJumping() override;

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

	float RegenerationInterval;
	FTimerHandle RegenerationTimer;

	void RegenerateStatus();

	virtual void UpdateState();
	
	void SetDeadState();

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UFUNCTION(BlueprintCallable, Category="Utils")
	ACharacter* GetNearestCharacter();

	UFUNCTION(BlueprintCallable, Category = "Utils")
	FText GetStatusText(FStatus Status) { return Status.GetStatusText(); }

	UFUNCTION(BlueprintCallable, Category = "Utils")
	float GetStatusPercent(FStatus Status) { return Status.GetPercent(); }

	UFUNCTION(BlueprintCallable, Category = "Utils")
	float GetStatusPercentProgressBar(FStatus Status) { return Status.GetPercentProgressBar(); }

	UFUNCTION(BlueprintCallable, Category = "Utils")
	FText GetPercentText(FStatus Status) { return Status.GetPercentText(); }

	// ===================================================================================================================
	// UTILITY METHODS
	// ===================================================================================================================
	
	UFUNCTION(BlueprintCallable, Category = "Utils")
	float GetUMGTargetScale();

	UFUNCTION(BlueprintCallable, Category = "Utils")
	void SetActorRotToFollowCameraAngle();

	UFUNCTION(BlueprintCallable, Category = "Utils")
	bool ConicalHitTest(TArray<ACharacterBase*> &Characters, float Radius, float Range);

	UFUNCTION(BlueprintCallable, Category = "Utils")
	bool AreaHitTest(TArray<ACharacterBase*> &Characters, float Radius);

	UFUNCTION(BlueprintCallable, Category = "Utils")
	bool DirectionalHitTest(TArray<ACharacterBase*> &Characters, float Radius, float Range);

	UFUNCTION(BlueprintCallable, Category = "Utils")
	float GetAngle(ACharacterBase* CharacterObserver, ACharacterBase* CharacterTarget);

	UFUNCTION(BlueprintCallable, Category = "Utils")
	bool FindFloor(FVector& Result, float Depth);

	// ===================================================================================================================
	// NETWORK METHODS
	// ===================================================================================================================

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Network")
	void ServerSearchTarget();
	void ServerSearchTarget_Implementation() { ClientSearchTarget(); }
	bool ServerSearchTarget_Validate() { return true; }

	UFUNCTION(BlueprintCallable, Client, Reliable, Category = "Network")
	void ClientSearchTarget();
	void ClientSearchTarget_Implementation();

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Network")
	void ServerSetTarget(ACharacterBase* Character);
	void ServerSetTarget_Implementation(ACharacterBase* Character);
	bool ServerSetTarget_Validate(ACharacterBase* Character) { return true; }

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Network")
	void MulticastSetTarget(ACharacterBase* Character);
	void MulticastSetTarget_Implementation(ACharacterBase* Character);
	bool MulticastSetTarget_Validate(ACharacterBase* Character) { return true; }

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Network")
	void ServerSetSelfTarget();
	void ServerSetSelfTarget_Implementation() { ServerSetTarget(this); }
	bool ServerSetSelfTarget_Validate() { return true; }

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Network")
	void ServerEscape();
	void ServerEscape_Implementation() { ServerSetTarget(NULL); }
	bool ServerEscape_Validate() { return true; }

	UFUNCTION(Server, Reliable, WithValidation, Category = "Network")
	void ServerRespawn();
	void ServerRespawn_Implementation();
	bool ServerRespawn_Validate() { return true; }

	UFUNCTION(BlueprintCallable, Client, Reliable, Category = "Network")
	void ClientNotification(const FString& Notification);
	void ClientNotification_Implementation(const FString& Notification) { NotificationDelegate.Broadcast(Notification); }

	UFUNCTION(Server, Reliable, WithValidation, Category = "Network")
	void ServerSetRotation(FRotator Rotation);
	void ServerSetRotation_Implementation(FRotator Rotation) { SetActorRotation(Rotation); }
	bool ServerSetRotation_Validate(FRotator Rotation) { return true; }

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Network")
	void ServerTeleportTo(FVector Location, FRotator Rotator);
	void ServerTeleportTo_Implementation(FVector Location, FRotator Rotator);
	bool ServerTeleportTo_Validate(FVector Location, FRotator Rotator) { return true; }

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Network")
	void ServerJumpTo(FVector Location);
	void ServerJumpTo_Implementation(FVector Location);
	bool ServerJumpTo_Validate(FVector Location) { return true; }

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Network")
	void ServerKnockBack(FVector CenterLocation, float Distance);
	void ServerKnockBack_Implementation(FVector CenterLocation, float Distance);
	bool ServerKnockBack_Validate(FVector CenterLocation, float Distance) { return true; }

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Network")
	void ServerSetTeam(EPlayerTeam NewTeam);
	void ServerSetTeam_Implementation(EPlayerTeam NewTeam);
	bool ServerSetTeam_Validate(EPlayerTeam NewTeam) { return true; }

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Network")
	void ServerSetAnimState(ECharacterAnimationState NewAnimState, FAnimation Animation);
	void ServerSetAnimState_Implementation(ECharacterAnimationState NewAnimState, FAnimation Animation) { MulticastSetAnimState(NewAnimState, Animation); }
	bool ServerSetAnimState_Validate(ECharacterAnimationState NewAnimState, FAnimation Animation) { return true; }

	UFUNCTION(NetMulticast, Reliable, Category = "Network")
	void MulticastSetAnimState(ECharacterAnimationState NewAnimState, FAnimation Animation);
	void MulticastSetAnimState_Implementation(ECharacterAnimationState NewAnimState, FAnimation Animation);

	UFUNCTION(NetMulticast, Reliable, Category = "Network")
	void MulticastSetPawn(TSubclassOf<ACharacterBase> Character);
	void MulticastSetPawn_Implementation(TSubclassOf<ACharacterBase> Character);

	UFUNCTION(NetMulticast, Reliable, Category = "Network")
	void MulticastSetChestParticle(UParticleSystem* Particlebool, bool Clear);
	void MulticastSetChestParticle_Implementation(UParticleSystem* Particle, bool Clear);

	UFUNCTION(NetMulticast, Reliable, Category = "Network")
	void MulticastSetHandsParticles(UParticleSystem* LeftHand, UParticleSystem* RightHand);
	void MulticastSetHandsParticles_Implementation(UParticleSystem* LeftHand, UParticleSystem* RightHand);

	UFUNCTION(NetMulticast, Reliable, Category = "Network")
	void MulticastSetAuraParticle(UParticleSystem* Particle);
	void MulticastSetAuraParticle_Implementation(UParticleSystem* Particle);


	UFUNCTION(NetMulticast, Reliable, Category = "Network")
	void MulticastSetMaxWalkSpeed(float Amount);
	void MulticastSetMaxWalkSpeed_Implementation(float Amount);

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;
};
