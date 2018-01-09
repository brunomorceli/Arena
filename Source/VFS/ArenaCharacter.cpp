// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "ArenaCharacter.h"
#include "GlobalLibrary.h"

AArenaCharacter::AArenaCharacter()
{
	AbilityTarget = NULL;

	CastTimeRemaining = 0.0f;

	GlobalCountdownTime = 1.0f;
	GlobalCountdownTimeRemaining = 0.0f;

	OutOfSightMaxTime = 0.2f;
	OutOfSightTime = 0.0f;

	AuraModifiers = TArray<FAuraModifier>();
	AbsorbingModifiers = TArray<FAbsorbingModifier>();
}

void AArenaCharacter::BeginPlay()
{
	Super::BeginPlay();

	SetupAbilities();

	if (!HasAuthority()) { return; }
	FTimerHandle MyTimer;
	GetWorldTimerManager().SetTimer(MyTimer, this, &AArenaCharacter::ClientSetPlayerProfile, 0.5, false);
}

void AArenaCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ValidateCast(DeltaTime);
	UpdateTimers(DeltaTime);

	if (CastTimeRemaining == 0.0f && (AnimationState == CAS_Cast || AnimationState == CAS_Channel))
	{
		StopCast(true);
	}
}

void AArenaCharacter::UpdateState()
{
	Super::UpdateState();

	if (!HasAuthority()) { return; }

	if (Health.Value == 0 && State != CS_Death)
	{
		AuraModifiers.Empty();
		BuffModifiers.Empty();
		OvertimeModifiers.Empty();
		AbsorbingModifiers.Empty();

		SetDeadState();

		return;
	}

	TEnumAsByte<ECharacterState> NextState = State;

	TArray<int32> Keys;
	BuffModifiers.GenerateKeyArray(Keys);
	for (auto Key : Keys)
	{
		if (BuffModifiers[Key].State > State) { State = BuffModifiers[Key].State; }
	}
}

void AArenaCharacter::ValidateCast(float DeltaTime)
{
	if (IsCasting() && ValidateStartAbility(CastAbility->Slot) == ABV_Allowed)
	{
		OutOfSightTime = 0.0f;
		return;
	}

	if (ABV_OutOfSight)
	{
		OutOfSightTime += DeltaTime;
		if (OutOfSightTime > OutOfSightMaxTime) { StopCast(true); }
	}
}

void AArenaCharacter::UpdateTimers(float DeltaTime)
{
	GlobalCountdownTimeRemaining =  FMath::Clamp(GlobalCountdownTimeRemaining - DeltaTime, 0.0f, GlobalCountdownTime);

	UpdateBuffModifiers(DeltaTime);

	UpdateOvertimeModifiers(DeltaTime);

	if (!IsCasting()) { return; }
	
	CastTimeRemaining = FMath::Clamp(CastTimeRemaining - DeltaTime, 0.0f, CastAbility->CastTime);
	EAbilityStartType Type = CastAbility->AbilityType;

	if (Type == ABST_Castable && CastTimeRemaining == 0.0f)
	{
		return CommitAbility(CastAbility->Slot);
	}

	if (Type == ABST_Channeling && CastAbility->UpdateChanneling(DeltaTime))
	{
		return CommitAbilityModifiers(CastAbility, true);
	}
}

void AArenaCharacter::UpdateBuffModifiers(float DeltaTime)
{
	if (!HasAuthority()) { return; }

	bool bMustBeUpdated = false;

	TArray<int32> Keys;
	BuffModifiers.GenerateKeyArray(Keys);
	for (auto Key : Keys)
	{
		BuffModifiers[Key].TimeRemaining -= DeltaTime;
		if (BuffModifiers[Key].TimeRemaining <= 0.0f)
		{
			FModifierInfo ModifierInfo = GetModifierInfo(BuffModifiers[Key]);
			ModifierInfo.Event = EABE_Expire;

			if (BuffModifiers[Key].OnExpireHandler) { BuffModifiers[Key].OnExpireHandler(ModifierInfo); }
			if (BuffModifiers[Key].OnRemoveHandler) { BuffModifiers[Key].OnRemoveHandler(ModifierInfo); }

			BuffModifiers.Remove(Key);
			bMustBeUpdated = true;
		}
	}

	if (bMustBeUpdated) { RefreshBuffModifiersStatus(); }
}

void AArenaCharacter::RefreshBuffModifiersStatus()
{
	if (!HasAuthority()) { return; }

	Health.ResetLimits();
	Mana.ResetLimits();
	Energy.ResetLimits();
	Critical.Reset();
	Speed.Reset();
	MagicPower.Reset();
	MagicDefense.Reset();
	PhysicalPower.Reset();
	PhysicalDefense.Reset();

	float HealthBuff = 0.0f;
	float ManaBuff = 0.0f;
	float EnergyBuff = 0.0f;
	float SpeedBuff = 0.0f;
	float CriticalBuff = 0.0f;
	float MagicPowerBuff = 0.0f;
	float MagicDefenseBuff = 0.0f;
	float PhysicalPowerBuff = 0.0f;
	float PhysicalDefenseBuff = 0.0f;

	float HealthDebuff = 0.0f;
	float ManaDebuff = 0.0f;
	float EnergyDebuff = 0.0f;
	float SpeedDebuff = 0.0f;
	float CriticalDebuff = 0.0f;
	float MagicPowerDebuff = 0.0f;
	float MagicDefenseDebuff = 0.0f;
	float PhysicalPowerDebuff = 0.0f;
	float PhysicalDefenseDebuff = 0.0f;

	TArray<int32> Keys;
	BuffModifiers.GenerateKeyArray(Keys);
	for (auto Key : Keys)
	{
		FBuffModifier Buff = BuffModifiers[Key];

		if (Buff.bIsHarmful)
		{
			if (Buff.HealthAmount > HealthDebuff) { HealthDebuff = Buff.HealthAmount; }
			if (Buff.ManaAmount > ManaDebuff) { ManaDebuff = Buff.ManaAmount; }
			if (Buff.EnergyAmount > EnergyDebuff) { EnergyDebuff = Buff.EnergyAmount; }
			if (Buff.CriticalAmount > CriticalDebuff) { HealthDebuff = Buff.CriticalAmount; }
			if (Buff.SpeedAmount > SpeedDebuff) { SpeedDebuff = Buff.SpeedAmount; }
			if (Buff.MagicPowerAmount > MagicPowerDebuff) { MagicPowerDebuff = Buff.MagicPowerAmount; }
			if (Buff.MagicDefenseAmount > MagicDefenseDebuff) { MagicDefenseDebuff = Buff.MagicDefenseAmount; }
			if (Buff.PhysicalPowerAmount > PhysicalPowerDebuff) { PhysicalPowerDebuff = Buff.PhysicalPowerAmount; }
			if (Buff.PhysicalDefenseAmount > PhysicalDefenseDebuff) { PhysicalDefenseDebuff = Buff.PhysicalDefenseAmount; }
		}
		else
		{
			if (Buff.HealthAmount > HealthBuff) { HealthBuff = Buff.HealthAmount; }
			if (Buff.ManaAmount > ManaBuff) { ManaBuff = Buff.ManaAmount; }
			if (Buff.EnergyAmount > EnergyBuff) { EnergyBuff = Buff.EnergyAmount; }
			if (Buff.CriticalAmount > CriticalBuff) { HealthBuff = Buff.CriticalAmount; }
			if (Buff.SpeedAmount > SpeedBuff) { SpeedBuff = Buff.SpeedAmount; }
			if (Buff.MagicPowerAmount > MagicPowerBuff) { MagicPowerBuff = Buff.MagicPowerAmount; }
			if (Buff.MagicDefenseAmount > MagicDefenseBuff) { MagicDefenseBuff = Buff.MagicDefenseAmount; }
			if (Buff.PhysicalPowerAmount > PhysicalPowerBuff) { PhysicalPowerBuff = Buff.PhysicalPowerAmount; }
			if (Buff.PhysicalDefenseAmount > PhysicalDefenseBuff) { PhysicalDefenseBuff = Buff.PhysicalDefenseAmount; }
		}
	}

	Health.Max += HealthBuff;
	Health.Max -= HealthDebuff;
	Health.Clamp(0.0f, false);

	Mana.Max += ManaBuff;
	Mana.Max -= ManaDebuff;
	Mana.Clamp(0.0f, false);

	Energy.Max += EnergyBuff;
	Energy.Max -= EnergyDebuff;
	Energy.Clamp(0.0f, false);

	Critical.Value += CriticalBuff;
	Critical.Value -= CriticalDebuff;

	Critical.Clamp(CriticalBuff - CriticalDebuff, false);

	Speed.Clamp(SpeedBuff - SpeedDebuff, false);
	
	MagicPower.Clamp(MagicPowerBuff - MagicPowerDebuff, false);
	MagicDefense.Clamp(MagicDefenseBuff - MagicDefenseDebuff, false);
	PhysicalPower.Clamp(PhysicalPowerBuff - PhysicalPowerDebuff, false);
	PhysicalDefense.Clamp(PhysicalDefenseBuff - PhysicalDefenseDebuff, false);

	MulticastSetMaxWalkSpeed(Speed.Value);
}

