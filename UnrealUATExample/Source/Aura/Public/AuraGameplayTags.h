// Copyright 2025, Daniel Gerardo Orozco Hernandez

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

/*
 * Tags to be registered
 */
#define AURA_GAMEPLAY_TAGS_ATTRIBUTES  \
X(Attributes_Primary_Strength , "Increases physical damage") \
X(Attributes_Primary_Intelligence , "Increases magical damage") \
X(Attributes_Primary_Resilience , "Increases Armor and Armor Penetration") \
X(Attributes_Primary_Vigor , "Increases Health") \
X(Attributes_Secondary_Armor , "Reduces damage taken, improves Block chance") \
X(Attributes_Secondary_ArmorPenetration , "Ignores percentage of enemy Armor, increases Crit Hit Chance") \
X(Attributes_Secondary_BlockChance , "Chance to cut damage in half") \
X(Attributes_Secondary_CriticalHitChance , "Chance to double damage plus critical hit bonus") \
X(Attributes_Secondary_CriticalHitDamage , "Bonus damage added when a critical hit is scored") \
X(Attributes_Secondary_CriticalHitResistance , "Reduces critical hit chance of attacking enemies") \
X(Attributes_Secondary_HealthRegeneration , "Amount of Health regenerated every 1 second") \
X(Attributes_Secondary_ManaRegeneration , "Amount of Mana regenerated every 1 second") \
X(Attributes_Secondary_MaxHealth , "Max amount of health you can have") \
X(Attributes_Secondary_MaxMana , "Max amount of mana") \
X(Attributes_Vital_Health , "How much health you currently have") \
X(Attributes_Vital_Mana , "How much mana you currently have") \
X(InputTag_LMB , "Input tag for left mouse button") \
X(InputTag_RMB ,"Input tag for right mouse button") \
X(InputTag_1 , "Input tag for 1 key") \
X(InputTag_2 , "Input tag for 2 key") \
X(InputTag_3 , "Input tag for 3 key") \
X(InputTag_4 , "Input tag for 4 key")

#define AURA_GAMEPLAY_TAGS_RESISTANCES \
X(Attributes_Resistance_FireResistance, "Resistance to fire Attacks") \
X(Attributes_Resistance_LightningResistance, "Resistance to lightning Attacks") \
X(Attributes_Resistance_ArcaneResistance, "Resistance to arcane Attacks") \
X(Attributes_Resistance_PhysicalResistance, "Resistance to physical Attacks")

// X(Attributes_Meta_IncomingDamage, "Partial calculation of damage to be applied")
#define AURA_GAMEPLAY_TAGS_DAMAGE \
X(Damage , "Damage") \
X(Damage_Fire , "Fire Damage") \
X(Damage_Lightning , "Lightning Damage") \
X(Damage_Arcane, "Arcane Damage") \
X(Damage_Physical, "Physical Damage")

#define AURA_GAMEPLAY_TAGS_EFFECTS \
X(Effects_HitReact , "Tag given when reacting to a hit of some sort")

/*
 * Relationships between tags
 */
#define AURA_GAMEPLAY_TAGS_RESISTANCE_MAPPING \
X(Fire) \
X(Lightning) \
X(Arcane) \
X(Physical)


/**
 * AuraGameplayTags
 *
 * Singleton containing native GameplayTags
 */
struct FAuraGameplayTags
{
public:
	static const FAuraGameplayTags& Get()
	{
		return GameplayTags;
	}

	static void InitializeNativeGameplayTags();

#define X(Tag, Description) FGameplayTag Tag;
	AURA_GAMEPLAY_TAGS_ATTRIBUTES

	AURA_GAMEPLAY_TAGS_DAMAGE

	AURA_GAMEPLAY_TAGS_RESISTANCES

	AURA_GAMEPLAY_TAGS_EFFECTS
#undef X

	TMap<FGameplayTag, FGameplayTag> DamageTypes;

protected:
private:
	static FAuraGameplayTags GameplayTags;
};
