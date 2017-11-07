// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityCharacter.h"

AAbilityCharacter::AAbilityCharacter()
{
	GlobalCountdownTime = 0.7f;
	GlobalCountdownLastUse = -3600.0f;

	CastAbilitySlot = -1;
	CastTimeRemaining = 0.0f;

	bReplicates = true;
	bAlwaysRelevant = true;
	bNetUseOwnerRelevancy = true;
	bNetLoadOnClient = true;
}

void AAbilityCharacter::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	if (!World) { return; }

	//FTimerHandle TimerHandle;
	//World->GetTimerManager().SetTimer(TimerHandle, this, &AAbilityCharacter::SetupAbilities, 5.0f, false);
	SetupAbilities();
}

void AAbilityCharacter::SetupAbilities()
{
	// Ability 01
	FAbility FirstBlood;
	//FirstBlood.EnergyCost.Value = 30.0f;
	FirstBlood.Area = EAbilityArea::AA_Target;
	FirstBlood.MaxDistance = 200.0f;
	FirstBlood.LoadIcon("/Game/Sprites/Icons/307.307");
	FirstBlood.Name = FName("First Blood");

	FInstantModifier FirstBloodDamage;
	FirstBloodDamage.Causer = Cast<ACharacterBase>(this);
	FirstBloodDamage.Health.Value = -100.0f;
	FirstBloodDamage.LoadIcon("/Game/Sprites/Icons/307.307");
	FirstBlood.InstantModifiers.Add(1, FirstBloodDamage);

	Abilities.Add(1, FirstBlood);

	// Ability 02
	FAbility Frostbolt;
	Frostbolt.ManaCost.Value = 150.0f;
	Frostbolt.UseType = EAbilityUseType::AUT_Castable;
	Frostbolt.CastTime = 2.0f;
	Frostbolt.Area = EAbilityArea::AA_Target;
	Frostbolt.MaxDistance = 1800.0f;
	Frostbolt.CommitType = EAbilityCommitType::ACT_Projectile;
	Frostbolt.ProjectileType = EProjectileType::PTP_Frost;
	Frostbolt.LoadIcon("/Game/Sprites/Icons/11.11");
	Frostbolt.Name = FName("Frostbolt");

	FInstantModifier FrostBoltDamage;
	FrostBoltDamage.Causer = Cast<ACharacterBase>(this);
	FrostBoltDamage.Health.Value = -300.0f;
	FrostBoltDamage.LoadIcon("/Game/Sprites/Icons/11.11");
	Frostbolt.InstantModifiers.Add(1, FrostBoltDamage);

	FBuffModifier FrostboltSlow;
	FrostboltSlow.Causer = Cast<ACharacterBase>(this);
	FrostboltSlow.Name = "Frostbolt Slow";
	FrostboltSlow.Description = "Slow the target by 50% for 6 seconds";
	FrostboltSlow.Speed.Value = -50.0f;
	FrostboltSlow.Speed.Type = EModifierStatusType::MST_Percent;
	FrostboltSlow.Duration = 6.0f;
	FrostboltSlow.LoadIcon("/Game/Sprites/Icons/123.123");
	Frostbolt.BuffModifiers.Add(1, FrostboltSlow);

	Abilities.Add(2, Frostbolt);

	// Ability 03
	FAbility Shadowbolt;
	Shadowbolt.ManaCost.Value = 150.0f;
	Shadowbolt.UseType = EAbilityUseType::AUT_Castable;
	Shadowbolt.CastTime = 2.0f;
	Shadowbolt.Area = EAbilityArea::AA_Target;
	Shadowbolt.MaxDistance = 1800.0f;
	Shadowbolt.CommitType = EAbilityCommitType::ACT_Projectile;
	Shadowbolt.ProjectileType = EProjectileType::PTP_Shadow;
	Shadowbolt.LoadIcon("/Game/Sprites/Icons/284.284");
	Shadowbolt.Name = FName("Shadowbolt");

	FInstantModifier ShadowboltDamage;
	ShadowboltDamage.Causer = Cast<ACharacterBase>(this);
	ShadowboltDamage.Health.Value = -300.0f;
	ShadowboltDamage.LoadIcon("/Game/Sprites/Icons/284.284");
	Shadowbolt.InstantModifiers.Add(1, ShadowboltDamage);

	FOvertimeModifier ShadowboltOvertime;
	ShadowboltOvertime.Causer = Cast<ACharacterBase>(this);
	ShadowboltOvertime.Name = "First Blood Overtime";
	ShadowboltOvertime.Health.Value = -100.0f;
	ShadowboltOvertime.Ticks = 5;
	ShadowboltOvertime.TickTime = 2.0f;
	ShadowboltOvertime.LoadIcon("/Game/Sprites/Icons/284.284");
	Shadowbolt.OvertimeModifiers.Add(1, ShadowboltOvertime);

	Abilities.Add(3, Shadowbolt);

	// Ability 04
	FAbility Stun;
	Stun.LoadIcon("/Game/Sprites/Icons/141.141");
	Stun.Name = FName("Stun");
	Stun.Area = EAbilityArea::AA_Target;
	Stun.UseType = EAbilityUseType::AUT_Instant;
	Stun.CountdownTime = 15.0f;
	Stun.MaxDistance = 300.0f;

	FBuffModifier StunModifier;
	StunModifier.Causer = Cast<ACharacterBase>(this);
	StunModifier.Name = "Stunned";
	StunModifier.LoadIcon("/Game/Sprites/Icons/141.141");
	StunModifier.Duration = 5.0f;
	StunModifier.State = ECharacterState::CS_Stun;
	Stun.BuffModifiers.Add(1, StunModifier);

	Abilities.Add(4, Stun);


	// Ability 05
	FAbility Frostnova;
	Frostnova.LoadIcon("/Game/Sprites/Icons/9.9");
	Frostnova.Name = FName("Frostnova");
	Frostnova.Area = EAbilityArea::AA_AreaOnEffect;
	Frostnova.UseType = EAbilityUseType::AUT_Instant;
	Frostnova.CountdownTime = 22.0f;
	Frostnova.MaxDistance = 400.0f;

	FBuffModifier FrostnovaModifier;
	FrostnovaModifier.Causer = Cast<ACharacterBase>(this);
	FrostnovaModifier.Name = "Frozen";
	FrostnovaModifier.Description = "Stuck the target by 7 seconds.";
	FrostnovaModifier.LoadIcon("/Game/Sprites/Icons/9.9");
	FrostnovaModifier.Duration = 7.0f;
	FrostnovaModifier.State = ECharacterState::CS_Stuck;
	Frostnova.BuffModifiers.Add(1, FrostnovaModifier);

	Abilities.Add(5, Frostnova);

	// Ability 06
	FAbility Fireball;
	Fireball.ManaCost.Value = 150.0f;
	Fireball.UseType = EAbilityUseType::AUT_Instant;
	Fireball.CountdownTime = 5.0f;
	Fireball.Area = EAbilityArea::AA_Target;
	Fireball.MaxDistance = 1800.0f;
	Fireball.CommitType = EAbilityCommitType::ACT_Projectile;
	Fireball.ProjectileType = EProjectileType::PTP_Fire;
	Fireball.LoadIcon("/Game/Sprites/Icons/309.309");
	Fireball.Name = FName("Fireball");

	FInstantModifier FireballDamage;
	FireballDamage.Causer = Cast<ACharacterBase>(this);
	FireballDamage.Health.Value = -300.0f;
	FireballDamage.LoadIcon("/Game/Sprites/Icons/11.11");
	Fireball.InstantModifiers.Add(1, FireballDamage);

	Abilities.Add(6, Fireball);
}

void AAbilityCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateAbilityTimers(DeltaTime);
}

void AAbilityCharacter::UpdateAbilityTimers(float DeltaTime)
{
	if (!IsCasting()) { return; }

	CastTimeRemaining = FMath::Max(CastTimeRemaining - DeltaTime, 0.0f);

	if (CastTimeRemaining == 0.0f) { return CommitAbility(CastAbilitySlot); };

	if (State == ECharacterState::CS_Death || State == ECharacterState::CS_Stun)
	{
		return StopCast();
	}

	float CastAngle = GetTargetAngle();
	float CastMaxAngle = Abilities[CastAbilitySlot].CastMaxAngle;
	float bIsMoving = GetVelocity().Size() > 30.0f;

	// are moving or are out of angle
	if (bIsMoving || CastAngle  > CastMaxAngle)
	{
		return StopCast();
	}
}

void AAbilityCharacter::StartGlobalCountdown()
{
	UWorld* World = GetWorld();
	if (World)
	{
		GlobalCountdownLastUse = UGameplayStatics::GetTimeSeconds(World);
	}
}

void AAbilityCharacter::StopGlobalCountdown()
{
	GlobalCountdownLastUse = -3600.0f;
}

bool AAbilityCharacter::IsGlobalCountdown()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return true;
	}

	float CurrentTime = UGameplayStatics::GetTimeSeconds(World);
	float ElapsedTime = CurrentTime - GlobalCountdownLastUse;

	return ElapsedTime < GlobalCountdownTime;
}

