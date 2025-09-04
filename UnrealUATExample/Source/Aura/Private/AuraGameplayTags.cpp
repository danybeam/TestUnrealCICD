// Copyright 2025, Daniel Gerardo Orozco Hernandez


#include "AuraGameplayTags.h"
#include "GameplayTagsManager.h"

FAuraGameplayTags FAuraGameplayTags::GameplayTags;

void FAuraGameplayTags::InitializeNativeGameplayTags()
{
	/*
	 * Register tags in C++
	 */
#define X(Tag, Description)	\
	FString TempTag_##Tag = FString(#Tag);\
	TempTag_##Tag.ReplaceCharInline('_','.');\
	GameplayTags.Tag = UGameplayTagsManager::Get().AddNativeGameplayTag(FName( TempTag_##Tag ), FString( Description ));

	AURA_GAMEPLAY_TAGS_ATTRIBUTES
	AURA_GAMEPLAY_TAGS_DAMAGE
	AURA_GAMEPLAY_TAGS_RESISTANCES
	AURA_GAMEPLAY_TAGS_EFFECTS
#undef X

	/*
	 * Maps of Damage Types to Resitances
	 */
#define X(DamageType) GameplayTags.DamageTypes.Add(GameplayTags.Damage_##DamageType, GameplayTags.Attributes_Resistance_##DamageType##Resistance);
	AURA_GAMEPLAY_TAGS_RESISTANCE_MAPPING
#undef X
}
