// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ArenaCharacter.h"
#include "GlobalLibrary.h"
#include "CharacterWizard.generated.h"

/**
*
*/
UCLASS()
class VFS_API ACharacterWizard : public AArenaCharacter
{
	GENERATED_BODY()

	ACharacterWizard();

	virtual void BeginPlay() override;

	virtual void SetAbility1() override;
	virtual void SetAbility2() override;
	virtual void SetAbility3() override;
	virtual void SetAbility4() override;
	virtual void SetAbility5() override;
	virtual void SetAbility6() override;
	virtual void SetAbility7() override;
	virtual void SetAbility8() override;
};
