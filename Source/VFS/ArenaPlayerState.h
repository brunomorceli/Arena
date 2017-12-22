// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Core.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"

#include "Enums.h"
#include "ArenaGameInstance.h"
#include "ArenaPlayerState.generated.h"


USTRUCT(BlueprintType)
struct FChatMsg
{
	GENERATED_USTRUCT_BODY(BlueprintType)

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
		AActor* Sender;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
		FText Message;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNotifyChatMsgDelegate, const FChatMsg, ChatMsg);

UCLASS()
class VFS_API AArenaPlayerState : public APlayerState
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable, VisibleAnywhere, Category = "Chat")
	FNotifyChatMsgDelegate NotifyChatMsgDelegate;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Match")
	TEnumAsByte<ECharacterClass> CharacterClass;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Match")
	TEnumAsByte<EPlayerTeam> Team;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Match")
	int32 Kills;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Match")
	int32 Deaths;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Statistics")
	float DamageDone;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Statistics")
	float DamageTaken;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Statistics")
	float HealingDone;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Statistics")
	float HealingTaken;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Statistics")
	float MaxDamage;

	AArenaPlayerState(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
	void ServerChangeTeam(EPlayerTeam NewTeam);
	void ServerChangeTeam_Implementation(EPlayerTeam NewTeam);
	bool ServerChangeTeam_Validate(EPlayerTeam NewTeam) { return true; }

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
	void ServerSetPlayerName(const FString &NewName);
	void ServerSetPlayerName_Implementation(const FString &NewName) { SetPlayerName(NewName); }
	bool ServerSetPlayerName_Validate(const FString &NewName) { return true; }

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
	void ServerSetClass(ECharacterClass NewClass);
	void ServerSetClass_Implementation(ECharacterClass NewClass);
	bool ServerSetClass_Validate(ECharacterClass NewClass) { return true; }

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
	void ServerAddKill();
	void ServerAddKill_Implementation() { Kills++; }
	bool ServerAddKill_Validate() { return true; }

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
	void ServerAddDeath();
	void ServerAddDeath_Implementation() { Deaths++; }
	bool ServerAddDeath_Validate() { return true; }


	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Network")
	void ServerNotifyChatMsg(AActor* Sender, const FText& Message);
	void ServerNotifyChatMsg_Implementation(AActor* Sender, const FText& Message);
	bool ServerNotifyChatMsg_Validate(AActor* Sender, const FText& Message) { return true; }

	UFUNCTION(Client, Reliable, Category = "Network")
	void ClientNotifyChatMsg(const FChatMsg& ChatMsg);
	void ClientNotifyChatMsg_Implementation(const FChatMsg& ChatMsg) { NotifyChatMsgDelegate.Broadcast(ChatMsg); }
};