float AAbilityCharacter::GetGlobalCountdownTimerRemaining()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return 0.0f;
	}

	float CurrentTime = UGameplayStatics::GetTimeSeconds(World);
	float ElapsedTime = CurrentTime - GlobalCountdownLastUse;
	float Remaining = GlobalCountdownTime - ElapsedTime;
	return FMath::Max(Remaining, 0.0f);
}

float AAbilityCharacter::GetGlobalCountdownCountdownPercent()
{
	float Remaining = GetGlobalCountdownTimerRemaining();
	if (Remaining == 0.0f) { return Remaining; }

	return ((Remaining * 100.0f) / GlobalCountdownTime);
}

bool AAbilityCharacter::IsAbilityCountdown(int32 Slot) {
	return Abilities.Contains(Slot)
		? IsAbilityCountdown(Abilities[Slot])
		: false;
}

bool AAbilityCharacter::IsAbilityCountdown(FAbility Ability)
{
	UWorld* World = GetWorld();
	if (!World) { return true; }

	float CurrentTime = UGameplayStatics::GetTimeSeconds(World);
	float ElapsedTime = CurrentTime - Ability.LastUseTime;

	return ElapsedTime < Ability.CountdownTime;
}

float AAbilityCharacter::GetAbilityCountdownTimerRemaining(int32 Slot) {
	return Abilities.Contains(Slot)
		? GetAbilityCountdownTimerRemaining(Abilities[Slot])
		: 0.0f;
}

float AAbilityCharacter::GetAbilityCountdownTimerRemaining(FAbility Ability)
{
	UWorld* World = GetWorld();
	if (!World || Ability.CountdownTime == 0.0f) { return 0.0f; }

	float CurrentTime = UGameplayStatics::GetTimeSeconds(World);
	float ElapsedTime = CurrentTime - Ability.LastUseTime;
	float Remaining = Ability.CountdownTime - ElapsedTime;
	return FMath::Max(Remaining, 0.0f);
}

float AAbilityCharacter::GetAbilityCountdownCountdownPercent(int32 Slot)
{
	return Abilities.Contains(Slot)
		? GetAbilityCountdownCountdownPercent(Abilities[Slot])
		: 0.0f;
}

float AAbilityCharacter::GetAbilityCountdownCountdownPercent(FAbility Ability)
{
	float Remaining = GetAbilityCountdownTimerRemaining(Ability);
	if (Remaining == 0.0f) { return Remaining; }

	return ((Remaining * 100.0f) / Ability.CountdownTime);
}

void AAbilityCharacter::StartCast(int32 Slot)
{
	CastTimeRemaining = Abilities[Slot].CastTime;
	CastAbilitySlot = Slot;
	State = ECharacterState::CS_Cast;
}

