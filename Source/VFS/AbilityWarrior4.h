// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityBase.h"
#include "AbilityWarrior4.generated.h"

/**
 * 
 */
UCLASS()
class VFS_API AAbilityWarrior4 : public AAbilityBase
{
	GENERATED_BODY()

	AAbilityWarrior4();

	virtual void BeginPlay() override;

	virtual void SetupModifiers() override;
};
