// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Core.h"

#include "AbilityBase.h"
#include "Ability3.generated.h"

/**
 * 
 */
UCLASS()
class VFS_API AAbility3 : public AAbilityBase
{
	GENERATED_BODY()

public:
	AAbility3();
	
	virtual void SetupModifiers() override;

	virtual void OnStart(ACharacterBase* Target) override;
};
