// Fill out your copyright notice in the Description page of Project Settings.

#include "Global.h"

UGlobal::UGlobal(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Sprites
	SpritePath = FString("/Game/Sprites/Icons/");
	TotalIcons = 488;
	TotalBadges = 39;
	LoadSprites(SpritePath);
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

