// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityBase.h"
#include "AbilityWarrior2.generated.h"

/**
 * 
 */
UCLASS()
class VFS_API AAbilityWarrior2 : public AAbilityBase
{
	GENERATED_BODY()

	AAbilityWarrior2();

	virtual void BeginPlay() override;

	virtual void SetupModifiers() override;

	virtual void OnStart(ACharacterBase* Target) override;
};
