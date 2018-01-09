// Fill out your copyright notice in the Description page of Project Settings.

#include "CharacterKholl.h"

ACharacterKholl::ACharacterKholl()
{
	PlayerClass = ECharacterClass::ECCL_Warrior;

	Critical.ValueBase = 20.0f;
	Critical.Reset();

	Energy.RegenerationBase = 10.0f;
	Energy.Reset();
}

void ACharacterKholl::BeginPlay()
{
	Super::BeginPlay();
}

void ACharacterKholl::SetAbility1()
{
	Super::SetAbility1();

	AAbilityBase* Ability = AddAbility(AAbilityBase::StaticClass(), 1);
	if (!Ability) { return; }

	Ability->Name = "Sword Strike";
	Ability->Description = "A powerful attack.";
	Ability->LoadIcon("/Game/Sprites/Icons/401.401");
	Ability->MaxDistance = 300.0f;
	Ability->AbilityType = ABST_Instant;
	Ability->AreaType = ABA_Target;
	Ability->CommitFX = UGlobalLibrary::GetAbilityUseFX(1);

	FDamageModifier Damage;
	Damage.AbilityOwner = Abilities[1];
	Damage.Icon = Abilities[1]->Icon;
	Damage.Health = 130.0f;
	Damage.ModifierCritical = 30.0f;
	Damage.OnApplyHandler = [](FModifierInfo ModifierInfo) {
		AArenaCharacter* Target = Cast<AArenaCharacter>(ModifierInfo.Target);
		if (Target) { Target->MulticastPlayFX(UGlobalLibrary::GetAbilityHitFX(1)); }
	};
	Ability->DamageModifiers.Add(Damage);
}

void ACharacterKholl::SetAbility2()
{
	Super::SetAbility2();

	AAbilityBase* Ability = AddAbility(AAbilityBase::StaticClass(), 2);
	if (!Ability) { return; }

	Ability->Name = "Slash Attack";
	Ability->Description = "Do an Area on Effect damage by 100HP and have 25% chance to apply a bleed effect.";
	Ability->AbilityType = ABST_Instant;
	Ability->AreaType = ABA_Directional;
	Ability->DirectionalRadius = 100.0f;
	Ability->DirectionalRange = 200.0f;
	Ability->EnergyCost = 40.0f;
	Ability->bAllowEnemy = true;
	Ability->bAllowSelf = false;
	Ability->bAllowTeam = false;
	Ability->CommitFX = UGlobalLibrary::GetAbilityUseFX(2);
	Ability->LoadIcon("/Game/Sprites/Icons/452.452");

	FDamageModifier Damage;
	Damage.AbilityOwner = Ability;
	Damage.Icon = Ability->Icon;
	Damage.Health = 250.0f;
	Damage.OnApplyHandler = [](FModifierInfo ModifierInfo) {
		AArenaCharacter* Target = Cast<AArenaCharacter>(ModifierInfo.Target);
		if (!Target) { return; }

		Target->MulticastPlayFX(UGlobalLibrary::GetAbilityHitFX(2));

		AAbilityBase* Ability = Cast<AAbilityBase>(ModifierInfo.Ability);
		if (!Ability || !UUtilities::IsCritical(50.0f)) { return; }

		FOvertimeModifier Overtime;
		Overtime.AbilityOwner = ModifierInfo.Ability;
		Overtime.Icon = Ability->Icon;
		Overtime.Name = "Slash Attack Bleeding";
		Overtime.Description = "30 physical damage every 1 second.";
		Overtime.Health = 30.0f;
		Overtime.TickTime = 1.0f;
		Overtime.TimeRemaining = 10.0f;
		Overtime.bIsHarmful = true;
		Overtime.OnTickHandler = [](FModifierInfo ModifierInfo) {
			AArenaCharacter* Target = Cast<AArenaCharacter>(ModifierInfo.Target);
			if (Target) { Target->MulticastPlayFX(UGlobalLibrary::GetAbilityHitFX(3)); }
		};

		Target->ApplyOvertimeModifier(Overtime);
	};
	Ability->DamageModifiers.Add(Damage);
}

