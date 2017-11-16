// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityBase.h"
#include "AbilityCleric1.generated.h"

/**
 * 
 */
UCLASS()
class VFS_API AAbilityCleric1 : public AAbilityBase
{
	GENERATED_BODY()

	AAbilityCleric1();

	virtual void BeginPlay() override;

	virtual void SetupModifiers() override;
};
