// Fill out your copyright notice in the Description page of Project Settings.

#include "Global.h"

UGlobal::UGlobal(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Sprites
	SpritePath = FString("/Game/Sprites/Icons/");
	TotalIcons = 488;
	TotalBadges = 39;
	LoadSprites(SpritePath);

	LoadAbilities();
}

void UGlobal::LoadSprites(FString& BasePath)
{
	LoadIcons(BasePath);
}

void UGlobal::LoadIcons(FString& BasePath)
{
	for (int32 i = 0; i < TotalIcons; i++)
	{
		FString Path = FString(BasePath);
		Path.AppendInt(i);
		Path.Append(".");
		Path.AppendInt(i);

		UTexture2D* Texture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, *(Path)));

		if (Texture != NULL)
		{
			Icons.Add(i, Texture);
		}
	}
}

void UGlobal::LoadAbilities()
{
	Abilities.Add(1, AAbility1::StaticClass());
	Abilities.Add(2, AAbility2::StaticClass());
	Abilities.Add(3, AAbility3::StaticClass());
	Abilities.Add(4, AAbility4::StaticClass());
	Abilities.Add(5, AAbility5::StaticClass());
}
