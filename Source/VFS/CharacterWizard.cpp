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
	Ability->Description = "150HP and 50% chance to add fire dot.";
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
		if (!UUtilities::IsCritical(50.0f)) { return; }

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
	Ability->CommitFX = UGlobalLibrary::GetAbilityUseFX(2);
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

	Ability->Name = "Warrior's will";
	Ability->Description = "Remove all snare effect for 3 seconds and increase the movement speed by 50%.";
	Ability->LoadIcon("/Game/Sprites/Icons/305.305");
	Ability->MaxDistance = 300.0f;
	Ability->AbilityType = ABST_Instant;
	Ability->AreaType = ABA_Target;
	Ability->CountdownTime = 25.0f;
	Ability->EnergyCost = 15.0f;
	Ability->bAllowSelf = true;
	Ability->bAllowEnemy = false;
	Ability->bAllowTeam = false;
	Ability->CommitFX = UGlobalLibrary::GetAbilityUseFX(5);

	FBuffModifier Buff;
	Buff.AbilityOwner = Ability;
	Buff.Icon = Ability->Icon;
	Buff.Name = "Warrior's will";
	Buff.Description = "Increase the movement speed by 50%.";
	Buff.Speed = 300.0f;
	Buff.School = MS_Physical;
	Buff.bAllowSelf = true;
	Buff.bAllowTeam = false;
	Buff.bAllowEnemy = false;
	Buff.bIsHarmful = false;
	Buff.TimeRemaining = 3.0f;
	Buff.OnApplyHandler = [](FAbilityInfo AbilityInfo) {
		AArenaCharacter* Causer = Cast<AArenaCharacter>(AbilityInfo.Causer);
		if (!Causer) { return; }

		Causer->MulticastPlayFX(UGlobalLibrary::GetAbilityUseFX(4));
		Causer->RemoveSnareBuffModifiers(Causer);
		Causer->RemoveStuckBuffModifiers(Causer);
	};
	Ability->BuffModifiers.Add(Buff);
}

void ACharacterWizard::SetAbility7()
{
	Super::SetAbility7();

	AAbilityBase* Ability = AddAbility(AAbilityBase::StaticClass(), 7);
	if (!Ability) { return; }

	Ability->Name = "Bola";
	Ability->Description = "Roots the target for 5 seconds.";
	Ability->MaxDistance = 300.0f;
	Ability->AbilityType = ABST_Instant;
	Ability->AreaType = ABA_Target;
	Ability->CommitType = ABC_Projectile;
	Ability->MaxDistance = 1500.0f;
	Ability->MinDistance = 600.0f;
	Ability->CountdownTime = 10.0f;
	Ability->ProjectileSpeed = 3000.0f;
	Ability->LoadIcon("/Game/Sprites/Icons/475.475");
	Ability->Projectile = UGlobalLibrary::GetProjectile(1);

	FDamageModifier Damage;
	Damage.AbilityOwner = Ability;
	Damage.Icon = Ability->Icon;
	Damage.Health = 100.0f;
	Ability->DamageModifiers.Add(Damage);

	FBuffModifier Root;
	Root.AbilityOwner = Ability;
	Root.Icon = Ability->Icon;
	Root.Name = "Bola - Root";
	Root.Description = "Incapable to move.";
	Root.State = CS_Stuck;
	Root.TimeRemaining = 5.0f;
	Root.OnApplyHandler = [](FAbilityInfo AbilityInfo) {
		AbilityInfo.Target->MulticastPlayFX(UGlobalLibrary::GetAbilityHitFX(7));
	};

	Ability->BuffModifiers.Add(Root);
}

void ACharacterWizard::SetAbility8()
{
	Super::SetAbility8();

	//AAbilityBase* Ability = AddAbility(AAbilityBase::StaticClass(), 8);
	//if (!Ability) { return; }
}
