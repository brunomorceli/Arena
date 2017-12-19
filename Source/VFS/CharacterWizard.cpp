// Fill out your copyright notice in the Description page of Project Settings.

#include "CharacterWizard.h"

ACharacterWizard::ACharacterWizard()
{
	PlayerClass = ECharacterClass::ECCL_Wizard;

	Critical.ValueBase = 20.0f;
	Critical.Reset();
}

void ACharacterWizard::BeginPlay()
{
	Super::BeginPlay();
}

void ACharacterWizard::SetAbility1()
{
	Super::SetAbility1();

	AAbilityBase* Ability = AddAbility(AAbilityBase::StaticClass(), 1);
	if (!Ability) { return; }

	Ability->Name = "Fireball";
	Ability->Description = "150HP and 66% chance to add fire dot.";
	Ability->AbilityType = ABST_Castable;
	Ability->CastTime = 1.5f;
	Ability->AreaType = ABA_Target;
	Ability->CommitType = ABC_Projectile;
	Ability->Projectile = UGlobalLibrary::GetProjectile(3);
	Ability->ProjectileSpeed = 2000.0f;
	Ability->MaxDistance = 1800.0f;
	Ability->MinDistance = 0.0f;
	Ability->ManaCost = 10.0f;
	Ability->bAllowEnemy = true;
	Ability->bAllowSelf = false;
	Ability->bAllowTeam = false;
	Ability->LoadIcon("/Game/Sprites/Icons/309.309");

	FDamageModifier Damage;
	Damage.AbilityOwner = Abilities[1];
	Damage.Icon = Abilities[1]->Icon;
	Damage.Health = 150.0f;
	Damage.OnApplyHandler = [](FAbilityInfo AbilityInfo) {
		AArenaCharacter* Target = Cast<AArenaCharacter>(AbilityInfo.Target);
		if (!Target) { return; }
		if (!UUtilities::IsCritical(66.66f)) { return; }

		FOvertimeModifier Dot;
		Dot.AbilityOwner = AbilityInfo.Ability;
		Dot.Name = "Fireball";
		Dot.Description = "Does 30HP damage every 2 seconds.";
		Dot.Icon = AbilityInfo.Ability->Icon;
		Dot.TimeRemaining = 10.0f;
		Dot.TickTime = 2.0f;
		Dot.Health = 30.0f;
		Dot.bIsStackable = true;
		Dot.MaxStacks = 4;
		Dot.bIsHarmful = true;
		Dot.bAllowEnemy = true;
		Dot.bAllowSelf = false;
		Dot.bAllowTeam = false;
		Dot.OnTickHandler = [](FAbilityInfo AbilityInfo) {
			AArenaCharacter* Target = Cast<AArenaCharacter>(AbilityInfo.Target);
			if (!Target) { return; }
			Target->MulticastPlayFX(UGlobalLibrary::GetAbilityHitFX(5));
		};

		Target->ApplyOvertimeModifier(Dot);
	};
	Ability->DamageModifiers.Add(Damage);
}

void ACharacterWizard::SetAbility2()
{
	Super::SetAbility2();

	AAbilityBase* Ability = AddAbility(AAbilityBase::StaticClass(), 2);
	if (!Ability) { return; }

	Ability->Name = "Frost Bolt";
	Ability->Description = "300HP and Slow by 50% by 7 seconds.";
	Ability->AbilityType = ABST_Castable;
	Ability->AreaType = ABA_Target;
	Ability->CommitType = ABC_Projectile;
	Ability->Projectile = UGlobalLibrary::GetProjectile(2);
	Ability->ProjectileSpeed = 2000.0f;
	Ability->MaxDistance = 1800.0f;
	Ability->MinDistance = 0.0f;
	Ability->ManaCost = 100.0f;
	Ability->CastTime = 2.2f;
	Ability->bAllowEnemy = true;
	Ability->bAllowSelf = false;
	Ability->bAllowTeam = false;
	Ability->LoadIcon("/Game/Sprites/Icons/105.105");

	FDamageModifier Damage;
	Damage.AbilityOwner = Ability;
	Damage.Icon = Ability->Icon;
	Damage.Health = 250.0f;
	Damage.OnApplyHandler = [](FAbilityInfo AbilityInfo) {
		if (AbilityInfo.Target) { AbilityInfo.Target->MulticastPlayFX(UGlobalLibrary::GetAbilityHitFX(7)); }
	};
	Ability->DamageModifiers.Add(Damage);

	FBuffModifier Debuff;
	Debuff.AbilityOwner = Ability;
	Debuff.Icon = Ability->Icon;
	Debuff.Name = "Frost Bolt";
	Debuff.Description = "Snared by 50%.";
	Debuff.Speed = 300.0f;
	Debuff.School = MS_Magic;
	Debuff.bAllowSelf = false;
	Debuff.bAllowTeam = false;
	Debuff.bAllowEnemy = true;
	Debuff.bIsHarmful = true;
	Debuff.TimeRemaining = 7.0f;

	Ability->BuffModifiers.Add(Debuff);
}