void ACharacterKholl::SetAbility3()
{
	Super::SetAbility3();

	AAbilityBase* Ability = AddAbility(AAbilityBase::StaticClass(), 3);
	if (!Ability) { return; }

	Ability->Name = "Push";
	Ability->Description = "Knockback the target causing 50% slow for 5 seconds.";
	Ability->CountdownTime = 15.0f;
	Ability->MaxDistance = 300.0f;
	Ability->AbilityType = ABST_Instant;
	Ability->AreaType = ABA_Target;
	Ability->EnergyCost = 35.0f;
	Ability->bAllowEnemy = true;
	Ability->bAllowSelf = false;
	Ability->bAllowTeam = false;
	Ability->LoadIcon("/Game/Sprites/Icons/228.228");

	FDamageModifier Damage;
	Damage.AbilityOwner = Ability;
	Damage.Icon = Ability->Icon;
	Damage.Health = 100.0f;
	Damage.OnApplyHandler = [](FModifierInfo ModifierInfo) {
		AArenaCharacter* Target = Cast<AArenaCharacter>(ModifierInfo.Target);
		if (Target) { Target->ServerKnockBack(ModifierInfo.Causer->GetActorLocation(), 400.0f); }
	};

	Ability->DamageModifiers.Add(Damage);

	FBuffModifier Slow;
	Slow.AbilityOwner = Ability;
	Slow.Name = "Push - Slow";
	Slow.Description = "50% Slow.";
	Slow.Icon = Ability->Icon;
	Slow.Speed = 300.0f;
	Slow.bIsHarmful = true;
	Slow.TimeRemaining = 5.0f;
	Ability->BuffModifiers.Add(Slow);
}

void ACharacterKholl::SetAbility4()
{
	Super::SetAbility4();
	
	AAbilityBase* Ability = AddAbility(AAbilityBase::StaticClass(), 4);
	if (!Ability) { return; }

	Ability->Name = "Shield Wall";
	Ability->Description = "Increase the physical defense by 50% and magic defense by 70% for 5 seconds.";
	Ability->CountdownTime = 30.0f;
	Ability->AbilityType = ABST_Instant;
	Ability->AreaType = ABA_Target;
	Ability->EnergyCost = 10.0f;
	Ability->bAllowSelf = true;
	Ability->bAllowEnemy = false;
	Ability->bAllowTeam = false;
	Ability->LoadIcon("/Game/Sprites/Icons/19.19");

	FBuffModifier Buff;
	Buff.AbilityOwner = Ability;
	Buff.Icon = Ability->Icon;
	Buff.Name = "Shield Wall";
	Buff.Description = "Magic Defense is increased by 70% and Physical Defense by 50%.";
	Buff.MagicDefense = 90.0f;
	Buff.PhysicalDefense = 70.0f;
	Buff.School = MS_Physical;
	Buff.bAllowSelf = true;
	Buff.bAllowTeam = false;
	Buff.bAllowEnemy = true;
	Buff.bIsHarmful = false;
	Buff.TimeRemaining = 5.0f;
	Buff.OnApplyHandler = [](FModifierInfo ModifierInfo) {
		AArenaCharacter* Target = Cast<AArenaCharacter>(ModifierInfo.Target);
		if (Target) { Target->MulticastPlayFX(UGlobalLibrary::GetAbilityUseFX(4)); }
	};

	Ability->BuffModifiers.Add(Buff);
}