void AAbilityCharacter::StopCast()
{
	if (State == ECharacterState::CS_Cast)
	{
		State = ECharacterState::CS_Idle;
	}

	CastTimeRemaining = 0.0f;
	CastAbilitySlot = -1;
}

void AAbilityCharacter::AddAbility(FAbility Ability, int32 Slot)
{
	Ability.Slot = Slot;
	Abilities.Add(Slot, Ability);
}

void AAbilityCharacter::RemoveAbility(int32 Slot)
{
	Abilities.Remove(Slot);
}

bool AAbilityCharacter::IsCountdown(int32 Slot)
{
	if (!Abilities.Contains(Slot)) { return false; }

	FAbility Ability = Abilities[Slot];
	if (Ability.UseType == EAbilityUseType::AUT_Instant && IsGlobalCountdown()) { return true; }

	return IsAbilityCountdown(Ability);
}

float AAbilityCharacter::GetCastTimeRemaining()
{
	return CastTimeRemaining;
}

float AAbilityCharacter::GetCastPercent()
{
	if (CastAbilitySlot < 1) { return 0.0f; }

	float CastTime = Abilities[CastAbilitySlot].CastTime;
	float Elapsed = CastTime - CastTimeRemaining;

	return (Elapsed * 100.0f) / CastTime;
}

float AAbilityCharacter::GetCountdownRemaining(int32 Slot)
{
	if (!Abilities.Contains(Slot)) { return 0.0f; }

	if (Abilities[Slot].UseType == EAbilityUseType::AUT_Instant)
	{
		float GlobalCountdownTimerRemaining = GetGlobalCountdownTimerRemaining();
		if (GlobalCountdownTimerRemaining)
		{
			return GlobalCountdownTimerRemaining;
		}
	}

	return GetAbilityCountdownTimerRemaining(Abilities[Slot]);
}

float AAbilityCharacter::GetCountdownPercent(int32 Slot)
{
	if (!Abilities.Contains(Slot)) { return 0.0f; }

	if (Abilities[Slot].UseType == EAbilityUseType::AUT_Instant)
	{
		float GCDTimeRemaining = GetGlobalCountdownTimerRemaining();
		if (GCDTimeRemaining > 0.0f)
		{
			return ((GCDTimeRemaining * 100.0f) / GlobalCountdownTime);
		}
	}

	float Remaining = GetCountdownRemaining(Slot);
	if (Remaining <= 0.0f) { return 0.0f; };

	FAbility Ability = Abilities[Slot];
	return ((Remaining * 100.0f) / Ability.CountdownTime);
}

EAbilityValidation AAbilityCharacter::ValidateAbility(int32 Slot)
{
	// General Validation
	if (!Abilities.Contains(Slot)) { return AV_InvalidAbility; }
	FAbility Ability = Abilities[Slot];
	if (IsCountdown(Slot)) { return EAbilityValidation::AV_NotReady; }

	// Cost Validation
	if (Ability.HealthCost.GetAmount(Health) >= Health.Value) { return EAbilityValidation::AV_Health; }
	if (Ability.ManaCost.GetAmount(Mana) > Mana.Value) { return EAbilityValidation::AV_Mana; }
	if (Ability.EnergyCost.GetAmount(Energy)> Energy.Value) { return EAbilityValidation::AV_Energy; }

	// Directional or Area on Effect Validation
	if (Ability.Area == EAbilityArea::AA_Directional || Ability.Area == EAbilityArea::AA_AreaOnEffect)
	{
		return EAbilityValidation::AV_Allowed;
	}

	// State Validation
	if (!Target || Target->State == ECharacterState::CS_Death) { return EAbilityValidation::AV_Target; }
	if (State == ECharacterState::CS_Stun || State == ECharacterState::CS_Death)
	{
		return EAbilityValidation::AV_Incapacitated;
	}

	// Target Abilities Validation.
	float Distance = FVector::Distance(GetActorLocation(), Target->GetActorLocation());
	if (Distance < Ability.MinDistance) { return EAbilityValidation::AV_TooClose; }
	if (Distance > Ability.MaxDistance) { return EAbilityValidation::AV_TooFar; }

	// Cast Validation
	if (Ability.UseType == EAbilityUseType::AUT_Castable)
	{
		if (GetTargetAngle() > Ability.CastMaxAngle) { return EAbilityValidation::AV_OutOfSight; }
		if (!MightCast(Slot)) { return EAbilityValidation::AV_InMovement; }
		
		return EAbilityValidation::AV_Allowed;
	}

	return EAbilityValidation::AV_Allowed;
}

