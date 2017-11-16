// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityBase.h"
#include "AbilityCleric2.generated.h"

/**
 * 
 */
UCLASS()
class VFS_API AAbilityCleric2 : public AAbilityBase
{
	GENERATED_BODY()

	AAbilityCleric2();

	virtual void BeginPlay() override;

	virtual void SetupModifiers() override;
};
