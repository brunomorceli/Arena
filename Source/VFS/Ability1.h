// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Core.h"
#include "AbilityBase.h"
#include "Ability1.generated.h"

/**
 * 
 */
UCLASS()
class VFS_API AAbility1 : public AAbilityBase
{
	GENERATED_BODY()

public:
	AAbility1();

	virtual void BeginPlay() override;

	virtual void SetupModifiers() override;
};