bool AAbilityCharacter::MightCast(int32 Slot)
{
	return GetVelocity().Size() <= 30.0f;
}

EAbilityValidation AAbilityCharacter::StartAbility(int32 Slot)
{
	if (!Abilities.Contains(Slot) || IsCountdown(Slot)) { return EAbilityValidation::AV_InvalidAbility; }

	if (IsCasting()) { return EAbilityValidation::AV_Allowed; }

	EAbilityValidation Validation = ValidateAbility(Slot);
	if (Validation != EAbilityValidation::AV_Allowed)
	{
		ClientNotification(GetValidationErrorText(Validation));
		return Validation;
	}

	AbilityTarget = Target;

	if (Abilities[Slot].UseType == EAbilityUseType::AUT_Instant)
	{
		CommitAbility(Slot);
		return EAbilityValidation::AV_Allowed;
	}

	StartCast(Slot);
	return EAbilityValidation::AV_Allowed;
}

void AAbilityCharacter::StopAbility(int32 Slot)
{
	if (!Abilities.Contains(Slot)) { return; }

	if (Abilities[Slot].UseType != EAbilityUseType::AUT_Castable) { return; }

	StopCast();
}

void AAbilityCharacter::CommitAbility(int32 Slot)
{
	UWorld* World = GetWorld();
	if (!World || !Abilities.Contains(Slot)) { return; }

	Abilities[Slot].LastUseTime = UGameplayStatics::GetTimeSeconds(World);

	FAbility Ability = Abilities[Slot];

	if (Ability.UseType == EAbilityUseType::AUT_Instant)
	{
		StartGlobalCountdown();
	}

	StopAbility(Slot);

	if (!HasAuthority()) { return; }

	// Start with the applier cost.
	Health.Value -= Ability.HealthCost.GetAmount(Health);
	Mana.Value -= Ability.ManaCost.GetAmount(Mana);
	Energy.Value -= Ability.EnergyCost.GetAmount(Energy);

	/*if (Ability.UseType == EAbilityUseType::AUT_Castable)
	{
		State = ECharacterState::CS_CommitRange;
	}
	else
	{
		State = ECharacterState::CS_CommitMelee;
	}*/

	// add an delay to animation
	FTimerHandle TimerHandle;
	World->GetTimerManager().SetTimer(TimerHandle, this, &AAbilityCharacter::ServerSetNormalState, 0.1f, false);

	// Is a Target Ability.
	if (Ability.Area == EAbilityArea::AA_Target)
	{
		if (!AbilityTarget) { return; }

		// Must generate a projectile.
		if (Ability.CommitType == EAbilityCommitType::ACT_Projectile)
		{
			MulticastSpawnProjectile(AbilityTarget, Ability.ProjectileType);
			return AddDelayedAbilityModifiers(AbilityTarget, Ability);
		}

		return AddAbilityModifiers(AbilityTarget, Ability);
	}

	// Is a Directional Ability.
	if (Ability.Area == EAbilityArea::AA_Directional)
	{
		TArray<FHitResult> OutResults;
		if (!GetDirectionalHitTest(OutResults, 50.0f, 100.0f)) { return; }

		for (auto OutResult : OutResults)
		{
			ACharacterBase* Character = Cast<ACharacterBase>(OutResult.GetActor());

			if (!Character || Character->State == ECharacterState::CS_Death) { continue; }

			AddAbilityModifiers(Character, Ability);
		}
	}

	// Is an Area On Effect Ability.
	if (Ability.Area == EAbilityArea::AA_AreaOnEffect)
	{
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(World, ACharacterBase::StaticClass(), FoundActors);

		for (auto Actor : FoundActors)
		{
			ACharacterBase* Character = Cast<ACharacterBase>(Actor);

			if (!Character || Character->State == ECharacterState::CS_Death) { continue; }

			FVector CharacterLocation = Character->GetActorLocation();
			if (FVector::Distance(CharacterLocation, GetActorLocation()) >= Ability.MaxDistance) { continue; }
			if (Character->GetUniqueID() == GetUniqueID()) { continue; }

			AddAbilityModifiers(Character, Ability);
		}
	}
}

