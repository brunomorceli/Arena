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

#include "ArenaPlayerState.h"
#include "ArenaSaveGame.h"
#include "CharacterBase.generated.h"

UENUM(BlueprintType)
enum EHUDMessage
{
	Info UMETA(DisplayName = "Info"),
	Success UMETA(DisplayName = "Success"),
	Warning UMETA(DisplayName = "Warning"),
	Danger UMETA(DisplayName = "Danger"),
};

UENUM(BlueprintType)
enum ECharacterState
{
	CS_Idle UMETA(DisplayName = "Idle"),
	CS_Move UMETA(DisplayName = "Move"),
	CS_Jump UMETA(DisplayName = "Jump"),
	CS_Stuck UMETA(DisplayName = "Stuck"),
	CS_Cast UMETA(DisplayName = "Cast"),
	CS_Channeling UMETA(DisplayName = "Channeling"),
	CS_Stun UMETA(DisplayName = "Stun"),
	CS_Invunerable UMETA(DisplayName = "Invunerable"),
	CS_Death UMETA(DisplayName = "Death"),
	CS_Spectate UMETA(DisplayName = "Spectate"),
};

UENUM(BlueprintType)
enum ECharacterAnimationState
{
	CAS_Idle UMETA(DisplayName = "Idle"),
	CAS_Damage UMETA(DisplayName = "Damage"),
	CAS_Move UMETA(DisplayName = "Move"),
	CAS_Cast UMETA(DisplayName = "Cast"),
	CAS_Channeling UMETA(DisplayName = "Channeling"),
	CAS_AttackRange UMETA(DisplayName = "AttackRange"),
	CAS_AttackMelee UMETA(DisplayName = "AttackMelee"),
	CAS_Stun UMETA(DisplayName = "Stun"),
	CAS_Incapacitate UMETA(DisplayName = "Incapacidate"),
	CAS_Death UMETA(DisplayName = "Death"),
	CAS_Stop UMETA(DisplayName = "Stop"),
};

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

// ============================================================================================================================
// CLASS
// ============================================================================================================================

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNotificationDelegate, FString, Message);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFXNotificationDelegate, int32, Slot);

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
	FFXNotificationDelegate FXNotificationDelegate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	class UParticleSystemComponent* ChestParticle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	class UParticleSystemComponent* LeftHandParticle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	class UParticleSystemComponent* RightHandParticle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	class UParticleSystemComponent* AuraParticle;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	class USkeletalMeshComponent* LeftHandWeapon;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	class USkeletalMeshComponent* RightHandWeapon;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	class USkeletalMeshComponent* BackWeapon;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "State")
	TEnumAsByte<ECharacterState> State;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "State")
	FString Name;

	UPROPERTY(ReplicatedUsing = OnRep_PlayerClass, VisibleAnywhere, BlueprintReadOnly, Category = "State")
	TEnumAsByte<ECharacterClass> PlayerClass;

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	ACharacterBase* Target;

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
	void GetTarget();

	UFUNCTION(BlueprintCallable, Category = "Utils")
	FText GetStatusText(FStatus Status) { return Status.GetStatusText(); }

	UFUNCTION(BlueprintCallable, Category = "Utils")
	float GetStatusPercent(FStatus Status) { return Status.GetPercent(); }

	UFUNCTION(BlueprintCallable, Category = "Utils")
	float GetStatusPercentProgressBar(FStatus Status) { return Status.GetPercentProgressBar(); }

	UFUNCTION(BlueprintCallable, Category = "Utils")
	FText GetPercentText(FStatus Status) { return Status.GetPercentText(); }

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Utils")
	void ServerGetTarget();
	void ServerGetTarget_Implementation() { ClientGetTarget(); }
	bool ServerGetTarget_Validate() { return true; }

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Utils")
	void ServerSetSelfTarget();
	void ServerSetSelfTarget_Implementation() { ClientSetSelfTarget(); }
	bool ServerSetSelfTarget_Validate() { return true; }

	UFUNCTION(BlueprintCallable, Client, Reliable, Category = "Utils")
	void ClientSetSelfTarget();
	void ClientSetSelfTarget_Implementation() { ServerSetTarget(this); }

	UFUNCTION(BlueprintCallable, Client, Reliable, Category = "Utils")
	void ClientClearTarget();
	void ClientClearTarget_Implementation() { ServerSetTarget(NULL); }
	
	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Utils")
	void ServerSetTarget(ACharacterBase* Character);
	void ServerSetTarget_Implementation(ACharacterBase* Character);
	bool ServerSetTarget_Validate(ACharacterBase* Character) { return true; }

	UFUNCTION(BlueprintCallable, Client, Reliable, Category = "Utils")
	void ClientGetTarget();
	void ClientGetTarget_Implementation();

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Utils")
	void ServerEscape();
	void ServerEscape_Implementation() { ClientClearTarget(); }
	bool ServerEscape_Validate() { return true; }

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRespawn();
	void ServerRespawn_Implementation();
	bool ServerRespawn_Validate() { return true; }

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerAddScore();
	void ServerAddScore_Implementation();
	bool ServerAddScore_Validate() { return true; }

	void ShowMessage(const FString &Notification, EHUDMessage MessageType);

	UFUNCTION(BlueprintCallable, Client, Reliable, Category = "Notification")
	void ClientNotification(const FString& Notification);
	void ClientNotification_Implementation(const FString& Notification);

	UFUNCTION(BlueprintCallable, Category = "Camera")
	void SetActorRotToFollowCameraAngle();

	bool ConicalHitTest(TArray<ACharacterBase*> &Characters, float Radius, float Range);

	bool AreaHitTest(TArray<ACharacterBase*> &Characters, float Radius);

	bool DirectionalHitTest(TArray<ACharacterBase*> &Characters, float Radius, float Range);

	float GetAngle(ACharacterBase* Observer, ACharacterBase* Target);

	UFUNCTION(BlueprintCallable, Category = "Utils")
	bool FindFloor(FVector& Result, float Depth);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetChestParticle(UParticleSystem* Particlebool, bool Clear);
	void MulticastSetChestParticle_Implementation(UParticleSystem* Particle, bool Clear);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetHandsParticles(UParticleSystem* LeftHand, UParticleSystem* RightHand);
	void MulticastSetHandsParticles_Implementation(UParticleSystem* LeftHand, UParticleSystem* RightHand);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetAuraParticle(UParticleSystem* Particle);
	void MulticastSetAuraParticle_Implementation(UParticleSystem* Particle);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetRotation(FRotator Rotation);
	void ServerSetRotation_Implementation(FRotator Rotation) { SetActorRotation(Rotation); }
	bool ServerSetRotation_Validate(FRotator Rotation) { return true; }

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
	void ServerSetAsSpectator(bool NewState);
	void ServerSetAsSpectator_Implementation(bool NewState);
	bool ServerSetAsSpectator_Validate(bool NewState) { return true; }

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
	void ServerTeleportToTarget();
	void ServerTeleportToTarget_Implementation();
	bool ServerTeleportToTarget_Validate() { return true; }

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
	void ServerJumpTo();
	void ServerJumpTo_Implementation();
	bool ServerJumpTo_Validate() { return true; }

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
	void ServerChangeTeam(EPlayerTeam NewTeam);
	void ServerChangeTeam_Implementation(EPlayerTeam NewTeam);
	bool ServerChangeTeam_Validate(EPlayerTeam NewTeam) { return true; }

	UFUNCTION(NetMulticast, Reliable)
	void MulticastFXNotification(int32 Slot);
	void MulticastFXNotification_Implementation(int32 Slot);


	void ChangeState(ECharacterState NewState);

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;
};
