// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Core.h"

#include "AbilityBase.h"
#include "Ability2.generated.h"

/**
 * 
 */
UCLASS()
class VFS_API AAbility2 : public AAbilityBase
{
	GENERATED_BODY()

public:
	AAbility2();
	
	virtual void SetupModifiers() override;
};
