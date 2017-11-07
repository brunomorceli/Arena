// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Core.h"
#include "GameFramework/Actor.h"
#include "CharacterBase.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Runtime/Engine/Classes/Particles/ParticleSystemComponent.h"
#include "Runtime/Engine/Classes/Components/SphereComponent.h"

#include "CharacterBase.h"
#include "AbilityProjectile.generated.h"

UENUM(BlueprintType)
enum EProjectileType
{
	PTP_Frost UMETA(DisplayName = "Frost"),
	PTP_Fire UMETA(DisplayName = "Fire"),
	PTP_Water UMETA(DisplayName = "Water"),
	PTP_Shadow UMETA(DisplayName = "Shadow"),
	PTP_Earth UMETA(DisplayName = "Earth"),
};

UCLASS()
class VFS_API AAbilityProjectile : public AActor
{
	GENERATED_BODY()
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private: 

	FRotator LookToward(AActor* ViewerActor, AActor* TargetActor);

	void UpdateLookToward();
	void UpdatePosition(float DeltaTime);

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:	

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Collision)
	class USphereComponent* CollisionSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Target")
	UParticleSystem* HitParticle;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Target")
	ACharacterBase* Target;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Target")
	float Speed;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Target")
	float TimeRemaining;

	// Sets default values for this actor's properties
	AAbilityProjectile();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Particle")
	TEnumAsByte<EProjectileType> Type;
};
