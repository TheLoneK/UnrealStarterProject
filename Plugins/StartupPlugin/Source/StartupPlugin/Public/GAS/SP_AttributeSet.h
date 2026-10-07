// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "SP_AttributeSet.generated.h"

/**
 * 
 */

	
// you don't forward declare the ASC because the attribute accessors need the actual class include to work
// 
// UAbilitySystempComponent;

// Uses macros from AttributeSet.h
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)


UCLASS()
class STARTUPPLUGIN_API USP_AttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:

	USP_AttributeSet();


	// AttributeSet Overrides
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	float GetHealthFloatValue() const;

	float GetMaxHealthFloatValue() const;


	// MaxHealth is its own attribute since GameplayEffects may modify it
	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(USP_AttributeSet, MaxHealth)

		// Current Health, when 0 we expect owner to die unless prevented by an ability. Capped by MaxHealth.
		// Positive changes can directly use this.
		// Negative changes to Health should go through Damage meta attribute.
		UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(USP_AttributeSet, Health)

		// DamageTaken attribute is hidden from editor effects and only calculated in C++ using stats and other modifiers.
		// Negative changes to Health should go through DamageTaken meta attribute.
		UPROPERTY(BlueprintReadOnly, Category = "Damage", ReplicatedUsing = OnRep_DamageTaken, Meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData DamageTaken;
	ATTRIBUTE_ACCESSORS(USP_AttributeSet, DamageTaken)

		UPROPERTY(BlueprintReadOnly, Category = "Damage", ReplicatedUsing = OnRep_BaseDamage)
	FGameplayAttributeData BaseDamage;
	ATTRIBUTE_ACCESSORS(USP_AttributeSet, BaseDamage)

		// MoveSpeed affects how fast characters can move.
		UPROPERTY(BlueprintReadOnly, Category = "MoveSpeed", ReplicatedUsing = OnRep_MoveSpeed)
	FGameplayAttributeData MoveSpeed;
	ATTRIBUTE_ACCESSORS(USP_AttributeSet, MoveSpeed)

		//Move speed multiplier such as for sprinting
		UPROPERTY(BlueprintReadOnly, Category = "MoveSpeed", ReplicatedUsing = OnRep_MoveSpeedMultiplier)
	FGameplayAttributeData MoveSpeedMultiplier;
	ATTRIBUTE_ACCESSORS(USP_AttributeSet, MoveSpeedMultiplier)



protected:
	// Helper function to proportionally adjust the value of an attribute when it's associated max attribute changes.
	// (i.e. When MaxHealth increases, Health increases by an amount that maintains the same percentage as before)
	void AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty);


	/**
* These OnRep functions exist to make sure that the ability system internal representations are synchronized properly during replication
**/

	UFUNCTION()
	virtual void OnRep_Health(const FGameplayAttributeData& OldHealth);

	UFUNCTION()
	virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);

	UFUNCTION()
	virtual void OnRep_MoveSpeedMultiplier(const FGameplayAttributeData& OldMoveSpeedMultiplier);

	UFUNCTION()
	virtual void OnRep_MoveSpeed(const FGameplayAttributeData& OldMoveSpeed);

	UFUNCTION()
	virtual void OnRep_DamageTaken(const FGameplayAttributeData& OldDamageTaken);

	UFUNCTION()
	virtual void OnRep_BaseDamage(const FGameplayAttributeData& OldBaseDamage);


};
