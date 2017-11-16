// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityBase.h"
#include "AbilityCleric5.generated.h"

/**
 * 
 */
UCLASS()
class VFS_API AAbilityCleric5 : public AAbilityBase
{
	GENERATED_BODY()

	AAbilityCleric5();

	virtual void BeginPlay() override;

	virtual void SetupModifiers() override;
};