void AArenaCharacter::UpdateOvertimeModifiers(float DeltaTime)
{
	if (!HasAuthority()) { return; }

	TArray<int32> OvertimeKeys;
	OvertimeModifiers.GenerateKeyArray(OvertimeKeys);
	for (auto Key : OvertimeKeys)
	{
		OvertimeModifiers[Key].ElapsedTime += DeltaTime;
		OvertimeModifiers[Key].TimeRemaining -= DeltaTime;

		FModifierInfo ModifierInfo = GetModifierInfo(OvertimeModifiers[Key]);
		ModifierInfo.Event = EABE_Expire;

		if (OvertimeModifiers[Key].ElapsedTime >= OvertimeModifiers[Key].TickTime)
		{
			TickOvertimeModifier(OvertimeModifiers[Key]);
			OvertimeModifiers[Key].ElapsedTime = FMath::Fmod(OvertimeModifiers[Key].ElapsedTime, OvertimeModifiers[Key].TickTime);

			if (OvertimeModifiers[Key].OnTickHandler) { OvertimeModifiers[Key].OnTickHandler(ModifierInfo); }
		}

		if (OvertimeModifiers[Key].TimeRemaining <= 0.0f)
		{
			if (OvertimeModifiers[Key].OnExpireHandler) { OvertimeModifiers[Key].OnExpireHandler(ModifierInfo); }
			if (OvertimeModifiers[Key].OnRemoveHandler) { OvertimeModifiers[Key].OnRemoveHandler(ModifierInfo); }

			OvertimeModifiers.Remove(Key);

			MulticastNotifyModifierInfo(ModifierInfo);
		}
	}
}

void AArenaCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Ability01", IE_Pressed, this, &AArenaCharacter::InputStartAbility<1>);
	PlayerInputComponent->BindAction("Ability02", IE_Pressed, this, &AArenaCharacter::InputStartAbility<2>);
	PlayerInputComponent->BindAction("Ability03", IE_Pressed, this, &AArenaCharacter::InputStartAbility<3>);
	PlayerInputComponent->BindAction("Ability04", IE_Pressed, this, &AArenaCharacter::InputStartAbility<4>);
	PlayerInputComponent->BindAction("Ability05", IE_Pressed, this, &AArenaCharacter::InputStartAbility<5>);
	PlayerInputComponent->BindAction("Ability06", IE_Pressed, this, &AArenaCharacter::InputStartAbility<6>);
	PlayerInputComponent->BindAction("Ability07", IE_Pressed, this, &AArenaCharacter::InputStartAbility<7>);
	PlayerInputComponent->BindAction("Ability08", IE_Pressed, this, &AArenaCharacter::InputStartAbility<8>);
}

// ==============================================================================================================================
// GLOBAL COUNTDOWN
// ==============================================================================================================================

void AArenaCharacter::StartGlobalCountdown()
{
	GlobalCountdownTimeRemaining = GlobalCountdownTime;
}

void AArenaCharacter::StopGlobalCountdown()
{
	GlobalCountdownTimeRemaining = 0.0f;
}

bool AArenaCharacter::IsGlobalCountdown()
{
	return GlobalCountdownTimeRemaining > 0.0f;
}

float AArenaCharacter::GetGlobalCountdownRemaining()
{
	return GlobalCountdownTimeRemaining;
}

float AArenaCharacter::GetGlobalCountdownPercent()
{
	float Elapsed = GlobalCountdownTime - GlobalCountdownTimeRemaining;
	return (Elapsed * 100.0f) / GlobalCountdownTime;
}

// ==============================================================================================================================
// ABILITY COUNTDOWN
// ==============================================================================================================================

bool AArenaCharacter::IsCountdown(int32 Slot) {
	if (!Abilities.Contains(Slot)) { return false; }

	AAbilityBase* Ability = Abilities[Slot];
	if (Ability->AbilityType == EAbilityStartType::ABST_Instant && IsGlobalCountdown()) { return true; }

	return Ability->IsCountdown();
}

float AArenaCharacter::GetCountdownRemaining(int32 Slot)
{
	if (!Abilities.Contains(Slot)) { return 0.0f; }

	AAbilityBase* Ability = Abilities[Slot];

	if (Ability->AbilityType == EAbilityStartType::ABST_Instant)
	{
		float GlobalCountdownRemaining = GetGlobalCountdownRemaining();
		if (GlobalCountdownRemaining) { return GlobalCountdownRemaining; }
	}

	return Ability->GetCountdownRemaining();
}

float AArenaCharacter::GetCountdownPercent(int32 Slot)
{
	if (!Abilities.Contains(Slot)) { return 0.0f; }
	AAbilityBase* Ability = Abilities[Slot];

	if (Ability->AbilityType == EAbilityStartType::ABST_Instant)
	{
		float GCDTimeRemaining = GetGlobalCountdownRemaining();
		if (GCDTimeRemaining > 0.0f)
		{
			return ((GCDTimeRemaining * 100.0f) / GlobalCountdownTime);
		}
	}

	float Remaining = GetCountdownRemaining(Slot);
	if (Remaining <= 0.0f) { return 0.0f; };

	return ((Remaining * 100.0f) / Ability->CountdownTime);
}

// ==============================================================================================================================
// ABILITY
// ==============================================================================================================================

AAbilityBase* AArenaCharacter::AddAbility(TSubclassOf<AAbilityBase> AbilityBase, int32 Slot)
{
	UWorld* World = GetWorld();
	if (!World) { return NULL; }

	AAbilityBase* Ability = World->SpawnActor<AAbilityBase>(AbilityBase);

	if (!Ability) { return NULL; }

	Ability->Slot = Slot;
	Ability->CharacterOwner = this;
	Abilities.Add(Slot, Ability);

	return Ability;
}

AAbilityBase* AArenaCharacter::GetAbility(int32 Slot)
{
	return Abilities.Contains(Slot) ? Abilities[Slot] : NULL;
}

EAbilityValidation AArenaCharacter::StartAbility(int32 Slot)
{
	if (!Abilities.Contains(Slot)) { return ABV_InvalidAbility; }
	if (IsCasting()) { return ABV_InProgress; }

	AAbilityBase* Ability = Abilities[Slot];
	if (Ability->IsCountdown()) { return ABV_NotReady; }

	EAbilityValidation Validation = ValidateStartAbility(Slot);
	if (Validation != ABV_Allowed) { return Validation; }

	// Validates the cost.
	Validation = ValidateCost(Ability);
	if (Validation != ABV_Allowed) { return Validation; }

	if (Ability->AbilityType == ABST_Castable || Ability->AbilityType == ABST_Channeling)
	{
		StartCast(Slot);

		if (Ability->AbilityType == ABST_Channeling)
		{
			ApplyCosts(Ability);
			Ability->StartCountdown();
			if (Ability->CommitFX) { ServerSetAnimState(CAS_Channel, Ability->CommitFX); }
		}

		return ABV_Allowed;
	}

	CommitAbility(Slot);
	return ABV_Allowed;
}

void  AArenaCharacter::StopAbility(int32 Slot)
{
	if (!Abilities.Contains(Slot)) { return; }
	StopCast(true);
	State = CS_Idle;
}

void AArenaCharacter::StartCast(int32 Slot)
{
	if (!Abilities.Contains(Slot)) { return; }

	StopCast(true);

	CastAbility = Abilities[Slot];
	CastTimeRemaining = CastAbility->CastTime;
	ServerSetAnimState(CastAbility->AbilityType == ABST_Castable ? CAS_Cast : CAS_Channel, CastAbility->CastFX);
}

void AArenaCharacter::StopCast(bool ChangeAnimation)
{
	CastTimeRemaining = 0.0f;
	CastAbility = NULL;

	if (ChangeAnimation) { ServerSetAnimState(CAS_None, NULL); }
}

void AArenaCharacter::CommitAbility(int32 Slot)
{
	if (!Abilities.Contains(Slot)) { return; }

	AAbilityBase* Ability = Abilities[Slot];

	// Countdown.
	StopCast(false);
	Ability->StartCountdown();
	if (Ability->AbilityType == ABST_Instant) { StartGlobalCountdown(); }

	if (!HasAuthority()) { return; }

	TArray<int32> Keys;
	BuffModifiers.GenerateKeyArray(Keys);
	for (auto Key : Keys)
	{
		if (!BuffModifiers[Key].OnUseAbility) { continue; }
		FModifierInfo ModifierInfo = GetModifierInfo(BuffModifiers[Key]);
		int32 AbilityId = BuffModifiers[Key].AbilityOwner->GetUniqueID();

		BuffModifiers[Key].OnUseAbility(ModifierInfo);
	}

	CommitAbilityModifiers(Ability, false);

	TEnumAsByte<ECharacterAnimationState> AnimationState;
	switch (Slot)
	{
		case 1:
			AnimationState = CAS_Ability1;
			break;
		case 2:
			AnimationState = CAS_Ability2;
			break;
		case 3:
			AnimationState = CAS_Ability3;
			break;
		case 4:
			AnimationState = CAS_Ability4;
			break;
		case 5:
			AnimationState = CAS_Ability5;
			break;
		case 6:
			AnimationState = CAS_Ability6;
			break;
		case 7:
			AnimationState = CAS_Ability7;
			break;
		case 8:
			AnimationState = CAS_Ability8;
			break;
		default:
			AnimationState = CAS_None;
			break;
	}

	ServerSetAnimState(AnimationState, Ability->CommitFX);
}

