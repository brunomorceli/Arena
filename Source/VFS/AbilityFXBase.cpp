// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityFXBase.h"


// Sets default values
AAbilityFXBase::AAbilityFXBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bool bIsLoop = false;
	float TimeRemaining = 0.0f;
	float Delay = 0.0f;
}

// Called when the game starts or when spawned
void AAbilityFXBase::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AAbilityFXBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AAbilityFXBase::Stop() {}

void AAbilityFXBase::PlayRandomSound(TArray<USoundBase*> Sounds, FVector Location)
{
	UWorld* World = GetWorld();
	if (!World || Sounds.Num() == 0) { return; }

	UGameplayStatics::PlaySoundAtLocation(World, Sounds[FMath::RandRange(0, Sounds.Num() - 1)], Location);
}
