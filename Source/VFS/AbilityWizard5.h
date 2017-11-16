// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityBase.h"
#include "AbilityWizard5.generated.h"

/**
 * 
 */
UCLASS()
class VFS_API AAbilityWizard5 : public AAbilityBase
{
	GENERATED_BODY()

	AAbilityWizard5();

	virtual void BeginPlay() override;

	virtual void SetupModifiers() override;
};
