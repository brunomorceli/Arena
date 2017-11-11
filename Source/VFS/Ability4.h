// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Core.h"

#include "AbilityBase.h"
#include "Ability4.generated.h"

/**
 * 
 */
UCLASS()
class VFS_API AAbility4 : public AAbilityBase
{
	GENERATED_BODY()
	
public:

	AAbility4();
	
	virtual void BeginPlay() override;

	virtual void SetupModifiers() override;
};
