// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameplayTagContainer.h"
#include "SP_Interface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USP_Interface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class STARTUPPLUGIN_API ISP_Interface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	//used to pass a tag container
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void PassTagFromCode(FGameplayTagContainer TagToCheck);


	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void InitAttributeSetsOnASCActor(const TArray<TSubclassOf<UAttributeSet>>& NewSets);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	UAttributeSet* GetBaseAttributeSetFromASCActor() const;


	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void UpdateMoveSpeedMultiplierValue(float Value);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void UpdateCharacterGroundSpeedValue(float Value);


	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnDamaged(float DamageAmount, const FGameplayTagContainer& GameplayTags, AActor* SourceActor, AActor* TargetActor);
};
