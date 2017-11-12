// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum ECharacterState
{
	CS_Idle UMETA(DisplayName = "Idle"),
	CS_Move UMETA(DisplayName = "Move"),
	CS_Jump UMETA(DisplayName = "Jump"),
	CS_Stuck UMETA(DisplayName = "Stuck"),
	CS_Stun UMETA(DisplayName = "Stun"),
	CS_Invunerable UMETA(DisplayName = "Invunerable"),
	CS_Death UMETA(DisplayName = "Death"),
	CS_Spectate UMETA(DisplayName = "Spectate"),
};

UENUM(BlueprintType)
enum ECharacterAnimationState
{
	CAS_TakeDamage UMETA(DisplayName = "TakeDamage"),
	CAS_Action UMETA(DisplayName = "Action"),
	CAS_Commit UMETA(DisplayName = "Commit"),
	CAS_Cast UMETA(DisplayName = "Cast"),
	CAS_None UMETA(DisplayName = "None"),
};

UENUM(BlueprintType)
enum EAbilityValidation
{
	ABV_Allowed UMETA(DisplayName = "Allowed"),
	ABV_InvalidAbility UMETA(DisplayName = "InvalidAbility"),
	ABV_NotReady UMETA(DisplayName = "NotReady"),
	ABV_Health UMETA(DisplayName = "Health"),
	ABV_Mana UMETA(DisplayName = "Mana"),
	ABV_Energy UMETA(DisplayName = "Energy"),
	ABV_MissTarget UMETA(DisplayName = "MissTarget"),
	ABV_InvalidTarget UMETA(DisplayName = "InvalidTarget"),
	ABV_TooFar UMETA(DisplayName = "TooFar"),
	ABV_TooClose UMETA(DisplayName = "ToClose"),
	ABV_Moving UMETA(DisplayName = "Moving"),
	ABV_OutOfSight UMETA(DisplayName = "OutOfSight"),
	ABV_Incapacitated UMETA(DisplayName = "Incapacitated"),
	ABV_InProgress UMETA(DisplayName = "InProgress"),
	ABV_Unknown UMETA(DisplayName = "Unknown"),
};

UENUM(BlueprintType)
enum EAbilitySchool
{
	ABS_Physical UMETA(DisplayName = "Physical"),
	ABS_Magic UMETA(DisplayName = "Magic"),
};


UENUM(BlueprintType)
enum EAbilityStartType
{
	ABST_Instant UMETA(DisplayName = "Instant"),
	ABST_Castable UMETA(DisplayName = "Castable"),
	ABST_Channeling UMETA(DisplayName = "Channeling"),
};

UENUM(BlueprintType)
enum EAbilityTargetType
{
	ABTT_Self UMETA(DisplayName = "Self"),
	ABTT_Enemy UMETA(DisplayName = "Enemy"),
	ABTT_Ally UMETA(DisplayName = "Ally"),
};

UENUM(BlueprintType)
enum EAbilityArea
{
	ABA_Directional UMETA(DisplayName = "Directional"),
	ABA_AreaOnEffect UMETA(DisplayName = "AreaOnEffect"),
	ABA_Target UMETA(DisplayName = "Target"),
};

UENUM(BlueprintType)
enum EAbilityCommit
{
	ABC_Instant UMETA(DisplayName = "Instant"),
	ABC_Projectile UMETA(DisplayName = "Projectile"),
};

UENUM(BlueprintType)
enum EAbilityProjectileType
{
	ABP_Chase UMETA(DisplayName = "Chase"),
	ABP_Lauch UMETA(DisplayName = "Lauch"),
};


UENUM(BlueprintType)
enum EModifierStatusType
{
	MST_Absolute UMETA(DisplayName = "Absolute"),
	MST_Percent UMETA(DisplayName = "Percent"),
};

UENUM(BlueprintType)
enum EModifierSchool
{
	MS_Physical UMETA(DisplayName = "Physical"),
	MS_Magic UMETA(DisplayName = "Magic"),
	MS_Nature UMETA(DisplayName = "Nature"),
};

UENUM(BlueprintType)
enum EModifierConstrain
{
	MDCT_None UMETA(DisplayName = "None"),
	MDCT_Unique UMETA(DisplayName = "Unique"),
	MDCT_UniqueByPlayer UMETA(DisplayName = "UniqueByPlayer"),
};

UENUM(BlueprintType)
enum EPlayerTeam
{
	EPT_Red UMETA(DisplayName = "Red"),
	EPT_Blue UMETA(DisplayName = "Blue"),
	EPT_Neutral UMETA(DisplayName = "Neutral"),
};


UENUM(BlueprintType)
enum ECharacterClass
{
	ECCL_None UMETA(DisplayName = "None"),
	ECCL_Assassin UMETA(DisplayName = "Assassin"),
	ECCL_Wizard UMETA(DisplayName = "Wizard"),
	ECCL_Cleric UMETA(DisplayName = "Cleric"),
	ECCL_Warrior UMETA(DisplayName = "Warrior"),
};







class VFS_API Enums
{
public:
	Enums();
	~Enums();
};