void AArenaCharacter::CommitAbilityModifiers(AAbilityBase* Ability, bool withoutCost)
{
	UWorld* World = GetWorld();
	if (!World || !Ability || !HasAuthority()) { return; }

	// Is a Target Ability.
	if (Ability->AreaType == ABA_Target)
	{
		AArenaCharacter* ArenaCharacter = Cast<AArenaCharacter>(AbilityTarget);

		bool LineOfSight = GetController()->LineOfSightTo(ArenaCharacter, FVector::ZeroVector);
		if (!LineOfSight || !ArenaCharacter->IsValidModifierState()) { return; }

		if (!withoutCost) { ApplyCosts(Ability); }

		// Must generate a projectile.
		if (Ability->CommitType == ABC_Projectile)
		{
			ArenaCharacter->ApplyDelayedModifiers(Ability);
			return MulticastSpawnProjectile(this, ArenaCharacter, Ability);
		}

		return ArenaCharacter->ApplyModifiers(Ability);
	}

	// Is a Directional Ability.
	if (Ability->AreaType == ABA_Directional)
	{
		if (!withoutCost) { ApplyCosts(Ability); }

		TArray<ACharacterBase*> Characters;
		if (!DirectionalHitTest(Characters, Ability->DirectionalRadius, Ability->DirectionalRange)) { return; }

		for (auto Character : Characters)
		{
			AArenaCharacter* ArenaCharacter = Cast<AArenaCharacter>(Character);

			bool LoS = GetController()->LineOfSightTo(ArenaCharacter, FVector::ZeroVector);
			if (!LoS || !ArenaCharacter->IsValidModifierState()) { continue; }

			// Target validation.
			EAbilityValidation TargetValidation = ValidateTarget(Ability, this, ArenaCharacter);
			if (TargetValidation != ABV_Allowed) { continue; }

			// Must generate a projectile.
			if (Ability->CommitType == ABC_Projectile)
			{
				ArenaCharacter->ApplyDelayedModifiers(Ability);
				return MulticastSpawnProjectile(this, ArenaCharacter, Ability);
			}

			ArenaCharacter->ApplyModifiers(Ability);
		}
	}

	// Is an Area On Effect Ability.
	if (Ability->AreaType == EAbilityArea::ABA_AreaOnEffect)
	{
		if (!withoutCost) { ApplyCosts(Ability); }

		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(World, ACharacterBase::StaticClass(), FoundActors);

		for (auto Actor : FoundActors)
		{
			AArenaCharacter* ArenaCharacter = Cast<AArenaCharacter>(Actor);
			if (!ArenaCharacter) { continue; }

			bool LoS = GetController()->LineOfSightTo(ArenaCharacter, FVector::ZeroVector);
			if (!LoS || !ArenaCharacter->IsValidModifierState()) { continue; }

			// Target validation.
			EAbilityValidation TargetValidation = ValidateTarget(Ability, this, ArenaCharacter);
			if (TargetValidation != ABV_Allowed) { continue; }

			FVector CharacterLocation = ArenaCharacter->GetActorLocation();
			if (FVector::Distance(CharacterLocation, GetActorLocation()) >= Ability->MaxDistance) { continue; }

			// Must generate a projectile.
			if (Ability->CommitType == ABC_Projectile)
			{
				ArenaCharacter->ApplyDelayedModifiers(Ability);
				return MulticastSpawnProjectile(this, ArenaCharacter, Ability);
			}

			ArenaCharacter->ApplyModifiers(Ability);
		}
	}
}

bool AArenaCharacter::IsValidCommitState()
{
	return State != CS_Death && State != CS_Spectate && State != CS_Stun;
}

bool AArenaCharacter::IsValidModifierState()
{
	return State != CS_Death && State != CS_Spectate;
}

EAbilityValidation AArenaCharacter::ValidateStartAbility(int32 Slot)
{
	if (!IsValidCommitState()) { return ABV_Incapacitated; }
	if (!Abilities.Contains(Slot)) { return ABV_InvalidAbility; }

	AAbilityBase* Ability = Abilities[Slot];

	// Target Validation
	if (Ability->AreaType == EAbilityArea::ABA_Target)
	{
		EAbilityValidation Validation = ValidateTarget(Ability, this, Target);
		if (Validation != ABV_Allowed)
		{
			if (!Ability->bAllowSelf) { return Validation; }
			AbilityTarget = this;
		}
		else
		{
			AbilityTarget = Target;
		}
	}

	if (Ability->AbilityType == ABST_Castable) { return ValidateStartCast(Ability); }
	if (Ability->AbilityType == ABST_Channeling) { return ValidateStartChanneling(Ability); }

	return ValidateStartInstant(Ability);
}

EAbilityValidation AArenaCharacter::ValidateStartCast(AAbilityBase* Ability)
{
	// Movement Validation.
	if (!Ability->bCastInMovement && GetVelocity().Size() > 30.0f) { return ABV_Moving; }

	// Directional or Area on Effect Validation.
	if (Ability->AreaType == ABA_Directional || Ability->AreaType == ABA_AreaOnEffect) { return ABV_Allowed; }

	// Target Abilities Validation.
	float Distance = FVector::Distance(GetActorLocation(), AbilityTarget->GetActorLocation());
	if (Distance < Ability->MinDistance) { return ABV_TooClose; }
	if (Distance > Ability->MaxDistance) { return ABV_TooFar; }

	if (!GetController()->LineOfSightTo(AbilityTarget, FVector::ZeroVector)) { return EAbilityValidation::ABV_OutOfSight; }
	if (GetAngle(this, AbilityTarget) > Ability->MaxAngle) { return EAbilityValidation::ABV_OutOfSight; }

	return ABV_Allowed;
}

EAbilityValidation AArenaCharacter::ValidateStartChanneling(AAbilityBase* Ability)
{
	// Movement Validation.
	if (!Ability->bCastInMovement && GetVelocity().Size() > 30.0f) { return ABV_Moving; }

	// Directional or Area on Effect Validation.
	if (Ability->AreaType == ABA_Directional || Ability->AreaType == ABA_AreaOnEffect) { return ABV_Allowed; }

	// Target Abilities Validation.
	float Distance = FVector::Distance(GetActorLocation(), Target->GetActorLocation());
	if (Distance < Ability->MinDistance) { return ABV_TooClose; }
	if (Distance > Ability->MaxDistance) { return ABV_TooFar; }

	if (!GetController()->LineOfSightTo(AbilityTarget, FVector::ZeroVector)) { return EAbilityValidation::ABV_OutOfSight; }
	if (GetAngle(this, AbilityTarget) > Ability->MaxAngle) { return EAbilityValidation::ABV_OutOfSight; }

	return ABV_Allowed;
}

EAbilityValidation AArenaCharacter::ValidateStartInstant(AAbilityBase* Ability)
{
	if (IsGlobalCountdown()) { return ABV_NotReady; }

	// Directional or Area on Effect Validation.
	if (Ability->AreaType == ABA_Directional || Ability->AreaType == ABA_AreaOnEffect) { return ABV_Allowed; }

	// Target Abilities Validation.
	float Distance = FVector::Distance(GetActorLocation(), AbilityTarget->GetActorLocation());
	if (Distance < Ability->MinDistance) { return ABV_TooClose; }
	if (Distance > Ability->MaxDistance) { return ABV_TooFar; }

	if (!GetController()->LineOfSightTo(AbilityTarget, FVector::ZeroVector)) { return EAbilityValidation::ABV_OutOfSight; }
	if (GetAngle(this, AbilityTarget) > Ability->MaxAngle) { return EAbilityValidation::ABV_OutOfSight; }

	return ABV_Allowed;
}

EAbilityValidation AArenaCharacter::ValidateCost(AAbilityBase* Ability)
{
	float bIsPercent = Ability->bIsPercent;

	if (!Health.GotAmount(Ability->HealthCost, bIsPercent)) { return ABV_Health; }
	if (!Mana.GotAmount(Ability->ManaCost, bIsPercent)) { return ABV_Mana; }
	if (!Energy.GotAmount(Ability->EnergyCost, bIsPercent)) { return ABV_Energy; }

	return ABV_Allowed;
}

EAbilityValidation AArenaCharacter::ValidateTarget(AAbilityBase* Ability, ACharacterBase* First, ACharacterBase* Second)
{
	if (!First || !Second) { return ABV_MissTarget; }

	bool IsEnemy = IsEnemyCharacter(First, Second);
	bool IsSame = First->GetUniqueID() ==  Second->GetUniqueID();

	if (IsEnemy) { return Ability->bAllowEnemy ? ABV_Allowed : ABV_InvalidTarget; }
	if (!IsSame) { return Ability->bAllowTeam ? ABV_Allowed : ABV_InvalidTarget; }
	if (IsSame) { return ABV_InvalidTarget; }

	return Ability->bAllowSelf ? ABV_Allowed : ABV_InvalidTarget;
}

bool AArenaCharacter::IsEnemyCharacter(ACharacterBase* First, ACharacterBase* Second)
{
	if (!First || !Second || First->GetUniqueID() == Second->GetUniqueID()) { return false; }

	AArenaPlayerState* FirstPlayerState = Cast<AArenaPlayerState>(First->PlayerState);
	AArenaPlayerState* SecondPlayerState = Cast<AArenaPlayerState>(Second->PlayerState);

	if (!FirstPlayerState || !SecondPlayerState) { return false; }

	return FirstPlayerState->Team != SecondPlayerState->Team;
}

bool AArenaCharacter::IsCasting()
{
	return CastTimeRemaining > 0.0f;
}

float AArenaCharacter::GetCastTimeRemaining()
{
	return CastTimeRemaining;
}

