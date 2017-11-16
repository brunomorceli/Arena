// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityBase.h"
#include "AbilityAssassin8.generated.h"

/**
 * 
 */
UCLASS()
class VFS_API AAbilityAssassin8 : public AAbilityBase
{
	GENERATED_BODY()

	AAbilityAssassin8();

	virtual void BeginPlay() override;

	virtual void SetupModifiers() override;
};
