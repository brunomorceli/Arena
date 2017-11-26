// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "ArenaCharacter.h"
#include "GlobalLibrary.h"

AArenaCharacter::AArenaCharacter()
{
	UWorld* World = GetWorld();
	if (!World) { return; }

	CastTimeRemaining = 0;

	GlobalCountdownTime = 1.0f;
	GlobalCountdownTimeRemaining = 0.0f;

	AuraModifiers = TArray<FAuraModifier>();
	AbsorbingModifiers = TArray<FAbsorbingModifier>();
}

void AArenaCharacter::BeginPlay()
{
	Super::BeginPlay();

	FTimerHandle MyTimer;
	GetWorldTimerManager().SetTimer(MyTimer, this, &AArenaCharacter::ClientSetPlayerProfile, 0.5, false);
}

void AArenaCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ValidateCast();
	UpdateTimers(DeltaTime);

	if (CastTimeRemaining == 0.0f && (AnimationState == CAS_Cast || AnimationState == CAS_Channel))
	{
		StopCast();
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

void AArenaCharacter::ValidateCast()
{
	if (IsCasting() && ValidateStartAbility(CastAbility->Slot) != ABV_Allowed) { StopCast(); }
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
		return CommitAbilityModifiers(CastAbility);
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
			FAbilityInfo AbilityInfo = GetAbilityInfo(BuffModifiers[Key]);
			AbilityInfo.Event = EABE_Expire;

			if (BuffModifiers[Key].OnExpireHandler) { BuffModifiers[Key].OnExpireHandler(AbilityInfo); }
			if (BuffModifiers[Key].OnRemoveHandler) { BuffModifiers[Key].OnRemoveHandler(AbilityInfo); }

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
	Health.Clamp(0.0f);

	Mana.Max += ManaBuff;
	Mana.Max -= ManaDebuff;
	Mana.Clamp(0.0f);

	Energy.Max += EnergyBuff;
	Energy.Max -= EnergyDebuff;
	Energy.Clamp(0.0f);

	Critical.Value += CriticalBuff;
	Critical.Value -= CriticalDebuff;

	Critical.Clamp(CriticalBuff - CriticalDebuff);

	Speed.Clamp(SpeedBuff - SpeedDebuff);

	MagicPower.Clamp(MagicPowerBuff - MagicPowerDebuff);
	MagicDefense.Clamp(MagicDefenseBuff - MagicDefenseDebuff);
	PhysicalPower.Clamp(PhysicalPowerBuff - PhysicalPowerDebuff);
	PhysicalDefense.Clamp(PhysicalDefenseBuff - PhysicalDefenseDebuff);

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

		FAbilityInfo AbilityInfo = GetAbilityInfo(OvertimeModifiers[Key]);
		AbilityInfo.Event = EABE_Expire;

		if (OvertimeModifiers[Key].ElapsedTime >= OvertimeModifiers[Key].TickTime)
		{
			TickOvertimeModifier(OvertimeModifiers[Key]);
			OvertimeModifiers[Key].ElapsedTime = FMath::Fmod(OvertimeModifiers[Key].ElapsedTime, OvertimeModifiers[Key].TickTime);

			if (OvertimeModifiers[Key].OnTickHandler) { OvertimeModifiers[Key].OnTickHandler(AbilityInfo); }
		}

		if (OvertimeModifiers[Key].TimeRemaining <= 0.0f)
		{
			if (OvertimeModifiers[Key].OnExpireHandler) { OvertimeModifiers[Key].OnExpireHandler(AbilityInfo); }
			if (OvertimeModifiers[Key].OnRemoveHandler) { OvertimeModifiers[Key].OnRemoveHandler(AbilityInfo); }

			OvertimeModifiers.Remove(Key);

			MulticastNotifyAbilityInfo(AbilityInfo);
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

bool AArenaCharacter::AddAbility(TSubclassOf<AAbilityBase> AbilityBase, int32 Slot)
{
	UWorld* World = GetWorld();
	if (!World) { return false; }

	AAbilityBase* Ability = World->SpawnActor<AAbilityBase>(AbilityBase);

	if (!Ability) { return false; }

	Ability->Slot = Slot;
	Ability->CharacterOwner = this;
	Abilities.Add(Slot, Ability);

	return true;
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

	if (Ability->AbilityType == ABST_Castable || Ability->AbilityType == ABST_Channeling) {
		StartCast(Slot);
		return ABV_Allowed;
	}

	CommitAbility(Slot);
	return ABV_Allowed;
}

void  AArenaCharacter::StopAbility(int32 Slot)
{
	if (!Abilities.Contains(Slot)) { return; }
	StopCast();
	State = CS_Idle;
}

void AArenaCharacter::StartCast(int32 Slot)
{
	if (!Abilities.Contains(Slot)) { return; }

	StopCast();

	CastAbility = Abilities[Slot];
	CastTimeRemaining = CastAbility->CastTime;
	ServerSetAnimState(CastAbility->AbilityType == ABST_Castable ? CAS_Cast : CAS_Channel, CastAbility->CastFX);
}

void AArenaCharacter::StopCast()
{
	CastTimeRemaining = 0.0f;
	CastAbility = NULL;
	ServerSetAnimState(CAS_None, NULL);
}

void AArenaCharacter::CommitAbility(int32 Slot)
{
	if (!Abilities.Contains(Slot)) { return; }

	AAbilityBase* Ability = Abilities[Slot];

	// Countdown.
	StopCast();
	Ability->StartCountdown();
	if (Ability->AbilityType == ABST_Instant) { StartGlobalCountdown(); }

	if (!HasAuthority()) { return; }

	CommitAbilityModifiers(Ability);

	TEnumAsByte<ECharacterAnimationState> AnimationState;
	switch (Ability->Slot)
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

void AArenaCharacter::CommitAbilityModifiers(AAbilityBase* Ability)
{
	UWorld* World = GetWorld();
	if (!World || !Ability) { return; }

	// Is a Target Ability.
	if (Ability->AreaType == ABA_Target)
	{
		AArenaCharacter* ArenaCharacter = Cast<AArenaCharacter>(Target);

		bool LineOfSight = GetController()->LineOfSightTo(ArenaCharacter, FVector::ZeroVector);
		if (!LineOfSight || !ArenaCharacter->IsValidModifierState()) { return; }

		ApplyCosts(Ability);

		// Must generate a projectile.
		if (Ability->CommitType == ABC_Projectile)
		{
			ArenaCharacter->ApplyDelayedModifiers(Ability);
			return MulticastSpawnProjectile(Cast<AArenaCharacter>(Target), Ability);
		}

		return ArenaCharacter->ApplyModifiers(Ability);
	}

	// Is a Directional Ability.
	if (Ability->AreaType == ABA_Directional)
	{
		TArray<ACharacterBase*> Characters;
		if (!DirectionalHitTest(Characters, Ability->DirectionalRadius, Ability->DirectionalRange)) { return; }

		ApplyCosts(Ability);

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
				return MulticastSpawnProjectile(ArenaCharacter, Ability);
			}

			ArenaCharacter->ApplyModifiers(Ability);
		}
	}

	// Is an Area On Effect Ability.
	if (Ability->AreaType == EAbilityArea::ABA_AreaOnEffect)
	{
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(World, ACharacterBase::StaticClass(), FoundActors);

		ApplyCosts(Ability);

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
				return MulticastSpawnProjectile(ArenaCharacter, Ability);
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
		if (Validation != ABV_Allowed) {
			if (!Ability->bAllowSelf) { return Validation; }
			Target = this;
			ServerSetTarget(this);
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
	float Distance = FVector::Distance(GetActorLocation(), Target->GetActorLocation());
	if (Distance < Ability->MinDistance) { return ABV_TooClose; }
	if (Distance > Ability->MaxDistance) { return ABV_TooFar; }

	if (!GetController()->LineOfSightTo(Target, FVector::ZeroVector)) { return EAbilityValidation::ABV_OutOfSight; }
	if (GetAngle(this, Target) > Ability->MaxAngle) { return EAbilityValidation::ABV_OutOfSight; }

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

	if (!GetController()->LineOfSightTo(Target, FVector::ZeroVector)) { return EAbilityValidation::ABV_OutOfSight; }
	if (GetAngle(this, Target) > Ability->MaxAngle) { return EAbilityValidation::ABV_OutOfSight; }

	return ABV_Allowed;
}

EAbilityValidation AArenaCharacter::ValidateStartInstant(AAbilityBase* Ability)
{
	if (IsGlobalCountdown()) { return ABV_NotReady; }

	// Directional or Area on Effect Validation.
	if (Ability->AreaType == ABA_Directional || Ability->AreaType == ABA_AreaOnEffect) { return ABV_Allowed; }

	// Target Abilities Validation.
	float Distance = FVector::Distance(GetActorLocation(), Target->GetActorLocation());
	if (Distance < Ability->MinDistance) { return ABV_TooClose; }
	if (Distance > Ability->MaxDistance) { return ABV_TooFar; }

	if (!GetController()->LineOfSightTo(Target, FVector::ZeroVector)) { return EAbilityValidation::ABV_OutOfSight; }
	if (GetAngle(this, Target) > Ability->MaxAngle) { return EAbilityValidation::ABV_OutOfSight; }

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
	if (Modifier.OnApplyHandler) { Modifier.OnApplyHandler(GetAbilityInfo(Modifier)); }
}

void AArenaCharacter::ApplyBuffModifier(FBuffModifier Modifier)
{
	if (!HasAuthority() || !Modifier.AbilityOwner) { return; }

	FAbilityInfo AbilityInfo = GetAbilityInfo(Modifier);
	int32 AbilityId = Modifier.AbilityOwner->GetUniqueID();

	if (BuffModifiers.Contains(AbilityId))
	{
		BuffModifiers[AbilityId].TimeRemaining = Modifier.TimeRemaining;	

		if (BuffModifiers[AbilityId].OnRenewHandler) { BuffModifiers[AbilityId].OnRenewHandler(AbilityInfo); }
		if (!BuffModifiers[AbilityId].AddStack())
		{
			return MulticastNotifyAbilityInfo(AbilityInfo);
		}
	}
	else
	{
		BuffModifiers.Add(AbilityId, Modifier);
		BuffModifiers[AbilityId].AddStack();
		if (Modifier.OnApplyHandler) { Modifier.OnApplyHandler(AbilityInfo); }
	}

	RefreshBuffModifiersStatus();

	MulticastNotifyAbilityInfo(AbilityInfo);
}

void AArenaCharacter::ApplyAbsorbingModifier(FAbsorbingModifier Modifier)
{
	AbsorbingModifiers.Add(Modifier);
	if (Modifier.OnApplyHandler) { Modifier.OnApplyHandler(GetAbilityInfo(Modifier)); }
}

void AArenaCharacter::ApplyDamageModifier(FDamageModifier Modifier)
{
	if (!Modifier.AbilityOwner) { return; }

	FAbilityInfo AbilityInfo = GetAbilityInfo(Modifier);

	CalculateDamage(Modifier, AbilityInfo);
	if (AbilityInfo.Amount >= 0.0f)
	{
		Health.Damage(AbilityInfo.Amount);
		UpdateOnTakeDamage(AbilityInfo);
		if (Modifier.OnDoDamageHandler) { Modifier.OnDoDamageHandler(AbilityInfo); }
	}

	Mana.Damage(Modifier.Mana, Modifier.bIsPercent);
	Energy.Damage(Modifier.Energy, Modifier.bIsPercent);

	if (Modifier.OnApplyHandler) { Modifier.OnApplyHandler(AbilityInfo); }
	MulticastNotifyAbilityInfo(AbilityInfo);

	if (Health.Value > 0.0f) { return; }

	ACharacterBase* Causer = Cast<ACharacterBase>(Modifier.AbilityOwner->CharacterOwner);
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

	FAbilityInfo AbilityInfo = GetAbilityInfo(Modifier);
	int32 AbilityId = Modifier.AbilityOwner->GetUniqueID();

	if (OvertimeModifiers.Contains(AbilityId))
	{
		OvertimeModifiers[AbilityId].TimeRemaining = Modifier.TimeRemaining;
		if (OvertimeModifiers[AbilityId].OnRenewHandler) { OvertimeModifiers[AbilityId].OnRenewHandler(AbilityInfo); }

		if (!OvertimeModifiers[AbilityId].AddStack())
		{
			return MulticastNotifyAbilityInfo(AbilityInfo);
		}
	}
	else
	{
		OvertimeModifiers.Add(AbilityId, Modifier);
		if (Modifier.OnApplyHandler) { Modifier.OnApplyHandler(AbilityInfo); }
	}

	MulticastNotifyAbilityInfo(AbilityInfo);
}

void AArenaCharacter::ApplyHealModifier(FHealModifier Modifier)
{
	if (!Modifier.AbilityOwner) { return; }

	FAbilityInfo AbilityInfo = GetAbilityInfo(Modifier);

	CalculateHeal(Modifier, AbilityInfo);
	if (AbilityInfo.Amount >= 0.0f)
	{
		Health.Heal(AbilityInfo.Amount);
		UpdateOnTakeHeal(AbilityInfo);
		if (Modifier.OnDoHealHandler) { Modifier.OnDoHealHandler(AbilityInfo); }
	}

	Mana.Heal(Modifier.Mana, Modifier.bIsPercent);
	Energy.Heal(Modifier.Energy, Modifier.bIsPercent);

	if (Modifier.OnApplyHandler) { Modifier.OnApplyHandler(AbilityInfo); }
	MulticastNotifyAbilityInfo(AbilityInfo);
}

void AArenaCharacter::CalculateHeal(FModifierBase Modifier, FAbilityInfo &AbilityInfo)
{
	if (Modifier.Health <= 0.0f) { return; }

	AbilityInfo.Amount = Health.GetModifierAmount(Modifier.Health, Modifier.bIsPercent);

	// Calculate the power.
	float PowerPercent = 0.0f;
	if (Modifier.School == MS_Magic)
	{
		PowerPercent += Modifier.ModifierMagicPower + Modifier.AbilityOwner->CharacterOwner->MagicPower.Value;
	}
	else
	{
		PowerPercent += Modifier.ModifierPhysicalPower + Modifier.AbilityOwner->CharacterOwner->PhysicalPower.Value;
	}

	// Calculates the Power Amount.
	float PowerAmount = UUtilities::GetAmountPercentage(AbilityInfo.Amount, PowerPercent);

	// Add a randomized Amount from Power Amount.
	AbilityInfo.Amount += PowerAmount * FMath::RandRange(0.0, 1.0f);

	// Check if is critical.
	float CriticalRate = Modifier.ModifierCritical + Modifier.AbilityOwner->CharacterOwner->Critical.Value;
	AbilityInfo.bCritical = UUtilities::IsCritical(CriticalRate);

	if (AbilityInfo.bCritical) { AbilityInfo.Amount *= FMath::FRandRange(1.5, 2.0); }
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

	FAbilityInfo AbilityInfo = GetAbilityInfo(Modifier);
	AbilityInfo.Amount = Modifier.HealthAmount;

	if (Modifier.bIsHarmful)
	{
		CalculateDamage(Modifier, AbilityInfo);
		if (AbilityInfo.Amount >= 0.0f)
		{
			Health.Damage(AbilityInfo.Amount);
			UpdateOnTakeDamage(AbilityInfo);
			if (Modifier.OnDoDamageHandler) { Modifier.OnDoDamageHandler(AbilityInfo); }
		}

		Mana.Damage(Modifier.ManaAmount, Modifier.bIsPercent);
		Energy.Damage(Modifier.EnergyAmount, Modifier.bIsPercent);
	}
	else
	{
		CalculateHeal(Modifier, AbilityInfo);
		if (AbilityInfo.Amount >= 0.0f)
		{
			Health.Heal(AbilityInfo.Amount);
			UpdateOnTakeHeal(AbilityInfo);
			if (Modifier.OnDoHealHandler) { Modifier.OnDoHealHandler(AbilityInfo); }
		}

		Mana.Heal(Modifier.ManaAmount, Modifier.bIsPercent);
		Energy.Heal(Modifier.ManaAmount, Modifier.bIsPercent);
	}

	if (Modifier.OnTickHandler) { Modifier.OnTickHandler(AbilityInfo); }
	MulticastNotifyAbilityInfo(AbilityInfo);

	if (!Modifier.bIsHarmful ||Health.Value > 0.0f) { return; }

	ACharacterBase* Causer = Cast<ACharacterBase>(Modifier.AbilityOwner->CharacterOwner);
	if (!Causer) { return; }

	AArenaPlayerState* CauserPlayerState = Cast<AArenaPlayerState>(Causer->PlayerState);
	AArenaPlayerState* MyPlayerState = Cast<AArenaPlayerState>(PlayerState);

	if (!CauserPlayerState || !MyPlayerState) { return; }

	CauserPlayerState->ServerAddKill();
	MyPlayerState->ServerAddDeath();
}

void AArenaCharacter::CalculateDamage(FModifierBase Modifier, FAbilityInfo &AbilityInfo)
{
	if (Modifier.Health <= 0.0f ) { return; }

	AbilityInfo.Amount = Health.GetModifierAmount(Modifier.Health, Modifier.bIsPercent);

	// Calculate the power.
	float PowerPercent = 0.0f;
	if (Modifier.School == MS_Magic)
	{
		PowerPercent += Modifier.ModifierMagicPower + Modifier.AbilityOwner->CharacterOwner->MagicPower.Value;
	}
	else
	{
		PowerPercent += Modifier.ModifierPhysicalPower + Modifier.AbilityOwner->CharacterOwner->PhysicalPower.Value;
	}

	// Calculates the Power Amount.
	float PowerAmount = UUtilities::GetAmountPercentage(AbilityInfo.Amount, PowerPercent);
	
	// Add a randomized Amount from Power Amount.
	AbilityInfo.Amount += PowerAmount * FMath::RandRange(0.0, 1.0f);

	// Check if is critical.
	float CriticalRate = Modifier.ModifierCritical + Modifier.AbilityOwner->CharacterOwner->Critical.Value;
	AbilityInfo.bCritical = UUtilities::IsCritical(CriticalRate);

	if (AbilityInfo.bCritical) { AbilityInfo.Amount *= FMath::FRandRange(1.5, 2.0); }

	CalculateDamageByBuffModifiers(Modifier, AbilityInfo);
	CalculateDamageByAbsorbingModifiers(Modifier, AbilityInfo);
}

void AArenaCharacter::CalculateDamageByBuffModifiers(FModifierBase Modifier, FAbilityInfo &AbilityInfo)
{
	if (State == CS_Invunerable || AbilityInfo.Amount <= 0.0f) { return; }

	if (Modifier.School == MS_Magic)
	{
		AbilityInfo.Amount -= UUtilities::GetAmountPercentage(AbilityInfo.Amount, MagicDefense.Value);
	}
	else
	{
		AbilityInfo.Amount -= UUtilities::GetAmountPercentage(AbilityInfo.Amount, PhysicalDefense.Value);
	}

	AbilityInfo.Amount = FMath::Max(AbilityInfo.Amount, 0.0f);
}

void AArenaCharacter::CalculateDamageByAbsorbingModifiers(FModifierBase Modifier, FAbilityInfo &AbilityInfo)
{
	if (State == CS_Invunerable || AbilityInfo.Amount <= 0.0f) { return; }

	for (int32 Index = 0; Index < AbsorbingModifiers.Num();)
	{
		if (AbsorbingModifiers[Index].School != Modifier.School)
		{
			Index++;
			continue;
		}

		if (AbsorbingModifiers[Index].Health > AbilityInfo.Amount)
		{
			AbilityInfo.Absorbed += AbilityInfo.Amount;
			AbsorbingModifiers[Index].Health -= AbilityInfo.Amount;
			AbilityInfo.Amount = 0.0f;
			return;
		}

		AbilityInfo.Amount -= AbsorbingModifiers[Index].Health;
		AbilityInfo.Absorbed -= AbsorbingModifiers[Index].Health;

		if (AbsorbingModifiers[Index].OnRemoveHandler)
		{
			AbsorbingModifiers[Index].OnRemoveHandler(AbilityInfo);
		}

		AbsorbingModifiers.RemoveAt(Index);
		
		if (AbilityInfo.Amount <= 0) { return; }

		Index++;
	}
}

void AArenaCharacter::UpdateOnTakeDamage(FAbilityInfo AbilityInfo)
{
	for (auto Aura : AuraModifiers)
	{
		if (Aura.OnTakeDamageHandler) { Aura.OnTakeDamageHandler(AbilityInfo); }
	}

	TArray<int32> BuffKeys;
	BuffModifiers.GenerateKeyArray(BuffKeys);
	for (auto Key : BuffKeys)
	{
		if (BuffModifiers[Key].OnTakeDamageHandler) { BuffModifiers[Key].OnTakeDamageHandler(AbilityInfo); }
	}

	TArray<int32> OvertimeKeys;
	OvertimeModifiers.GenerateKeyArray(OvertimeKeys);
	for (auto Key : OvertimeKeys)
	{
		if (OvertimeModifiers[Key].OnTakeDamageHandler) { OvertimeModifiers[Key].OnTakeDamageHandler(AbilityInfo); }
	}
}

void AArenaCharacter::UpdateOnTakeHeal(FAbilityInfo AbilityInfo)
{
	for (auto Aura : AuraModifiers)
	{
		if (Aura.OnTakeHealHandler) { Aura.OnTakeHealHandler(AbilityInfo); }
	}

	TArray<int32> BuffKeys;
	BuffModifiers.GenerateKeyArray(BuffKeys);
	for (auto Key : BuffKeys)
	{
		if (BuffModifiers[Key].OnTakeHealHandler) { BuffModifiers[Key].OnTakeHealHandler(AbilityInfo); }
	}

	TArray<int32> OvertimeKeys;
	OvertimeModifiers.GenerateKeyArray(OvertimeKeys);
	for (auto Key : OvertimeKeys)
	{
		if (OvertimeModifiers[Key].OnTakeHealHandler) { OvertimeModifiers[Key].OnTakeHealHandler(AbilityInfo); }
	}
}

void AArenaCharacter::UpdateOnBreak(FAbilityInfo AbilityInfo)
{
	for (auto Aura : AuraModifiers)
	{
		if (Aura.OnBreakHandler) { Aura.OnBreakHandler(AbilityInfo); }
	}

	TArray<int32> BuffKeys;
	BuffModifiers.GenerateKeyArray(BuffKeys);
	for (auto Key : BuffKeys)
	{
		if (BuffModifiers[Key].OnBreakHandler) { BuffModifiers[Key].OnBreakHandler(AbilityInfo); }
	}

	TArray<int32> OvertimeKeys;
	OvertimeModifiers.GenerateKeyArray(OvertimeKeys);
	for (auto Key : OvertimeKeys)
	{
		if (OvertimeModifiers[Key].OnBreakHandler) { OvertimeModifiers[Key].OnBreakHandler(AbilityInfo); }
	}
}

FAbilityInfo AArenaCharacter::GetDefaultAbilityInfo(FModifierBase Modifier)
{
	FAbilityInfo AbilityInfo;
	AbilityInfo.Ability = Modifier.AbilityOwner;
	AbilityInfo.Causer = Modifier.AbilityOwner->CharacterOwner;
	AbilityInfo.Target = this;
	AbilityInfo.bIsDispellable = Modifier.bIsDispellable;
	AbilityInfo.ModifierIcon = Modifier.Icon;
	AbilityInfo.ModifierName = Modifier.Name;

	return AbilityInfo;
}

FAbilityInfo AArenaCharacter::GetAbilityInfo(FAuraModifier Modifier)
{
	FAbilityInfo AbilityInfo = GetDefaultAbilityInfo(Modifier);
	AbilityInfo.Type = EAIT_Damage;
	AbilityInfo.bIsDispellable = false;

	return AbilityInfo;
}

FAbilityInfo AArenaCharacter::GetAbilityInfo(FBuffModifier Modifier)
{
	FAbilityInfo AbilityInfo = GetDefaultAbilityInfo(Modifier);
	AbilityInfo.Type = EAIT_Buff;
	AbilityInfo.Event = EABE_Apply;
	AbilityInfo.bIsDispellable = Modifier.bIsDispellable;
	AbilityInfo.bIsHarmful = Modifier.bIsHarmful;
	AbilityInfo.bExpires = Modifier.bUntilUse;
	AbilityInfo.TimeRemaining = Modifier.TimeRemaining;
	AbilityInfo.Target = this;
	AbilityInfo.Causer = Modifier.AbilityOwner->CharacterOwner;

	return AbilityInfo;
}

FAbilityInfo AArenaCharacter::GetAbilityInfo(FAbsorbingModifier Modifier)
{
	FAbilityInfo AbilityInfo = GetDefaultAbilityInfo(Modifier);
	AbilityInfo.Type = EAIT_Buff;
	AbilityInfo.bIsDispellable = Modifier.bIsDispellable;
	AbilityInfo.bIsHarmful = Modifier.bIsHarmful;
	AbilityInfo.bExpires = Modifier.bUntilUse;
	AbilityInfo.TimeRemaining = Modifier.bUntilUse ? Modifier.TimeRemaining : 0.0f;

	return AbilityInfo;
}

FAbilityInfo AArenaCharacter::GetAbilityInfo(FDamageModifier Modifier)
{
	FAbilityInfo AbilityInfo = GetDefaultAbilityInfo(Modifier);
	AbilityInfo.Type = EAIT_Damage;
	AbilityInfo.bIsHarmful = true;

	return AbilityInfo;
}

FAbilityInfo AArenaCharacter::GetAbilityInfo(FOvertimeModifier Modifier)
{
	FAbilityInfo AbilityInfo = GetDefaultAbilityInfo(Modifier);
	AbilityInfo.bIsHarmful = Modifier.bIsHarmful;
	AbilityInfo.Type = AbilityInfo.bIsHarmful ? EAIT_Damage : EAIT_Heal;
	AbilityInfo.bIsDispellable = Modifier.bIsDispellable;
	AbilityInfo.TimeRemaining = Modifier.TimeRemaining;

	return AbilityInfo;
}

FAbilityInfo AArenaCharacter::GetAbilityInfo(FHealModifier Modifier)
{
	FAbilityInfo AbilityInfo = GetDefaultAbilityInfo(Modifier);
	AbilityInfo.Type = EAIT_Heal;
	AbilityInfo.bIsHarmful = false;

	return AbilityInfo;
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

void AArenaCharacter::MulticastSpawnProjectile_Implementation(AArenaCharacter* CharacterTarget, AAbilityBase* Ability)
{
	UWorld* World = GetWorld();
	if (!World || !CharacterTarget || !Ability || !Ability->Projectile) { return; }

	USkeletalMeshComponent* Mesh = GetMesh();
	if (!Mesh) { return; }

	FVector SocketLocation = Mesh->GetSocketLocation("hand_r_socket");
	AAbilityProjectile* Projectile = World->SpawnActor<AAbilityProjectile>(Ability->Projectile, SocketLocation, GetActorRotation());
	if (!Projectile) { return; }

	Projectile->Speed = Ability->ProjectileSpeed;
	Projectile->Target = CharacterTarget;
	if (Ability->ProjectileHitParticle) { Projectile->HitParticle = Ability->ProjectileHitParticle; }
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
	UArenaGameInstance* MyGameInstance = Cast<UArenaGameInstance>(GetGameInstance());
	if (!MyGameInstance) { return; }

	FPlayerProfile PlayerProfile =  MyGameInstance->GetProfile();

	ServerSetPlayerProfile(PlayerProfile);
}

void AArenaCharacter::ServerSetPlayerProfile_Implementation(FPlayerProfile PlayerProfile)
{
	AArenaPlayerState* ArenaPlayerState = Cast<AArenaPlayerState>(PlayerState);
	if (!ArenaPlayerState) { return; }

	Name = PlayerProfile.PlayerName.ToString();
	PlayerClass = PlayerProfile.PlayerClass;

	ArenaPlayerState->ServerSetClass(PlayerProfile.PlayerClass);
	ArenaPlayerState->ServerSetPlayerName(PlayerProfile.PlayerName.ToString());

	OnRep_PlayerClass();

	FClassPreset ClassPreset = UGlobalLibrary::GetClass(PlayerProfile.PlayerClass);

	for (int32 Index = 0; Index <ClassPreset.Abilities.Num(); Index++)
	{
		ServerAddAbility(ClassPreset.Abilities[Index], Index + 1);
	}
}

void AArenaCharacter::OnRep_PlayerClass()
{
	Super::OnRep_PlayerState();

	FClassPreset ClassPreset = UGlobalLibrary::GetClass(PlayerClass);

	if (ClassPreset.CharacterMesh)
	{
		GetMesh()->SetSkeletalMesh(ClassPreset.CharacterMesh, false);
	}

	LeftHandWeapon->SetSkeletalMesh(ClassPreset.LeftWeapon, false);
	RightHandWeapon->SetSkeletalMesh(ClassPreset.RightWeapon, false);
}

void AArenaCharacter::MulticastNotifyAbilityInfo_Implementation(FAbilityInfo AbilityInfo)
{
	AddAbilityInfo(AbilityInfo);
	AbilityInfoDelegate.Broadcast(AbilityInfo);
}


void AArenaCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	/*DOREPLIFETIME(AArenaCharacter, AuraModifiers);
	DOREPLIFETIME(AArenaCharacter, BuffModifiers);
	DOREPLIFETIME(AArenaCharacter, OvertimeModifiers);
	DOREPLIFETIME(AArenaCharacter, AbsorbingModifiers);*/
}