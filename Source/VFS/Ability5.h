// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Core.h"
#include "AbilityBase.h"
#include "Ability5.generated.h"

/**
 * 
 */
UCLASS()
class VFS_API AAbility5 : public AAbilityBase
{
	GENERATED_BODY()

public:
	AAbility5();

	virtual void BeginPlay() override;

	virtual void SetupModifiers() override;
};
