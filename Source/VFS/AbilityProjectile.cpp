// Fill out your copyright notice in the Description page of Project Settings.
#include "AbilityProjectile.h"

// Sets default values
AAbilityProjectile::AAbilityProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	bAlwaysRelevant = true;
	bNetUseOwnerRelevancy = true;
	bNetLoadOnClient = true;

	Speed = 1200.0f;
	TimeRemaining = 7.0f;

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	CollisionSphere->SetupAttachment(RootComponent);
	CollisionSphere->InitSphereRadius(70.0f);
	CollisionSphere->SetCollisionProfileName("OverlapAll");
	CollisionSphere->bGenerateOverlapEvents = true;
	CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AAbilityProjectile::OnOverlapBegin);
}

// Called when the game starts or when spawned
void AAbilityProjectile::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AAbilityProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateLookToward();
	UpdatePosition(DeltaTime);

	TimeRemaining -= DeltaTime;
	if (TimeRemaining <= 0.0f) { Destroy(); }
}

FRotator AAbilityProjectile::LookToward(AActor* ViewerActor, AActor* TargetActor)
{
	if (!Target) { return ViewerActor->GetActorRotation(); }

	FVector ViewerLocation = ViewerActor->GetActorLocation();
	FVector TargetLocation = TargetActor->GetActorLocation();
	
	return UKismetMathLibrary::FindLookAtRotation(ViewerLocation, TargetLocation);
}

void AAbilityProjectile::UpdateLookToward()
{
	FRotator LookAt = LookToward(this, Target);
	SetActorRotation(LookAt);
}

void AAbilityProjectile::UpdatePosition(float DeltaTime)
{
	float Amount = Speed * DeltaTime;
	FVector ForwardVector = GetActorForwardVector();
	FVector Location = GetActorLocation();

	FVector NewPosition = (ForwardVector * Amount) + Location;

	SetActorLocation(NewPosition);
}

void AAbilityProjectile::OnOverlapBegin(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	if (!Target || OtherActor->GetUniqueID() != Target->GetUniqueID()) { return; }

	if (HitParticle) { Target->MulticastSetChestParticle(HitParticle, false); }

	Destroy();
}