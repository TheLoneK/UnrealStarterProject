// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/SP_AttributeSet.h"
#include "GameplayEffect.h"
#include "Core/SP_Interface.h"
#include "GameFramework/Character.h"
#include "GameplayEffectExtension.h"
#include "AbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"

USP_AttributeSet::USP_AttributeSet()
	: MaxHealth(100.f),
	Health(100.f),
	MoveSpeed(600.f),
	MoveSpeedMultiplier(1.f)
{
}

void USP_AttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	// This is called whenever attributes change, so for max health/mana we want to scale the current totals to match
	Super::PreAttributeChange(Attribute, NewValue);

	// If a Max value changes, adjust current to keep Current % of Current to Max
	if (Attribute == GetMaxHealthAttribute()) // GetMaxHealthAttribute comes from the Macros defined at the top of the header
	{
		AdjustAttributeForMaxChange(Health, MaxHealth, NewValue, GetHealthAttribute());
	}
	else if (Attribute == GetMoveSpeedAttribute())
	{
		// MoveSpeed base cannot slow less than 150 units/s and cannot boost more than 1200 units/s
		NewValue = FMath::Clamp<float>(NewValue, 150, 1200);
	}
}

void USP_AttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeBaseChange(Attribute, OldValue, NewValue);

	if (!GetOwningActor()->HasAuthority())
	{
		return;
	}

	if (Attribute == GetMoveSpeedMultiplierAttribute())
	{
		UAbilitySystemComponent* AbilityComp = GetOwningAbilitySystemComponent();

		if (AbilityComp)
		{
			if (AbilityComp->GetAvatarActor())
			{
				AActor* OwningAvatar = AbilityComp->GetAvatarActor();

				if (OwningAvatar && OwningAvatar->Implements<USP_Interface>())
				{
					ISP_Interface::Execute_UpdateMoveSpeedMultiplierValue(OwningAvatar, NewValue);
				}

			}
		}
	}
}
void USP_AttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{

	Super::PostGameplayEffectExecute(Data);

	FGameplayEffectContextHandle Context = Data.EffectSpec.GetContext();
	UAbilitySystemComponent* Source = Context.GetOriginalInstigatorAbilitySystemComponent();
	const FGameplayTagContainer& SourceTags = *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();
	FGameplayTagContainer SpecAssetTags;
	Data.EffectSpec.GetAllAssetTags(SpecAssetTags);

	// Get the Target actor, which should be our owner
	AActor* TargetActor = nullptr;
	AController* TargetController = nullptr;
	ACharacter* TargetCharacter = nullptr;

	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		TargetActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		TargetCharacter = Cast<ACharacter>(TargetActor);
	}

	// Get the Source actor
	AActor* SourceActor = nullptr;
	AController* SourceController = nullptr;
	ACharacter* SourceCharacter = nullptr;


	if (Source && Source->AbilityActorInfo.IsValid() && Source->AbilityActorInfo->AvatarActor.IsValid())
	{
		SourceActor = Source->AbilityActorInfo->AvatarActor.Get();
		SourceController = Source->AbilityActorInfo->PlayerController.Get();
		if (SourceController == nullptr && SourceActor != nullptr)
		{
			if (APawn* Pawn = Cast<APawn>(SourceActor))
			{
				SourceController = Pawn->GetController();
			}
		}

		// Use the controller to find the source pawn
		if (SourceController)
		{
			SourceCharacter = Cast<ACharacter>(SourceController->GetPawn());
		}
		else
		{
			SourceCharacter = Cast<ACharacter>(SourceActor);
		}

		// Set the causer actor based on context if it's set
		if (Context.GetEffectCauser())
		{
			SourceActor = Context.GetEffectCauser();
		}
	}

	if (Data.EvaluatedData.Attribute == GetDamageTakenAttribute())
	{
		// Try to extract a hit result
		FHitResult HitResult;
		if (Context.GetHitResult())
		{
			HitResult = *Context.GetHitResult();
		}

		// Store a local copy of the amount of damage done and clear the damage attribute
		const float LocalDamageDone = GetDamageTaken();
		SetDamageTaken(0.f);

		if (LocalDamageDone > 0.0f)
		{

			// Apply the health change and then clamp it
			const float NewHealth = GetHealth() - LocalDamageDone;
			SetHealth(FMath::Clamp(NewHealth, 0.0f, GetMaxHealth()));

		}
	}
	else if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		// Handle other health changes.
		// Health loss should go through Damage.
		SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));
	}

	else if (Data.EvaluatedData.Attribute == GetMoveSpeedAttribute())
	{
		// Clamp MoveSpeed
		SetMoveSpeed(FMath::Clamp(GetMoveSpeed(), 150.0f, 1000.0f));

		if (TargetCharacter && TargetCharacter->Implements<USP_Interface>())
		{
			ISP_Interface::Execute_UpdateCharacterGroundSpeedValue(TargetCharacter, GetMoveSpeed());
		}

	}

}

void USP_AttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(USP_AttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USP_AttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USP_AttributeSet, BaseDamage, COND_None, REPNOTIFY_Always);
}

float USP_AttributeSet::GetHealthFloatValue() const
{
	return GetHealthAttribute().GetGameplayAttributeData(this)->GetCurrentValue();
}

float USP_AttributeSet::GetMaxHealthFloatValue() const
{
	return GetMaxHealthAttribute().GetGameplayAttributeData(this)->GetCurrentValue();
}

void USP_AttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USP_AttributeSet, MaxHealth, OldMaxHealth);
}

void USP_AttributeSet::OnRep_MoveSpeedMultiplier(const FGameplayAttributeData& OldMoveSpeedMultiplier)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USP_AttributeSet, MoveSpeedMultiplier, OldMoveSpeedMultiplier);
}

void USP_AttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USP_AttributeSet, Health, OldHealth);
}

void USP_AttributeSet::OnRep_MoveSpeed(const FGameplayAttributeData& OldMoveSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USP_AttributeSet, MoveSpeed, OldMoveSpeed);
}

void USP_AttributeSet::OnRep_DamageTaken(const FGameplayAttributeData& OldDamageTaken)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USP_AttributeSet, DamageTaken, OldDamageTaken);
}

void USP_AttributeSet::OnRep_BaseDamage(const FGameplayAttributeData& OldBaseDamage)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USP_AttributeSet, BaseDamage, OldBaseDamage);
}

void USP_AttributeSet::AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty)
{
	UAbilitySystemComponent* AbilityComp = GetOwningAbilitySystemComponent();
	const float CurrentMaxValue = MaxAttribute.GetCurrentValue();
	if (!FMath::IsNearlyEqual(CurrentMaxValue, NewMaxValue) && AbilityComp)
	{
		// Change current value to maintain the current Val / Max percent
		const float CurrentValue = AffectedAttribute.GetCurrentValue();
		float NewDelta = (CurrentMaxValue > 0.f) ? (CurrentValue * NewMaxValue / CurrentMaxValue) - CurrentValue : NewMaxValue;

		AbilityComp->ApplyModToAttributeUnsafe(AffectedAttributeProperty, EGameplayModOp::Additive, NewDelta);
	}
}