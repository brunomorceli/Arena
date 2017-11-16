// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityBase.h"
#include "AbilityAssassin6.generated.h"

/**
 * 
 */
UCLASS()
class VFS_API AAbilityAssassin6 : public AAbilityBase
{
	GENERATED_BODY()

	AAbilityAssassin6();

	virtual void BeginPlay() override;

	virtual void SetupModifiers() override;
};