float AArenaCharacter::GetCastPercent()
{
	if (!CastAbility) { return 0.0f; }
	float Elapsed = CastAbility->CastTime - CastTimeRemaining;
	return (Elapsed * 100.0f) / CastAbility->CastTime;
}

// ==============================================================================================================================
// GENERAL
// ==============================================================================================================================

FString AArenaCharacter::GetErrorString(EAbilityValidation Validation)
{
	switch (Validation)
	{
	case ABV_InvalidAbility:
		return "Invalid Ability";
	case ABV_NotReady:
		return "Not Ready Yet";
	case ABV_Health:
		return "Not Enough Health ";
	case ABV_Mana:
		return "Not Enough Mana";
	case ABV_Energy:
		return "Not Enough Energy";
	case ABV_MissTarget:
		return "You Have No Target";
	case ABV_InvalidTarget:
		return "Invalid Target";
	case ABV_TooFar:
		return "Is Too Far Away";
	case ABV_TooClose:
		return "Ist Too Close";
	case ABV_Moving:
		return "You Can't Move";
	case ABV_OutOfSight:
		return "Is Out Of Sight";
	case ABV_Incapacitated:
		return "You Are Incapacitated";
	case ABV_Unknown:
		return "Invalid";
	case ABV_InProgress:
		return "Another Ability in Progress";
	default:
		return "";
	}
}

void AArenaCharacter::ApplyModifiers(AAbilityBase* Ability)
{
	if (!HasAuthority()) { return; }

	AArenaCharacter* Causer = Cast<AArenaCharacter>(Ability->CharacterOwner);
	if (!Causer) { return; }

	bool IsEnemy = IsEnemyCharacter(this, Causer);
	bool IsSame = GetUniqueID() == Causer->GetUniqueID();

	for (auto Modifier : Ability->AuraModifiers)
	{
		if (IsEnemy)
		{
			if (Modifier.bAllowEnemy){ ApplyAuraModifier(Modifier); }
			if (Modifier.bAllowSelf) { Causer->ApplyAuraModifier(Modifier); }

			continue;
		}

		if (!IsSame && Modifier.bAllowTeam)
		{
			ApplyAuraModifier(Modifier);
			continue;
		}

		if (Modifier.bAllowSelf) { Causer->ApplyAuraModifier(Modifier); }
	}

	for (auto Modifier : Ability->BuffModifiers)
	{
		if (IsEnemy)
		{
			if (Modifier.bAllowEnemy) { ApplyBuffModifier(Modifier); }
			if (Modifier.bAllowSelf) { Causer->ApplyBuffModifier(Modifier); }

			continue;
		}

		if (!IsSame && Modifier.bAllowTeam)
		{
			ApplyBuffModifier(Modifier);
			continue;
		}

		if (Modifier.bAllowSelf) { Causer->ApplyBuffModifier(Modifier); }
	}

	for (auto Modifier : Ability->AbsorbingModifiers)
	{
		if (IsEnemy)
		{
			if (Modifier.bAllowEnemy) { ApplyAbsorbingModifier(Modifier); }
			if (Modifier.bAllowSelf) { Causer->ApplyAbsorbingModifier(Modifier); }

			continue;
		}

		if (!IsSame && Modifier.bAllowTeam)
		{
			ApplyAbsorbingModifier(Modifier);
			continue;
		}

		if (Modifier.bAllowSelf) { Causer->ApplyAbsorbingModifier(Modifier); }
	}

	for (auto Modifier : Ability->OvertimeModifiers)
	{
		if (IsEnemy)
		{
			if (Modifier.bAllowEnemy) { ApplyOvertimeModifier(Modifier); }
			if (Modifier.bAllowSelf) { Causer->ApplyOvertimeModifier(Modifier); }

			continue;
		}

		if (!IsSame && Modifier.bAllowTeam)
		{
			ApplyOvertimeModifier(Modifier);
			continue;
		}

		if (Modifier.bAllowSelf) { Causer->ApplyOvertimeModifier(Modifier); }
	}

	for (auto Modifier : Ability->HealModifiers)
	{
		if (IsEnemy)
		{
			if (Modifier.bAllowEnemy) { ApplyHealModifier(Modifier); }
			if (Modifier.bAllowSelf) { Causer->ApplyHealModifier(Modifier); }

			continue;
		}

		if (!IsSame && Modifier.bAllowTeam)
		{
			ApplyHealModifier(Modifier);
			continue;
		}

		if (Modifier.bAllowSelf) { Causer->ApplyHealModifier(Modifier); }
	}

	for (auto Modifier : Ability->DamageModifiers)
	{
		if (IsEnemy)
		{
			if (Modifier.bAllowEnemy) { ApplyDamageModifier(Modifier); }
			if (Modifier.bAllowSelf) { Causer->ApplyDamageModifier(Modifier); }

			continue;
		}

		if (!IsSame && Modifier.bAllowTeam)
		{
			ApplyDamageModifier(Modifier);
			continue;
		}

		if (Modifier.bAllowSelf) { Causer->ApplyDamageModifier(Modifier); }
	}
}

void AArenaCharacter::ApplyDelayedModifiers(AAbilityBase* Ability)
{
	UWorld* World = GetWorld();
	if (!World || !Ability || !IsValidModifierState()) { return; }

	AArenaCharacter* Causer = Cast<AArenaCharacter>(Ability->CharacterOwner);
	if (!Causer) { return; }


	float Distance = FVector::Distance(Causer->GetActorLocation(), GetActorLocation());
	float Delay = Distance / Ability->ProjectileSpeed;
	FTimerHandle TimerHandle;
	FTimerDelegate TimerDel;

	TimerDel.BindUFunction(this, FName("ApplyModifiers"), Ability);
	World->GetTimerManager().SetTimer(TimerHandle, TimerDel, Delay, false);
}

void AArenaCharacter::ApplyAuraModifier(FAuraModifier Modifier)
{
	//if (Modifier.OnApplyHandler) { Modifier.OnApplyHandler(GetModifierInfo(Modifier)); }
}

void AArenaCharacter::ApplyBuffModifier(FBuffModifier Modifier)
{
	bool OnApply = false;
	bool OnRenew = false;

	if (!HasAuthority() || !Modifier.AbilityOwner) { return; }
	
	FModifierInfo ModifierInfo = GetModifierInfo(Modifier);
	int32 AbilityId = Modifier.AbilityOwner->GetUniqueID();

	// Target is Invunerable
	if (State == CS_Invunerable && Modifier.bIsHarmful == 1)
	{
		ModifierInfo.bDenied = true;
		ModifierInfo.TimeRemaining = 0.0f;

		MulticastNotifyModifierInfo(ModifierInfo);
		if (Modifier.OnDenied) { Modifier.OnDenied(ModifierInfo); }

		return;
	}

	if (BuffModifiers.Contains(AbilityId))
	{
		BuffModifiers[AbilityId].TimeRemaining = Modifier.TimeRemaining;	
		OnRenew = true;

		if (!BuffModifiers[AbilityId].AddStack()) { ModifierInfo.Stacks = BuffModifiers[AbilityId].Stacks; }
	}
	else
	{
		BuffModifiers.Add(AbilityId, Modifier);
		BuffModifiers[AbilityId].AddStack();
		OnApply = true;
	}

	RefreshBuffModifiersStatus();
	ModifierInfo.Stacks = BuffModifiers[AbilityId].Stacks;

	MulticastNotifyModifierInfo(ModifierInfo);

	if (OnApply && Modifier.OnApplyHandler) { Modifier.OnApplyHandler(ModifierInfo); }
	if (OnRenew && BuffModifiers[AbilityId].OnRenewHandler) { BuffModifiers[AbilityId].OnRenewHandler(ModifierInfo); }
}

void AArenaCharacter::ApplyAbsorbingModifier(FAbsorbingModifier Modifier)
{
	Modifier.AddStack();
	FModifierInfo ModifierInfo = GetModifierInfo(Modifier);

	// Target is Invunerable
	if (State == CS_Invunerable && Modifier.bIsHarmful == 1)
	{
		ModifierInfo.bDenied = true;
		ModifierInfo.TimeRemaining = 0.0f;
		MulticastNotifyModifierInfo(ModifierInfo);

		if (Modifier.OnDenied) { Modifier.OnDenied(ModifierInfo); }
		return;
	}

	AbsorbingModifiers.Add(Modifier);
	MulticastNotifyModifierInfo(ModifierInfo);

	if (Modifier.OnApplyHandler) { Modifier.OnApplyHandler(ModifierInfo); }
}

