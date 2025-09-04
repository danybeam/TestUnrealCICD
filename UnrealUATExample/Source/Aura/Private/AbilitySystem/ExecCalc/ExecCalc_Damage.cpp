// Copyright 2025, Daniel Gerardo Orozco Hernandez


#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"

#include <numeric>
#include <execution>

#include "AbilitySystemComponent.h"
#include "AuraAbilityTypes.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Interaction/CombatInterface.h"

#define CAPTURED_ATTRIBUTES \
	X(Armor, Secondary, Target, false) \
	X(ArmorPenetration, Secondary, Source, false) \
	X(BlockChance, Secondary, Target, false) \
	X(CriticalHitChance, Secondary, Source, false) \
	X(CriticalHitDamage, Secondary, Source, false) \
	X(CriticalHitResistance, Secondary, Target, false) \
	X(FireResistance, Resistance, Target, false) \
	X(LightningResistance, Resistance, Target, false) \
	X(ArcaneResistance, Resistance, Target, false) \
	X(PhysicalResistance, Resistance, Target, false )

struct AuraDamageStatics
{
#define X(Name, Category, Source, Snapshot) DECLARE_ATTRIBUTE_CAPTUREDEF(Name);
	CAPTURED_ATTRIBUTES

#undef X

	TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition> TagsToCaptureDefs;

	AuraDamageStatics()
	{
#define X(Name, Category, Source, Snapshot) DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Name, Source, Snapshot);
		CAPTURED_ATTRIBUTES
#undef X

#define X(Name, Category, Source, Snapshot) TagsToCaptureDefs.Add(FAuraGameplayTags::Get().Attributes_##Category##_##Name, Name##Def);
		CAPTURED_ATTRIBUTES
#undef X
	}
};

static const AuraDamageStatics& DamageStatics()
{
	static AuraDamageStatics DamageStatics;
	return DamageStatics;
}


UExecCalc_Damage::UExecCalc_Damage()
{
#define X(Name, Category, Source, Snapshot) RelevantAttributesToCapture.Add(DamageStatics().Name##Def);
	CAPTURED_ATTRIBUTES
#undef X
}

