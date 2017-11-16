// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityBase.h"
#include "AbilityWizard1.generated.h"

/**
 * 
 */
UCLASS()
class VFS_API AAbilityWizard1 : public AAbilityBase
{
	GENERATED_BODY()

	AAbilityWizard1();

	virtual void BeginPlay() override;

	virtual void SetupModifiers() override;
};