void AArenaCharacter::ApplyDamageModifier(FDamageModifier Modifier)
{
	if (!Modifier.AbilityOwner) { return; }

	bool OnDoDamage = false;

	Modifier.AddStack();
	FModifierInfo ModifierInfo = GetModifierInfo(Modifier);

	// Target is Invunerable
	if (State == CS_Invunerable && Modifier.bIsHarmful == 1)
	{
		ModifierInfo.bDenied = true;
		ModifierInfo.TimeRemaining = 0.0f;
		MulticastNotifyModifierInfo(ModifierInfo);

		if (Modifier.OnDenied) { Modifier.OnDenied(ModifierInfo); }
		return;
	}

	CalculateDamage(Modifier, ModifierInfo);
	if (ModifierInfo.Amount >= 0.0f)
	{
		Health.Damage(ModifierInfo.Amount);
		UpdateOnTakeDamage(ModifierInfo);
		OnDoDamage = true;
	}

	Mana.Damage(Modifier.Mana, Modifier.bIsPercent);
	Energy.Damage(Modifier.Energy, Modifier.bIsPercent);

	MulticastNotifyModifierInfo(ModifierInfo);

	if (Modifier.OnApplyHandler) { Modifier.OnApplyHandler(ModifierInfo); }
	if (OnDoDamage && Modifier.OnDoDamageHandler) { Modifier.OnDoDamageHandler(ModifierInfo); }

	if (Health.Value > 0.0f) { return; }

	AAbilityBase* Ability = Cast<AAbilityBase>(Modifier.AbilityOwner);
	if (!Ability) { return; }

	ACharacterBase* Causer = Cast<ACharacterBase>(Ability->CharacterOwner);
	if (!Causer) { return; }

	AArenaPlayerState* CauserPlayerState = Cast<AArenaPlayerState>(Causer->PlayerState);
	AArenaPlayerState* MyPlayerState = Cast<AArenaPlayerState>(PlayerState);

	if (!CauserPlayerState || !MyPlayerState) { return; }

	CauserPlayerState->ServerAddKill();
	MyPlayerState->ServerAddDeath();
}

void AArenaCharacter::ApplyOvertimeModifier(FOvertimeModifier Modifier)
{
	if (!HasAuthority() || !Modifier.AbilityOwner) { return; }

	bool OnApply = false;
	bool OnRenew = false;

	FModifierInfo ModifierInfo = GetModifierInfo(Modifier);
	int32 AbilityId = Modifier.AbilityOwner->GetUniqueID();

	// Target is Invunerable
	if (State == CS_Invunerable && Modifier.bIsHarmful == 1)
	{
		ModifierInfo.bDenied = true;
		ModifierInfo.TimeRemaining = 0.0f;
		MulticastNotifyModifierInfo(ModifierInfo);

		if (Modifier.OnDenied) { Modifier.OnDenied(ModifierInfo); }
		return;
	}

	if (OvertimeModifiers.Contains(AbilityId))
	{
		OvertimeModifiers[AbilityId].TimeRemaining = Modifier.TimeRemaining;
		OvertimeModifiers[AbilityId].AddStack();
		ModifierInfo.Stacks = OvertimeModifiers[AbilityId].Stacks;

		OnRenew = true;
	}
	else
	{
		OvertimeModifiers.Add(AbilityId, Modifier);
		OvertimeModifiers[AbilityId].AddStack();
		ModifierInfo.Stacks = OvertimeModifiers[AbilityId].Stacks;

		OnApply = true;

	}

	MulticastNotifyModifierInfo(ModifierInfo);

	if (OnApply && OvertimeModifiers[AbilityId].OnApplyHandler) { OvertimeModifiers[AbilityId].OnApplyHandler(ModifierInfo); }
	if (OnRenew && OvertimeModifiers[AbilityId].OnRenewHandler) { OvertimeModifiers[AbilityId].OnRenewHandler(ModifierInfo); }
}

void AArenaCharacter::ApplyHealModifier(FHealModifier Modifier)
{
	if (!Modifier.AbilityOwner) { return; }

	bool OnDoHeal = false;

	Modifier.AddStack();

	FModifierInfo ModifierInfo = GetModifierInfo(Modifier);

	CalculateHeal(Modifier, ModifierInfo);
	if (ModifierInfo.Amount >= 0.0f)
	{
		Health.Heal(ModifierInfo.Amount);
		UpdateOnTakeHeal(ModifierInfo);
		OnDoHeal = true;
	}

	Mana.Heal(Modifier.Mana, Modifier.bIsPercent);
	Energy.Heal(Modifier.Energy, Modifier.bIsPercent);

	MulticastNotifyModifierInfo(ModifierInfo);
	if (Modifier.OnApplyHandler) { Modifier.OnApplyHandler(ModifierInfo); }
	if (OnDoHeal && Modifier.OnDoHealHandler) { Modifier.OnDoHealHandler(ModifierInfo); }
}

void AArenaCharacter::CalculateHeal(FModifierBase Modifier, FModifierInfo &ModifierInfo)
{
	if (Modifier.HealthAmount <= 0.0f) { return; }

	AAbilityBase* Ability = Cast<AAbilityBase>(Modifier.AbilityOwner);
	if (!Ability) { return; }

	AArenaCharacter* CharacterOwner = Cast<AArenaCharacter>(Ability->CharacterOwner);
	if (!CharacterOwner) { return; }

	ModifierInfo.Amount = Health.GetModifierAmount(Modifier.HealthAmount, Modifier.bIsPercent);

	// Calculate the power.
	float PowerPercent = 0.0f;
	if (Modifier.School == MS_Magic)
	{
		PowerPercent += Modifier.ModifierMagicPower + CharacterOwner->MagicPower.Value;
	}
	else
	{
		PowerPercent += Modifier.ModifierPhysicalPower + CharacterOwner->PhysicalPower.Value;
	}

	// Calculates the Power Amount.
	float PowerAmount = UUtilities::GetAmountPercentage(ModifierInfo.Amount, PowerPercent);

	// Add a randomized Amount from Power Amount.
	ModifierInfo.Amount += PowerAmount * FMath::RandRange(0.0, 1.0f);

	// Check if is critical.
	float CriticalRate = Modifier.ModifierCritical + CharacterOwner->Critical.Value;
	ModifierInfo.bCritical = UUtilities::IsCritical(CriticalRate);

	if (ModifierInfo.bCritical) { ModifierInfo.Amount *= FMath::FRandRange(1.5, 2.0); }
}

void AArenaCharacter::ApplyCosts(AAbilityBase* Ability)
{
	if (Ability->HealthCost != 0.0f)
	{
		Health.Damage(Ability->HealthCost, Ability->bIsPercent);
	}

	if (Ability->ManaCost != 0.0f)
	{
		Mana.Damage(Ability->ManaCost, Ability->bIsPercent);
	}

	if (Ability->EnergyCost != 0.0f)
	{
		Energy.Damage(Ability->EnergyCost, Ability->bIsPercent);
	}
}

void AArenaCharacter::TickOvertimeModifier(FOvertimeModifier Modifier)
{
	if (!Modifier.AbilityOwner) { return; }

	AAbilityBase* Ability = Cast<AAbilityBase>(Modifier.AbilityOwner);
	if (!Ability) { return; }

	AArenaCharacter* CharacterOwner = Cast<AArenaCharacter>(Ability->CharacterOwner);
	if (!CharacterOwner) { return; }

	FModifierInfo ModifierInfo = GetModifierInfo(Modifier);
	ModifierInfo.Amount = Modifier.HealthAmount;

	if (Modifier.bIsHarmful)
	{
		CalculateDamage(Modifier, ModifierInfo);
		if (ModifierInfo.Amount >= 0.0f)
		{
			Health.Damage(ModifierInfo.Amount);
			UpdateOnTakeDamage(ModifierInfo);
			if (Modifier.OnDoDamageHandler) { Modifier.OnDoDamageHandler(ModifierInfo); }
		}

		Mana.Damage(Modifier.ManaAmount, Modifier.bIsPercent);
		Energy.Damage(Modifier.EnergyAmount, Modifier.bIsPercent);
	}
	else
	{
		CalculateHeal(Modifier, ModifierInfo);
		if (ModifierInfo.Amount >= 0.0f)
		{
			Health.Heal(ModifierInfo.Amount);
			UpdateOnTakeHeal(ModifierInfo);
			if (Modifier.OnDoHealHandler) { Modifier.OnDoHealHandler(ModifierInfo); }
		}

		Mana.Heal(Modifier.ManaAmount, Modifier.bIsPercent);
		Energy.Heal(Modifier.ManaAmount, Modifier.bIsPercent);
	}

	if (Modifier.OnTickHandler) { Modifier.OnTickHandler(ModifierInfo); }
	MulticastNotifyModifierInfo(ModifierInfo);

	if (!Modifier.bIsHarmful ||Health.Value > 0.0f) { return; }

	ACharacterBase* Causer = Cast<ACharacterBase>(CharacterOwner);
	if (!Causer) { return; }

	AArenaPlayerState* CauserPlayerState = Cast<AArenaPlayerState>(Causer->PlayerState);
	AArenaPlayerState* MyPlayerState = Cast<AArenaPlayerState>(PlayerState);

	if (!CauserPlayerState || !MyPlayerState) { return; }

	CauserPlayerState->ServerAddKill();
	MyPlayerState->ServerAddDeath();
}

void AArenaCharacter::CalculateDamage(FModifierBase Modifier, FModifierInfo &ModifierInfo)
{
	if (Modifier.HealthAmount <= 0.0f ) { return; }

	AAbilityBase* Ability = Cast<AAbilityBase>(Modifier.AbilityOwner);
	if (!Ability) { return; }

	AArenaCharacter* CharacterOwner = Cast<AArenaCharacter>(Ability->CharacterOwner);
	if (!CharacterOwner) { return; }

	ModifierInfo.Amount = Health.GetModifierAmount(Modifier.HealthAmount, Modifier.bIsPercent);

	// Calculate the power.
	float PowerPercent = 0.0f;
	if (Modifier.School == MS_Magic)
	{
		PowerPercent += Modifier.ModifierMagicPower + CharacterOwner->MagicPower.Value;
	}
	else
	{
		PowerPercent += Modifier.ModifierPhysicalPower + CharacterOwner->PhysicalPower.Value;
	}

	// Calculates the Power Amount.
	float PowerAmount = UUtilities::GetAmountPercentage(ModifierInfo.Amount, PowerPercent);
	
	// Add a randomized Amount from Power Amount.
	ModifierInfo.Amount += PowerAmount * FMath::RandRange(0.0, 1.0f);

	// Check if is critical.
	float CriticalRate = Modifier.ModifierCritical + CharacterOwner->Critical.Value;
	ModifierInfo.bCritical = UUtilities::IsCritical(CriticalRate);

	if (ModifierInfo.bCritical) { ModifierInfo.Amount *= FMath::FRandRange(1.5, 2.0); }

	CalculateDamageByBuffModifiers(Modifier, ModifierInfo);
	CalculateDamageByAbsorbingModifiers(Modifier, ModifierInfo);
}