void AAbilityCharacter::AddAbilityModifiers(ACharacterBase* TargetCharacter, FAbility Ability)
{
	if (!TargetCharacter || TargetCharacter->State == ECharacterState::CS_Death) { return; }

	TargetCharacter->AddBuffModifiers(Ability.BuffModifiers);
	TargetCharacter->AddOvertimeModifiers(Ability.OvertimeModifiers);
	TargetCharacter->AddInstantModifiers(Ability.InstantModifiers);
}

void AAbilityCharacter::AddDelayedAbilityModifiers(ACharacterBase* TargetCharacter, FAbility Ability)
{
	if (
		State == ECharacterState::CS_Death 
		|| !TargetCharacter 
		|| TargetCharacter->State == ECharacterState::CS_Death
		) 
	{
		return;
	}

	float Distance = FVector::Distance(GetActorLocation(), TargetCharacter->GetActorLocation());
	float Delay = Distance / 1200.0f;

	UWorld* World = GetWorld();
	if (!World) { return; }

	FTimerHandle TimerHandle;
	FTimerDelegate TimerDel;
	TimerDel.BindUFunction(this, FName("AddAbilityModifiers"), TargetCharacter, Ability);

	World->GetTimerManager().SetTimer(TimerHandle, TimerDel, Delay, false);
}

bool AAbilityCharacter::GetDirectionalHitTest(TArray<FHitResult> &OutResults, float Radius, float Range)
{
	UWorld* World = GetWorld();

	if (!World) { return false; }

	FCollisionShape MySphere = FCollisionShape::MakeSphere(Radius);

	FVector Start = GetActorLocation();
	FVector End = (GetActorForwardVector() * Range) + Start;

	FCollisionQueryParams TraceParams(FName(TEXT("VFS Trace")), true, this);
	TraceParams.bTraceComplex = true;
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = false;

	//Ignore Actors
	TraceParams.AddIgnoredActor(this);

	return GetWorld()->SweepMultiByChannel(OutResults, Start, End, FQuat::Identity, ECollisionChannel::ECC_Pawn, MySphere, TraceParams);
}

FString AAbilityCharacter::GetValidationErrorText(EAbilityValidation Validation)
{
	switch (Validation)
	{
	case AV_InvalidAbility:
		return "Invalid Ability";
	case AV_NotReady:
		return "That Ability Is Not Ready Yet";
	case AV_Health:
		return "Not Enought Health";
	case AV_Mana:
		return "Not Enought Mana";
	case AV_Energy:
		return "Not Enought Energy";
	case AV_Target:
		return "You Need To Target Something First";
	case AV_TooFar:
		return "Is Too Far Away";
	case AV_TooClose:
		return "Is Too Close";
	case AV_Incapacitated:
		return "You are Incapacitated";
	case AV_InMovement:
		return "You Cannot Cast in Movement";
	case AV_OutOfSight:
		return "The Target is Out of Sight";
	default:
		return "";
	}
}

void AAbilityCharacter::ServerSetNormalState_Implementation()
{
	/*if (State != ECharacterState::CS_CommitMelee && State != ECharacterState::CS_CommitRange) { return; }

	State = ECharacterState::CS_Idle;
	ClientSetNormalState();*/
}

void AAbilityCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AAbilityCharacter::MulticastSpawnProjectile_Implementation(ACharacterBase* TargetCharacter, EProjectileType ProjectileType)
{
	if (HasAuthority()) { return; }
	SpawnProjectileDelegate.Broadcast(TargetCharacter, ProjectileType);
}
