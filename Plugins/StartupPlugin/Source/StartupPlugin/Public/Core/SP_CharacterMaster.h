// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySysteminterface.h"
#include "GameplayTagAssetInterface.h"
#include "GameFramework/Character.h"
#include "SP_CharacterMaster.generated.h"


class USP_MovementComponent;
class UAttributeSet;



//this class should not be used directly but I'm allowing it, Use SP_PlayerCharacter or SP_AICharacter instead. If extending this, be sure to implement the AbilitySystemComponent and Avatar correctly on the children.
UCLASS(Blueprintable, BlueprintType)
class STARTUPPLUGIN_API ASP_CharacterMaster : public ACharacter, public IAbilitySystemInterface, public IGameplayTagAssetInterface
{
	GENERATED_BODY()

public:


	// Sets default values for this character's properties
	ASP_CharacterMaster(const class FObjectInitializer& ObjectInitializer);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//Implement GameplayTagAssetInterface, this will get this characters AbilitySystemComponent and return OwnedTags from that.
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;


	// Implement IAbilitySystemInterface
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	//SP_GASInterface implementations
	//virtual void UpdateMoveSpeedMultiplierValue_Implementation(float Value) override;

	//virtual void UpdateCharacterGroundSpeedValue_Implementation(float Value) override;


	//virtual void OnDamaged_Implementation(float Damage, const FGameplayTagContainer& GameplayTags, AActor* SourceActor, AActor* TargetActor) override;

	//movement component implementations
	UFUNCTION(BlueprintNativeEvent)
	void PhysNetCustom(float DeltaTime, int32 Iterations);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Movement Component")
	USP_MovementComponent* GetCharacterMovementComponent() const;

	UFUNCTION(BlueprintNativeEvent)
	void OnMovementUpdatedCustom(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity);


	UFUNCTION(Client, Reliable)
	void UpdateCharacterGroundMovespeed(float NewMoveSpeed);

	UFUNCTION(Client, Reliable)
	void UpdateCharacterMoveSpeedUsingMultiplier(float NewMoveSpeedMultiplier);



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

	// Only called on the Server. Calls before Server's AcknowledgePossession.
	virtual void PossessedBy(AController* NewController) override;

	virtual void OnRep_PlayerState() override;

	//TWeakObjectPtr<class UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<USP_AbilitySystemComponent> AbilitySystemComponent;

	//the default stat class for the character
	UPROPERTY()
	TObjectPtr<UAttributeSet> CharacterAttributeSet;

	// Default abilities for this Character.
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TArray<TSubclassOf<class USP_GameplayAbility>> CharacterAbilities;

	// These effects are applied using OnPossess
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TArray<TSubclassOf<class UGameplayEffect>> StartupEffects;



	//Attribute Sets array, set in blueprint, in this plugin there is always a SP_BaseAttributes set character
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TArray<TSubclassOf<UAttributeSet>> AdditionalAttributeSetsToAdd;




	// Grant abilities on the Server. The Ability Specs will be replicated to the owning client.
	virtual void InitializeCharacterAbilities();

	virtual void InitializeStartupEffects(float EffectsLevel);

	void GrantCharacterAttributeSets(const TArray<TSubclassOf<UAttributeSet>>& NewSets);

	void RemoveCharacterAttributeSets(const TArray<TSubclassOf<UAttributeSet>>& SetsToRemove);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;


public:

	//attribute accessors
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetHealthStat() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetMaxHealthStat() const;



};