void ACharacterKholl::SetAbility5()
{
	Super::SetAbility5();

	AAbilityBase* Ability = AddAbility(AAbilityBase::StaticClass(), 5);
	if (!Ability) { return; }
	
	Ability->Name = "Shockwave";
	Ability->Description = "Hit the target using the shield causing 100 HP damage and causing 20% of physical damage for 10 seconds.";
	Ability->MaxDistance = 300.0f;
	Ability->AbilityType = ABST_Instant;
	Ability->AreaType = ABA_Target;
	Ability->bAllowEnemy = true;
	Ability->bAllowSelf = false;
	Ability->bAllowTeam = false;
	Ability->EnergyCost = 50.0f;
	Ability->CountdownTime = 30.0f;
	Ability->CommitFX = UGlobalLibrary::GetAbilityUseFX(5);
	Ability->LoadIcon("/Game/Sprites/Icons/475.475");

	FDamageModifier Damage;
	Damage.AbilityOwner = Ability;
	Damage.Icon = Ability->Icon;
	Damage.Health = 100.0f;
	Damage.bIsHarmful = true;
	Ability->DamageModifiers.Add(Damage);

	FBuffModifier PhysicalDebuff;
	PhysicalDebuff.AbilityOwner = Ability;
	PhysicalDebuff.Icon = Ability->Icon;
	PhysicalDebuff.Name = "Shield Blow";
	PhysicalDebuff.Description = "Physical damage is increased by 20%.";
	PhysicalDebuff.PhysicalDefense = 20.0f;
	PhysicalDebuff.School = MS_Physical;
	PhysicalDebuff.bAllowSelf = false;
	PhysicalDebuff.bAllowTeam = false;
	PhysicalDebuff.bAllowEnemy = true;
	PhysicalDebuff.bIsHarmful = true;
	PhysicalDebuff.TimeRemaining = 6.0f;

	Ability->BuffModifiers.Add(PhysicalDebuff);
}

void ACharacterKholl::SetAbility6()
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
	Buff.OnApplyHandler = [](FModifierInfo ModifierInfo) {
		AArenaCharacter* Causer = Cast<AArenaCharacter>(ModifierInfo.Causer);
		if (!Causer) { return; }

		Causer->RemoveSnareBuffModifiers(Causer);
		Causer->RemoveStuckBuffModifiers(Causer);

		Causer->MulticastPlayFX(UGlobalLibrary::GetAbilityUseFX(6));
	};
	Ability->BuffModifiers.Add(Buff);
}

void ACharacterKholl::SetAbility7()
{
	Super::SetAbility7();

	AAbilityBase* Ability = AddAbility(AAbilityBase::StaticClass(), 7);
	if (!Ability) { return; }

	Ability->Name = "Tangled Chains";
	Ability->Description = "Roots the target for 4 seconds.";
	Ability->MaxDistance = 300.0f;
	Ability->AbilityType = ABST_Instant;
	Ability->AreaType = ABA_Target;
	Ability->CommitType = ABC_Projectile;
	Ability->MaxDistance = 1500.0f;
	Ability->CountdownTime = 18.0f;
	Ability->ProjectileSpeed = 3000.0f;
	Ability->LoadIcon("/Game/Sprites/Icons/418.418");
	Ability->Projectile = UGlobalLibrary::GetProjectile(1);

	FDamageModifier Damage;
	Damage.AbilityOwner = Ability;
	Damage.Icon = Ability->Icon;
	Damage.Health = 100.0f;
	Ability->DamageModifiers.Add(Damage);

	FBuffModifier Root;
	Root.AbilityOwner = Ability;
	Root.Icon = Ability->Icon;
	Root.Name = "Tangled Chains";
	Root.Description = "Incapable to move.";
	Root.State = CS_Stuck;
	Root.TimeRemaining = 4.0f;
	Root.OnApplyHandler = [](FModifierInfo ModifierInfo) {
		AArenaCharacter* Target = Cast<AArenaCharacter>(ModifierInfo.Target);
		if (Target) { Target->MulticastPlayFX(UGlobalLibrary::GetAbilityHitFX(7)); }
	};

	Ability->BuffModifiers.Add(Root);
}

void ACharacterKholl::SetAbility8()
{
	Super::SetAbility8();

	//AAbilityBase* Ability = AddAbility(AAbilityBase::StaticClass(), 8);
	//if (!Ability) { return; }
}