void AArenaCharacter::CalculateDamageByBuffModifiers(FModifierBase Modifier, FModifierInfo &ModifierInfo)
{
	if (State == CS_Invunerable || ModifierInfo.Amount <= 0.0f) { return; }

	if (Modifier.School == MS_Magic)
	{
		ModifierInfo.Amount -= UUtilities::GetAmountPercentage(ModifierInfo.Amount, MagicDefense.Value);
	}
	else
	{
		ModifierInfo.Amount -= UUtilities::GetAmountPercentage(ModifierInfo.Amount, PhysicalDefense.Value);
	}

	ModifierInfo.Amount = FMath::Max(ModifierInfo.Amount, 0.0f);
}

void AArenaCharacter::CalculateDamageByAbsorbingModifiers(FModifierBase Modifier, FModifierInfo &ModifierInfo)
{
	if (State == CS_Invunerable || ModifierInfo.Amount <= 0.0f) { return; }

	for (int32 Index = 0; Index < AbsorbingModifiers.Num();)
	{
		if (AbsorbingModifiers[Index].School != Modifier.School)
		{
			Index++;
			continue;
		}

		if (AbsorbingModifiers[Index].Health > ModifierInfo.Amount)
		{
			ModifierInfo.Absorbed += ModifierInfo.Amount;
			AbsorbingModifiers[Index].Health -= ModifierInfo.Amount;
			ModifierInfo.Amount = 0.0f;
			return;
		}

		ModifierInfo.Amount -= AbsorbingModifiers[Index].Health;
		ModifierInfo.Absorbed -= AbsorbingModifiers[Index].Health;

		if (AbsorbingModifiers[Index].OnRemoveHandler)
		{
			AbsorbingModifiers[Index].OnRemoveHandler(ModifierInfo);
		}

		AbsorbingModifiers.RemoveAt(Index);
		
		if (ModifierInfo.Amount <= 0) { return; }

		Index++;
	}
}

void AArenaCharacter::UpdateOnTakeDamage(FModifierInfo ModifierInfo)
{
	for (auto Aura : AuraModifiers)
	{
		if (Aura.OnTakeDamageHandler) { Aura.OnTakeDamageHandler(ModifierInfo); }
	}

	TArray<int32> BuffKeys;
	BuffModifiers.GenerateKeyArray(BuffKeys);
	for (auto Key : BuffKeys)
	{
		if (BuffModifiers[Key].OnTakeDamageHandler) { BuffModifiers[Key].OnTakeDamageHandler(ModifierInfo); }
	}

	TArray<int32> OvertimeKeys;
	OvertimeModifiers.GenerateKeyArray(OvertimeKeys);
	for (auto Key : OvertimeKeys)
	{
		if (OvertimeModifiers[Key].OnTakeDamageHandler) { OvertimeModifiers[Key].OnTakeDamageHandler(ModifierInfo); }
	}
}

void AArenaCharacter::UpdateOnTakeHeal(FModifierInfo ModifierInfo)
{
	for (auto Aura : AuraModifiers)
	{
		if (Aura.OnTakeHealHandler) { Aura.OnTakeHealHandler(ModifierInfo); }
	}

	TArray<int32> BuffKeys;
	BuffModifiers.GenerateKeyArray(BuffKeys);
	for (auto Key : BuffKeys)
	{
		if (BuffModifiers[Key].OnTakeHealHandler) { BuffModifiers[Key].OnTakeHealHandler(ModifierInfo); }
	}

	TArray<int32> OvertimeKeys;
	OvertimeModifiers.GenerateKeyArray(OvertimeKeys);
	for (auto Key : OvertimeKeys)
	{
		if (OvertimeModifiers[Key].OnTakeHealHandler) { OvertimeModifiers[Key].OnTakeHealHandler(ModifierInfo); }
	}
}

void AArenaCharacter::UpdateOnBreak(FModifierInfo ModifierInfo)
{
	for (auto Aura : AuraModifiers)
	{
		if (Aura.OnBreakHandler) { Aura.OnBreakHandler(ModifierInfo); }
	}

	TArray<int32> BuffKeys;
	BuffModifiers.GenerateKeyArray(BuffKeys);
	for (auto Key : BuffKeys)
	{
		if (BuffModifiers[Key].OnBreakHandler) { BuffModifiers[Key].OnBreakHandler(ModifierInfo); }
	}

	TArray<int32> OvertimeKeys;
	OvertimeModifiers.GenerateKeyArray(OvertimeKeys);
	for (auto Key : OvertimeKeys)
	{
		if (OvertimeModifiers[Key].OnBreakHandler) { OvertimeModifiers[Key].OnBreakHandler(ModifierInfo); }
	}
}

FModifierInfo AArenaCharacter::GetDefaultModifierInfo(FModifierBase Modifier)
{
	AAbilityBase* Ability = Cast<AAbilityBase>(Modifier.AbilityOwner);
	if (!Ability) { return FModifierInfo(); }

	AArenaCharacter* CharacterOwner = Cast<AArenaCharacter>(Ability->CharacterOwner);
	if (!CharacterOwner) { return FModifierInfo(); }

	FModifierInfo ModifierInfo;
	ModifierInfo.Ability = Ability;
	ModifierInfo.Causer = CharacterOwner;
	ModifierInfo.Target = this;
	ModifierInfo.bIsDispellable = Modifier.bIsDispellable;
	ModifierInfo.ModifierIcon = Modifier.Icon;
	ModifierInfo.ModifierName = Modifier.Name;
	ModifierInfo.ModifierDescription = Modifier.Description;
	ModifierInfo.Stacks = Modifier.Stacks;

	return ModifierInfo;
}

FModifierInfo AArenaCharacter::GetModifierInfo(FAuraModifier Modifier)
{
	FModifierInfo ModifierInfo = GetDefaultModifierInfo(Modifier);
	ModifierInfo.Type = EAIT_Damage;
	ModifierInfo.bIsDispellable = false;

	return ModifierInfo;
}

FModifierInfo AArenaCharacter::GetModifierInfo(FBuffModifier Modifier)
{
	FModifierInfo ModifierInfo = GetDefaultModifierInfo(Modifier);
	ModifierInfo.Type = EAIT_Buff;
	ModifierInfo.Event = EABE_Apply;
	ModifierInfo.bIsDispellable = Modifier.bIsDispellable;
	ModifierInfo.bIsHarmful = Modifier.bIsHarmful;
	ModifierInfo.bExpires = Modifier.bUntilUse;
	ModifierInfo.TimeRemaining = Modifier.TimeRemaining;
	ModifierInfo.Target = this;

	return ModifierInfo;
}

FModifierInfo AArenaCharacter::GetModifierInfo(FAbsorbingModifier Modifier)
{
	FModifierInfo ModifierInfo = GetDefaultModifierInfo(Modifier);
	ModifierInfo.Type = EAIT_Buff;
	ModifierInfo.bIsDispellable = Modifier.bIsDispellable;
	ModifierInfo.bIsHarmful = Modifier.bIsHarmful;
	ModifierInfo.bExpires = Modifier.bUntilUse;
	ModifierInfo.TimeRemaining = Modifier.bUntilUse ? Modifier.TimeRemaining : 0.0f;

	return ModifierInfo;
}

FModifierInfo AArenaCharacter::GetModifierInfo(FDamageModifier Modifier)
{
	FModifierInfo ModifierInfo = GetDefaultModifierInfo(Modifier);
	ModifierInfo.Type = EAIT_Damage;
	ModifierInfo.bIsHarmful = true;

	return ModifierInfo;
}

FModifierInfo AArenaCharacter::GetModifierInfo(FOvertimeModifier Modifier)
{
	FModifierInfo ModifierInfo = GetDefaultModifierInfo(Modifier);
	ModifierInfo.bIsHarmful = Modifier.bIsHarmful;
	ModifierInfo.Type = ModifierInfo.bIsHarmful ? EAIT_Damage : EAIT_Heal;
	ModifierInfo.bIsDispellable = Modifier.bIsDispellable;
	ModifierInfo.TimeRemaining = Modifier.TimeRemaining;

	return ModifierInfo;
}

FModifierInfo AArenaCharacter::GetModifierInfo(FHealModifier Modifier)
{
	FModifierInfo ModifierInfo = GetDefaultModifierInfo(Modifier);
	ModifierInfo.Type = EAIT_Heal;
	ModifierInfo.bIsHarmful = false;

	return ModifierInfo;
}

void AArenaCharacter::SetupAbilities()
{
	SetAbility1();
	SetAbility2();
	SetAbility3();
	SetAbility4();
	SetAbility5();
	SetAbility6();
	SetAbility7();
	SetAbility8();
}