void ACharacterWizard::SetAbility3()
{
	Super::SetAbility3();

	AAbilityBase* Ability = AddAbility(AAbilityBase::StaticClass(), 3);
	if (!Ability) { return; }

	Ability->Name = "Live Furnace";
	Ability->Description = "Burn the target doing 250HP Damage and slow the target by 70% for 2 seconds.";
	Ability->CountdownTime = 15.0f;
	Ability->MaxDistance = 1500.0f;
	Ability->MaxAngle = 360.0f;
	Ability->AbilityType = ABST_Instant;
	Ability->AreaType = ABA_Target;
	Ability->ManaCost = 130.0f;
	Ability->bAllowEnemy = true;
	Ability->bAllowSelf = false;
	Ability->bAllowTeam = false;
	Ability->LoadIcon("/Game/Sprites/Icons/345.345");

	FDamageModifier Damage;
	Damage.AbilityOwner = Ability;
	Damage.Icon = Ability->Icon;
	Damage.Health = 100.0f;
	Damage.OnApplyHandler = [](FAbilityInfo AbilityInfo) {
		AbilityInfo.Target->MulticastPlayFX(UGlobalLibrary::GetAbilityHitFX(6));
	};

	Ability->DamageModifiers.Add(Damage);

	FBuffModifier Slow;
	Slow.AbilityOwner = Ability;
	Slow.Name = "Live Funace";
	Slow.Description = "70% Slow.";
	Slow.Icon = Ability->Icon;
	Slow.MaxStacks = 1;
	Slow.Speed = 600.0f * 0.7f;
	Slow.bIsHarmful = true;
	Slow.TimeRemaining = 2.0f;
	Ability->BuffModifiers.Add(Slow);
}

void ACharacterWizard::SetAbility4()
{
	Super::SetAbility4();

	AAbilityBase* Ability = AddAbility(AAbilityBase::StaticClass(), 4);
	if (!Ability) { return; }

	Ability->Name = "Force Wall";
	Ability->Description = "Add a force wall that only the invoker would overlap for 5 seconds.";
	Ability->CountdownTime = 60.0f;
	Ability->AbilityType = ABST_Instant;
	Ability->AreaType = ABA_Target;
	Ability->ManaCost = 200.0f;
	Ability->bAllowSelf = true;
	Ability->bAllowEnemy = false;
	Ability->bAllowTeam = false;
	Ability->LoadIcon("/Game/Sprites/Icons/18.18");

	Ability->CommitFX = UGlobalLibrary::GetAbilityUseFX(7);
}

void ACharacterWizard::SetAbility5()
{
	Super::SetAbility5();

	AAbilityBase* Ability = AddAbility(AAbilityBase::StaticClass(), 5);
	if (!Ability) { return; }

	Ability->Name = "Frost Nova";
	Ability->Description = "Snare all enemy targets whithin X yards for 4s.";
	Ability->MaxDistance = 600.0f;
	Ability->AbilityType = ABST_Instant;
	Ability->AreaType = ABA_AreaOnEffect;
	Ability->MaxAngle = 360.0f;
	Ability->ManaCost = 100.0f;
	Ability->CountdownTime = 30.0f;
	Ability->CommitFX = UGlobalLibrary::GetAbilityUseFX(8);
	Ability->LoadIcon("/Game/Sprites/Icons/118.118");

	FDamageModifier Damage;
	Damage.Name = "Frost Nova Damage";
	Damage.AbilityOwner = Ability;
	Damage.Icon = Ability->Icon;
	Damage.Health = 100.0f;
	Damage.bIsHarmful = true;
	Ability->DamageModifiers.Add(Damage);

	FBuffModifier Root;
	Root.AbilityOwner = Ability;
	Root.Icon = Ability->Icon;
	Root.Name = "Frost Nova";
	Root.Description = "Incapable to move.";
	Root.State = CS_Stuck;
	Root.TimeRemaining = 4.0f;

	Ability->BuffModifiers.Add(Root);
}

