// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityBase.h"
#include "AbilityWizard8.generated.h"

/**
 * 
 */
UCLASS()
class VFS_API AAbilityWizard8 : public AAbilityBase
{
	GENERATED_BODY()

	AAbilityWizard8();

	virtual void BeginPlay() override;

	virtual void SetupModifiers() override;
};