void AArenaCharacter::SetAbility1() {}
void AArenaCharacter::SetAbility2() {}
void AArenaCharacter::SetAbility3() {}
void AArenaCharacter::SetAbility4() {}
void AArenaCharacter::SetAbility5() {}
void AArenaCharacter::SetAbility6() {}
void AArenaCharacter::SetAbility7() {}
void AArenaCharacter::SetAbility8() {}

bool AArenaCharacter::RemoveBuffModifier(int32 Key, ACharacterBase* Breaker = NULL)
{
	if (!BuffModifiers.Contains(Key)) { return false; }

	FModifierInfo ModifierInfo = GetModifierInfo(BuffModifiers[Key]);
	ModifierInfo.Breaker = Breaker;
	ModifierInfo.Event = Breaker ? EABE_Dispell : EABE_Break;

	if (BuffModifiers[Key].OnBreakHandler) { BuffModifiers[Key].OnBreakHandler(ModifierInfo); }
	if (BuffModifiers[Key].OnRemoveHandler) { BuffModifiers[Key].OnRemoveHandler(ModifierInfo); }

	BuffModifiers.Remove(Key);
	MulticastNotifyModifierInfo(ModifierInfo);

	return true;
}

bool AArenaCharacter::RemoveRandomBuffModifier(bool bIsHarmful, ACharacterBase* Breaker = NULL)
{
	TArray<int32> BuffKeys;
	TArray<int32> Keys;
	BuffModifiers.GenerateKeyArray(Keys);
	for (auto Key : Keys)
	{
		if (BuffModifiers[Key].bIsHarmful == (bIsHarmful ? 1 : 0)) { BuffKeys.Push(Key); }
	}

	if (BuffKeys.Num() < 1) return false;
	int32 Key = BuffKeys[FMath::RandRange(0, BuffKeys.Num() - 1)];

	FModifierInfo ModifierInfo = GetModifierInfo(BuffModifiers[Key]);
	ModifierInfo.Breaker = Breaker;
	ModifierInfo.Event = Breaker ? EABE_Dispell : EABE_Break;

	if (BuffModifiers[Key].OnBreakHandler) { BuffModifiers[Key].OnBreakHandler(ModifierInfo); }
	if (BuffModifiers[Key].OnRemoveHandler) { BuffModifiers[Key].OnRemoveHandler(ModifierInfo); }

	BuffModifiers.Remove(Key);
	MulticastNotifyModifierInfo(ModifierInfo);

	return true;
}

int32 AArenaCharacter::RemoveBuffModifiers(bool bIsHarmful, ACharacterBase* Breaker = NULL)
{
	int32 Total = 0;

	TArray<int32> Keys;
	BuffModifiers.GenerateKeyArray(Keys);
	for (auto Key : Keys)
	{
		if (BuffModifiers[Key].bIsHarmful != (bIsHarmful ? 1 : 0)) { continue; }

		FModifierInfo ModifierInfo = GetModifierInfo(BuffModifiers[Key]);
		ModifierInfo.Breaker = Breaker;
		ModifierInfo.Event = Breaker ? EABE_Dispell : EABE_Break;

		if (BuffModifiers[Key].OnBreakHandler) { BuffModifiers[Key].OnBreakHandler(ModifierInfo); }
		if (BuffModifiers[Key].OnRemoveHandler) { BuffModifiers[Key].OnRemoveHandler(ModifierInfo); }

		MulticastNotifyModifierInfo(ModifierInfo);

		BuffModifiers.Remove(Key);
		MulticastNotifyModifierInfo(ModifierInfo);

		Total++;
	}

	return Total;
}

int32 AArenaCharacter::RemoveAllBuffModifiers(ACharacterBase* Breaker = NULL)
{
	int32 Total = BuffModifiers.Num();

	TArray<int32> Keys;
	BuffModifiers.GenerateKeyArray(Keys);
	for (auto Key : Keys)
	{
		FModifierInfo ModifierInfo = GetModifierInfo(BuffModifiers[Key]);
		ModifierInfo.Breaker = Breaker;
		ModifierInfo.Event = Breaker ? EABE_Dispell : EABE_Break;

		if (BuffModifiers[Key].OnBreakHandler) { BuffModifiers[Key].OnBreakHandler(ModifierInfo); }
		if (BuffModifiers[Key].OnRemoveHandler) { BuffModifiers[Key].OnRemoveHandler(ModifierInfo); }

		MulticastNotifyModifierInfo(ModifierInfo);
	}

	BuffModifiers.Empty();

	return Total;
}

bool AArenaCharacter::RemoveAbsorbingModifier(int32 Key, ACharacterBase* Breaker = NULL)
{
	if (AbsorbingModifiers.IsValidIndex(Key)) { return false; }

	FModifierInfo ModifierInfo = GetModifierInfo(AbsorbingModifiers[Key]);
	ModifierInfo.Breaker = Breaker;
	ModifierInfo.Event = Breaker ? EABE_Dispell : EABE_Break;

	if (AbsorbingModifiers[Key].OnBreakHandler) { AbsorbingModifiers[Key].OnBreakHandler(ModifierInfo); }
	if (AbsorbingModifiers[Key].OnRemoveHandler) { AbsorbingModifiers[Key].OnRemoveHandler(ModifierInfo); }

	AbsorbingModifiers.RemoveAt(Key);
	MulticastNotifyModifierInfo(ModifierInfo);

	return true;
}

bool AArenaCharacter::RemoveRandomAbsorbingModifier(bool bIsHarmful, ACharacterBase* Breaker = NULL)
{
	if (AbsorbingModifiers.Num() < 1) return false;
	int32 Key = FMath::RandRange(0, AbsorbingModifiers.Num() - 1);

	FModifierInfo ModifierInfo = GetModifierInfo(AbsorbingModifiers[Key]);
	ModifierInfo.Breaker = Breaker;
	ModifierInfo.Event = Breaker ? EABE_Dispell : EABE_Break;

	if (AbsorbingModifiers[Key].OnBreakHandler) { AbsorbingModifiers[Key].OnBreakHandler(ModifierInfo); }
	if (AbsorbingModifiers[Key].OnRemoveHandler) { AbsorbingModifiers[Key].OnRemoveHandler(ModifierInfo); }

	AbsorbingModifiers.RemoveAt(Key);
	MulticastNotifyModifierInfo(ModifierInfo);

	return true;
}

int32 AArenaCharacter::RemoveAbsorbingModifiers(bool bIsHarmful, ACharacterBase* Breaker = NULL)
{
	int32 Total = 0;

	for (int32 Key = 0; Key < AbsorbingModifiers.Num();)
	{
		if (AbsorbingModifiers[Key].bIsHarmful != (bIsHarmful ? 1 : 0))
		{
			Key++;
			continue;
		}

		FModifierInfo ModifierInfo = GetModifierInfo(AbsorbingModifiers[Key]);
		ModifierInfo.Breaker = Breaker;
		ModifierInfo.Event = Breaker ? EABE_Dispell : EABE_Break;

		if (AbsorbingModifiers[Key].OnBreakHandler) { AbsorbingModifiers[Key].OnBreakHandler(ModifierInfo); }
		if (AbsorbingModifiers[Key].OnRemoveHandler) { AbsorbingModifiers[Key].OnRemoveHandler(ModifierInfo); }

		AbsorbingModifiers.RemoveAt(Key);
		MulticastNotifyModifierInfo(ModifierInfo);
	}

	return Total;
}

int32 AArenaCharacter::RemoveAllAbsorbingModifiers(ACharacterBase* Breaker = NULL)
{
	int32 Total = AbsorbingModifiers.Num();

	for (int32 Key = 0; Key < AbsorbingModifiers.Num(); Key++)
	{
		FModifierInfo ModifierInfo = GetModifierInfo(AbsorbingModifiers[Key]);
		ModifierInfo.Breaker = Breaker;
		ModifierInfo.Event = Breaker ? EABE_Dispell : EABE_Break;

		if (AbsorbingModifiers[Key].OnBreakHandler) { AbsorbingModifiers[Key].OnBreakHandler(ModifierInfo); }
		if (AbsorbingModifiers[Key].OnRemoveHandler) { AbsorbingModifiers[Key].OnRemoveHandler(ModifierInfo); }
		MulticastNotifyModifierInfo(ModifierInfo);
	}

	AbsorbingModifiers.Empty();

	return Total;
}

int32 AArenaCharacter::RemoveSnareBuffModifiers(ACharacterBase* Breaker = NULL)
{
	int32 Total = 0;

	TArray<int32> Keys;
	BuffModifiers.GenerateKeyArray(Keys);
	for (int32 Key : Keys)
	{
		if (BuffModifiers[Key].bIsHarmful == 0 || BuffModifiers[Key].Speed == 0.0f) { continue; }

		FModifierInfo ModifierInfo = GetModifierInfo(BuffModifiers[Key]);
		ModifierInfo.Breaker = Breaker;
		ModifierInfo.Event = Breaker ? EABE_Dispell : EABE_Break;

		if (BuffModifiers[Key].OnBreakHandler) { BuffModifiers[Key].OnBreakHandler(ModifierInfo); }
		if (BuffModifiers[Key].OnRemoveHandler) { BuffModifiers[Key].OnRemoveHandler(ModifierInfo); }

		MulticastNotifyModifierInfo(ModifierInfo);

		BuffModifiers.Remove(Key);
		MulticastNotifyModifierInfo(ModifierInfo);

		Total++;
	}

	if (Total > 0) { RefreshBuffModifiersStatus(); }

	return Total;
}