void UExecCalc_Damage::Execute_Implementation(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput
) const
{
	// Get relevant data used by this calculation
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;
	ICombatInterface* SourceCombatInterface = Cast<ICombatInterface>(SourceAvatar);
	ICombatInterface* TargetCombatInterface = Cast<ICombatInterface>(TargetAvatar);

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParams;
	EvaluationParams.SourceTags = SourceTags;
	EvaluationParams.TargetTags = TargetTags;
	FGameplayEffectContextHandle EffectContextHandle = Spec.GetContext();


	// Get base Damage Set by Caller Magnitude
	float Damage = 0;
	for (const auto& Pair : FAuraGameplayTags::Get().DamageTypes)
	{
		const FGameplayTag DamageType = Pair.Key;
		const FGameplayTag ResistanceTag = Pair.Value;
		checkf(AuraDamageStatics().TagsToCaptureDefs.Contains(ResistanceTag),
		       TEXT("TagsToCaptureDefs doesn't contain Tag: [%s] in ExecCalc_Damage"), *ResistanceTag.ToString());
		const FGameplayEffectAttributeCaptureDefinition CaptureDef = AuraDamageStatics().TagsToCaptureDefs[
			ResistanceTag];

		float DamageTypeValue = Spec.GetSetByCallerMagnitude(Pair.Key);

		float Resistance = 0.f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(CaptureDef, EvaluationParams, Resistance);
		Resistance = FMath::Clamp(Resistance, 0.f, 100.f);

		DamageTypeValue *= (100.f - Resistance) / 100.f;
		
		Damage += DamageTypeValue;
	}

	// Capture BlockChance on Target
	float TargetBlockChance = 0;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		DamageStatics().BlockChanceDef,
		EvaluationParams,
		TargetBlockChance
	);
	TargetBlockChance = FMath::Max(TargetBlockChance, 0);

	// Determine if there was a successful block and save it in the effect context
	const bool bBlocked = FMath::RandRange(1.f, 100.f) < TargetBlockChance;
	UAuraAbilitySystemLibrary::SetIsBlockedHit(EffectContextHandle, bBlocked);

	// If Block, halve the damage
	Damage = bBlocked ? Damage / 2.f : Damage;


	// Get Armor related data
	float TargetArmor = 0;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		DamageStatics().ArmorDef,
		EvaluationParams,
		TargetArmor
	);
	TargetArmor = FMath::Max(TargetArmor, 0);

	float SourceArmorPenetration = 0;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		DamageStatics().ArmorPenetrationDef,
		EvaluationParams,
		SourceArmorPenetration
	);
	SourceArmorPenetration = FMath::Max(SourceArmorPenetration, 0);


	// Get armor penetration coefficient
	UCharacterClassInfo* CharacterClassInfo = UAuraAbilitySystemLibrary::GetCharacterClassInfo(SourceAvatar);
	FRealCurve* ArmorPenetrationCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(
		FName("ArmorPenetration"), FString());
	const float ArmorPenetrationCoefficient = ArmorPenetrationCurve->Eval(SourceCombatInterface->GetPlayerLevel());

	// Calculate effective Armor; Armor penetration ignores a percentage of Target's Armor
	const float EffectiveArmor = TargetArmor * (100 - SourceArmorPenetration * ArmorPenetrationCoefficient) / 100.f;

	// Get Armor coefficient to determine how much damage is reduced
	FRealCurve* EffectiveArmorCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(
		FName("EffectiveArmor"), FString());
	const float EffectiveArmorCoefficient = EffectiveArmorCurve->Eval(TargetCombatInterface->GetPlayerLevel());

	// Armor ignores a percentage of incoming damage; calculate effective damage after armor
	Damage *= (100 - EffectiveArmor * EffectiveArmorCoefficient) / 100.f;


	// Get Critical hit related data
	float SourceCriticalHitChance = 0;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		DamageStatics().CriticalHitChanceDef,
		EvaluationParams,
		SourceCriticalHitChance
	);
	SourceCriticalHitChance = FMath::Max(SourceCriticalHitChance, 0);

	float TargetCriticalHitResistance = 0;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		DamageStatics().CriticalHitChanceDef,
		EvaluationParams,
		TargetCriticalHitResistance
	);
	TargetCriticalHitResistance = FMath::Max(TargetCriticalHitResistance, 0);

	float SourceCriticalHitDamage = 0;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		DamageStatics().CriticalHitChanceDef,
		EvaluationParams,
		SourceCriticalHitDamage
	);
	SourceCriticalHitDamage = FMath::Max(SourceCriticalHitDamage, 0);


	// Get Critical hit resistance coefficient to dampen it 
	FRealCurve* CriticalHitResistanceCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(
		FName("CriticalHitResistance"), FString());
	const float CriticalHitResistanceCoefficient = CriticalHitResistanceCurve->Eval(
		SourceCombatInterface->GetPlayerLevel());

	const float EffectiveCriticalHitChance = SourceCriticalHitChance - TargetCriticalHitResistance *
		CriticalHitResistanceCoefficient;

	// Critical hit check
	const bool bCriticalHit = FMath::RandRange(1, 100) < EffectiveCriticalHitChance;
	UAuraAbilitySystemLibrary::SetIsCriticalHit(EffectContextHandle, bCriticalHit);

	// If critical hit double damage first, then add bonus damage
	Damage = bCriticalHit ? 2.f * Damage + SourceCriticalHitDamage : Damage;


	// Create modifier to IncomingDamageAttribute
	FGameplayModifierEvaluatedData EvaluatedData(
		UAuraAttributeSet::GetIncomingDamageAttribute(),
		EGameplayModOp::AddBase,
		Damage
	);

	// Add modifier to the output
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}
