// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

#include "CharacterBase.h"
#include "AbilityFXBase.generated.h"

class ACharacterBase;

UCLASS(BlueprintType)
class VFS_API AAbilityFXBase : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	AAbilityFXBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Target")
	ACharacterBase* Target;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	bool bIsLoop;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	FName BuffName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	float Delay;

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	virtual void Stop();

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void PlayRandomSound(TArray<USoundBase*> Sounds, FVector Location);
};