int32 AArenaCharacter::RemoveStuckBuffModifiers(ACharacterBase* Breaker = NULL)
{
	int32 Total = 0;

	TArray<int32> Keys;
	BuffModifiers.GenerateKeyArray(Keys);

	for (int32 Key : Keys)
	{
		if (BuffModifiers[Key].bIsHarmful == false || BuffModifiers[Key].State != CS_Stuck) { continue; }

		FModifierInfo ModifierInfo = GetModifierInfo(BuffModifiers[Key]);
		ModifierInfo.Breaker = Breaker;
		ModifierInfo.Event = Breaker ? EABE_Dispell : EABE_Break;

		if (BuffModifiers[Key].OnBreakHandler) { BuffModifiers[Key].OnBreakHandler(ModifierInfo); }
		if (BuffModifiers[Key].OnRemoveHandler) { BuffModifiers[Key].OnRemoveHandler(ModifierInfo); }

		MulticastNotifyModifierInfo(ModifierInfo);

		BuffModifiers.Remove(Key);
		MulticastNotifyModifierInfo(ModifierInfo);

		Total++;
	}

	if (Total > 0) { RefreshBuffModifiersStatus(); }

	return Total;
}

bool AArenaCharacter::RemoveOvertimeModifier(int32 Key, ACharacterBase* Breaker = NULL)
{
	if (!OvertimeModifiers.Contains(Key)) { return false; }

	FModifierInfo ModifierInfo = GetModifierInfo(OvertimeModifiers[Key]);
	ModifierInfo.Breaker = Breaker;
	ModifierInfo.Event = Breaker ? EABE_Dispell : EABE_Break;

	if (OvertimeModifiers[Key].OnBreakHandler) { OvertimeModifiers[Key].OnBreakHandler(ModifierInfo); }
	if (OvertimeModifiers[Key].OnRemoveHandler) { OvertimeModifiers[Key].OnRemoveHandler(ModifierInfo); }

	OvertimeModifiers.Remove(Key);
	return true;
}

bool AArenaCharacter::RemoveRandomOvertimeModifier(bool bIsHarmful, ACharacterBase* Breaker = NULL)
{
	TArray<int32> BuffKeys;
	TArray<int32> Keys;
	OvertimeModifiers.GenerateKeyArray(Keys);
	for (auto Key : Keys)
	{
		if (OvertimeModifiers[Key].bIsHarmful == (bIsHarmful ? 1 : 0)) { BuffKeys.Push(Key); }
	}

	if (BuffKeys.Num() < 1) return false;
	int32 Key = BuffKeys[FMath::RandRange(0, BuffKeys.Num() - 1)];

	FModifierInfo ModifierInfo = GetModifierInfo(OvertimeModifiers[Key]);
	ModifierInfo.Breaker = Breaker;
	ModifierInfo.Event = Breaker ? EABE_Dispell : EABE_Break;

	if (OvertimeModifiers[Key].OnBreakHandler) { OvertimeModifiers[Key].OnBreakHandler(ModifierInfo); }
	if (OvertimeModifiers[Key].OnRemoveHandler) { OvertimeModifiers[Key].OnRemoveHandler(ModifierInfo); }

	OvertimeModifiers.Remove(Key);

	return true;
}

int32 AArenaCharacter::RemoveOvertimeModifiers(bool bIsHarmful, ACharacterBase* Breaker = NULL)
{
	int32 Total = 0;

	TArray<int32> Keys;
	OvertimeModifiers.GenerateKeyArray(Keys);
	for (auto Key : Keys)
	{
		if (OvertimeModifiers[Key].bIsHarmful != (bIsHarmful ? 1 : 0)) { continue; }

		FModifierInfo ModifierInfo = GetModifierInfo(OvertimeModifiers[Key]);
		ModifierInfo.Breaker = Breaker;
		ModifierInfo.Event = Breaker ? EABE_Dispell : EABE_Break;

		if (OvertimeModifiers[Key].OnBreakHandler) { OvertimeModifiers[Key].OnBreakHandler(ModifierInfo); }
		if (OvertimeModifiers[Key].OnRemoveHandler) { OvertimeModifiers[Key].OnRemoveHandler(ModifierInfo); }

		OvertimeModifiers.Remove(Key);
		Total++;
	}

	return Total;
}

int32 AArenaCharacter::RemoveAllOvertimeModifiers(ACharacterBase* Breaker = NULL)
{
	int32 Total = OvertimeModifiers.Num();

	TArray<int32> Keys;
	OvertimeModifiers.GenerateKeyArray(Keys);
	for (auto Key : Keys)
	{
		FModifierInfo ModifierInfo = GetModifierInfo(OvertimeModifiers[Key]);
		ModifierInfo.Breaker = Breaker;
		ModifierInfo.Event = Breaker ? EABE_Dispell : EABE_Break;

		if (OvertimeModifiers[Key].OnBreakHandler) { OvertimeModifiers[Key].OnBreakHandler(ModifierInfo); }
		if (OvertimeModifiers[Key].OnRemoveHandler) { OvertimeModifiers[Key].OnRemoveHandler(ModifierInfo); }
	}

	OvertimeModifiers.Empty();

	return Total;
}

// ==========================================================================================
// NETWORK
// ==========================================================================================

template<int32 Slot> void AArenaCharacter::InputStartAbility()
{
	ServerStartAbility(Slot);
}

void AArenaCharacter::ServerStartAbility_Implementation(int32 Slot)
{
	EAbilityValidation Validation = StartAbility(Slot);

	if (Validation != EAbilityValidation::ABV_Allowed) {
		FString ErrorMsg = GetErrorString(Validation);
		ClientNotification(ErrorMsg);
		return;
	}

	ClientStartAbility(Slot);
}

void AArenaCharacter::ClientStartAbility_Implementation(int32 Slot)
{
	 StartAbility(Slot);
}

void AArenaCharacter::MulticastSpawnProjectile_Implementation(AArenaCharacter* Causer, AArenaCharacter* CharacterTarget, AAbilityBase* Ability)
{
	UWorld* World = GetWorld();
	if (!World || !Causer || !CharacterTarget || !Ability || !Ability->Projectile) { return; }

	if (this->GetUniqueID() != Causer->GetUniqueID()) { return; }

	USkeletalMeshComponent* Mesh = Causer->GetMesh();
	if (!Mesh) { return; }

	FVector SocketLocation = Mesh->GetSocketLocation("hand_r_socket");
	AAbilityProjectile* Projectile = World->SpawnActor<AAbilityProjectile>(Ability->Projectile, SocketLocation, FRotator::ZeroRotator);
	if (!Projectile) { return; }

	Projectile->Speed = Ability->ProjectileSpeed;
	Projectile->Target = CharacterTarget;
}

void AArenaCharacter::ServerAddAbility_Implementation(TSubclassOf<AAbilityBase> AbilityBase, int32 Slot)
{
	AddAbility(AbilityBase, Slot);
	ClientAddAbility(AbilityBase, Slot);
}

void AArenaCharacter::ClientAddAbility_Implementation(TSubclassOf<AAbilityBase> AbilityBase, int32 Slot)
{
	AddAbility(AbilityBase, Slot);
}

void AArenaCharacter::ClientSetPlayerProfile_Implementation()
{
	AArenaPlayerState* MyPlayerState = Cast<AArenaPlayerState>(PlayerState);
	if (!MyPlayerState) { return; }

	UArenaGameInstance* MyGameInstance = Cast<UArenaGameInstance>(GetGameInstance());
	if (!MyGameInstance) { return; }

	FPlayerProfile PlayerProfile =  MyGameInstance->GetProfile();

	ServerSetPlayerProfile(PlayerProfile);
}

void AArenaCharacter::ServerSetPlayerProfile_Implementation(FPlayerProfile PlayerProfile)
{
	AArenaPlayerState* MyPlayerState = Cast<AArenaPlayerState>(PlayerState);
	if (!MyPlayerState) { return; }

	Name = MyPlayerState->PlayerName;
	Team = MyPlayerState->Team;

	if (MyPlayerState->CharacterClass != ECCL_None) { return; }

	MyPlayerState->ServerSetClass(PlayerProfile.PlayerClass);
	MyPlayerState->ServerSetPlayerName(PlayerProfile.PlayerName.ToString());

	UWorld* World = GetWorld();
	if (!World) { return; }

	AArenaGameMode* GameMode = Cast<AArenaGameMode>(World->GetAuthGameMode());
	if (!GameMode) { return; }

	TSubclassOf<APawn> PawnClass = GameMode->GetPawnClassByCharacterClass(PlayerProfile.PlayerClass);
	if (!PawnClass) { return; }

	SetPawn(PawnClass);
}

void AArenaCharacter::OnRep_PlayerClass()
{
	Super::OnRep_PlayerClass();
}

void AArenaCharacter::MulticastNotifyModifierInfo_Implementation(FModifierInfo ModifierInfo)
{
	AddModifierInfo(ModifierInfo);
	ModifierInfoDelegate.Broadcast(ModifierInfo);
}


void AArenaCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	/*DOREPLIFETIME(AArenaCharacter, AuraModifiers);
	DOREPLIFETIME(AArenaCharacter, BuffModifiers);
	DOREPLIFETIME(AArenaCharacter, OvertimeModifiers);
	DOREPLIFETIME(AArenaCharacter, AbsorbingModifiers);*/
}