void ACharacterWizard::SetAbility6()
{
	Super::SetAbility6();

	AAbilityBase* Ability = AddAbility(AAbilityBase::StaticClass(), 6);
	if (!Ability) { return; }

	Ability->Name = "Magic Trigger";
	Ability->Description = "Silence the target for 5 seconds after he use any ability.";
	Ability->AbilityType = ABST_Instant;
	Ability->AreaType = ABA_Target;
	Ability->MaxDistance = 1800.0f;
	Ability->ManaCost = 100.0f;
	Ability->CountdownTime = 22.0f;
	Ability->bAllowEnemy = true;
	Ability->bAllowSelf = false;
	Ability->bAllowTeam = false;
	Ability->CommitFX = UGlobalLibrary::GetAbilityUseFX(2);
	Ability->LoadIcon("/Game/Sprites/Icons/16.16");

	FBuffModifier Debuff;
	Debuff.AbilityOwner = Ability;
	Debuff.Icon = Ability->Icon;
	Debuff.Name = "Magic Trigger";
	Debuff.Description = "In the next 2 seconds if the affect target to use any magical ability he will be silenced for 5 seconds.";
	Debuff.School = MS_Magic;
	Debuff.bAllowSelf = false;
	Debuff.bAllowTeam = false;
	Debuff.bAllowEnemy = true;
	Debuff.bIsHarmful = true;
	Debuff.TimeRemaining = 2.0f;
	Debuff.OnUseAbility = [](FAbilityInfo AbilityInfo) {
		AArenaCharacter* Target = Cast<AArenaCharacter>(AbilityInfo.Target);
		if (!Target || !AbilityInfo.Ability || AbilityInfo.Ability->SchoolType != ABS_Magic) { return; }

		Target->MulticastPlayFX(UGlobalLibrary::GetAbilityHitFX(7));

		Target->RemoveBuffModifier(AbilityInfo.Ability->GetUniqueID(), NULL);

		FBuffModifier Silence;
		Silence.AbilityOwner = AbilityInfo.Ability;
		Silence.Icon = AbilityInfo.Ability->Icon;
		Silence.Name = "Magic Trigger";
		Silence.Description = "Silenced for 5s.";
		Silence.School = MS_Magic;
		Silence.bIsHarmful = true;
		Silence.State = CS_Stun;
		Silence.TimeRemaining = 5.0f;

		Target->ApplyBuffModifier(Silence);
	};
	Ability->BuffModifiers.Add(Debuff);
}

void ACharacterWizard::SetAbility7()
{
	Super::SetAbility7();

	AAbilityBase* Ability = AddAbility(AAbilityBase::StaticClass(), 7);
	if (!Ability) { return; }

	Ability->Name = "Magic Shell";
	Ability->Description = "Gives invulnerability for 1 second to you or an allied target..";
	Ability->MaxDistance = 300.0f;
	Ability->AbilityType = ABST_Instant;
	Ability->AreaType = ABA_Target;
	Ability->MaxDistance = 1200.0f;
	Ability->CountdownTime = 35.0f;
	Ability->ManaCost = 100.0f;
	Ability->bAllowEnemy = false;
	Ability->bAllowTeam = true;
	Ability->bAllowSelf = true;
	Ability->LoadIcon("/Game/Sprites/Icons/121.121");

	FBuffModifier Buff;
	Buff.AbilityOwner = Ability;
	Buff.Icon = Ability->Icon;
	Buff.Name = "Magic Shell";
	Buff.Description = "Invulnerable.";
	Buff.bAllowEnemy = false;
	Buff.bAllowTeam = true;
	Buff.bAllowSelf = true;
	Buff.State = CS_Invunerable;
	Buff.TimeRemaining = 1.0f;
	Buff.OnApplyHandler = [](FAbilityInfo AbilityInfo) {
		AbilityInfo.Target->MulticastPlayFX(UGlobalLibrary::GetAbilityHitFX(8));

		AArenaCharacter* Target = Cast<AArenaCharacter>(AbilityInfo.Target);
		if (!Target) { return; }

		Target->RemoveSnareBuffModifiers(AbilityInfo.Causer);
		Target->RemoveStuckBuffModifiers(AbilityInfo.Causer);
	};

	Ability->BuffModifiers.Add(Buff);
}

void ACharacterWizard::SetAbility8()
{
	Super::SetAbility8();

	AAbilityBase* Ability = AddAbility(AAbilityBase::StaticClass(), 8);
	if (!Ability) { return; }

	Ability->Name = "Laser Beam";
	Ability->Description = "100 Damage every 1 second.";
	Ability->AbilityType = ABST_Channeling;
	Ability->AreaType = ABA_Target;
	Ability->MaxDistance = 1800.0f;
	Ability->MinDistance = 0.0f;
	Ability->ManaCost = 400.0f;
	Ability->CountdownTime = 30.0f;
	Ability->CastTime = 3.0f;
	Ability->ChannelingTime = 0.5f;
	Ability->ChannelingTotalTime = 0.5f;
	Ability->CommitFX = UGlobalLibrary::GetAbilityUseFX(9);
	Ability->LoadIcon("/Game/Sprites/Icons/417.417");

	FDamageModifier Damage;
	Damage.AbilityOwner = Ability;
	Damage.Icon = Ability->Icon;
	Damage.Health = 100.0f;
	Damage.ModifierCritical = 15.0f;
	Ability->DamageModifiers.Add(Damage);
}
