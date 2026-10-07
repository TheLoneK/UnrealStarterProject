// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/SP_CharacterMaster.h"
#include "Core/Components/SP_MovementComponent.h"
#include "GAS/SP_AbilitySystemComponent.h"
#include "GAS/SP_AttributeSet.h"
#include "AttributeSet.h"
#include "GAS/SP_GameplayAbility.h"

// Sets default values - constructor
ASP_CharacterMaster::ASP_CharacterMaster(const class FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer.SetDefaultSubobjectClass<USP_MovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Create ability system component, and set it to be explicitly replicated
	AbilitySystemComponent = CreateDefaultSubobject<USP_AbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);

	// Minimal Mode means that no GameplayEffects will replicate. They will only live on the Server. Attributes, GameplayTags, and GameplayCues will still replicate to us.
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	// Create the attribute set, this replicates by default
	// Adding it as a subobject of the owning actor of an AbilitySystemComponent
	// automatically registers the AttributeSet with the AbilitySystemComponent
	CharacterAttributeSet = CreateDefaultSubobject<UAttributeSet>(TEXT("AttributeSetBase"));


	SetReplicateMovement(true);

	SetNetUpdateFrequency(60.0f);
	bReplicates = true;
	NetPriority = 3.0f;
}

void ASP_CharacterMaster::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}

void ASP_CharacterMaster::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	if (GetAbilitySystemComponent())
	{
		TagContainer.AppendTags(GetAbilitySystemComponent()->GetOwnedGameplayTags());
		return;
	}
	return;
}


UAbilitySystemComponent* ASP_CharacterMaster::GetAbilitySystemComponent() const
{
	if (AbilitySystemComponent)
	{
		return AbilitySystemComponent;
	}
	return nullptr;
}




// Called when the game starts or when spawned
void ASP_CharacterMaster::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ASP_CharacterMaster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}



void ASP_CharacterMaster::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	AbilitySystemComponent->InitAbilityActorInfo(this, this);

}

void ASP_CharacterMaster::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

}

void ASP_CharacterMaster::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	AbilitySystemComponent->InitAbilityActorInfo(this, this);

}


void ASP_CharacterMaster::InitializeCharacterAbilities()
{

	// Grant abilities, but only on the server	
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	for (TSubclassOf<USP_GameplayAbility>& StartupAbility : CharacterAbilities)
	{
		GetAbilitySystemComponent()->GiveAbility(FGameplayAbilitySpec(StartupAbility, 1, -1, this));

	}

}

void ASP_CharacterMaster::InitializeStartupEffects(float EffectsLevel)
{
	if (GetLocalRole() != ROLE_Authority && !GetAbilitySystemComponent())
	{
		return;
	}

	FGameplayEffectContextHandle EffectContext = GetAbilitySystemComponent()->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	for (TSubclassOf<UGameplayEffect> GameplayEffect : StartupEffects)
	{
		FGameplayEffectSpecHandle NewHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(GameplayEffect, EffectsLevel, EffectContext);
		if (NewHandle.IsValid())
		{
			FActiveGameplayEffectHandle ActiveGEHandle = GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), GetAbilitySystemComponent());
		}
	}
}

#pragma region AttributeSet Management
void ASP_CharacterMaster::GrantCharacterAttributeSets(const TArray<TSubclassOf<UAttributeSet>>& NewSets)
{
	// Only the Server should spawn replicated subobjects
	if (!HasAuthority() || !IsValid(AbilitySystemComponent))
	{
		return;
	}

	for (const TSubclassOf<UAttributeSet>& SetClass : NewSets)
	{
		if (SetClass)
		{


			//Respawn check if we already have an instance of this class

			bool bAlreadyHasSet = false;
			for (UAttributeSet* ExistingSet : AbilitySystemComponent->GetSpawnedAttributes())
			{
				if (ExistingSet && ExistingSet->IsA(SetClass))
				{
					bAlreadyHasSet = true;
					break;
				}
			}

			// Only spawn if the Player State doesn't already own it
			if (!bAlreadyHasSet)
			{
				//makes the Player State the memory owner (Outer)
				UAttributeSet* NewSet = NewObject<UAttributeSet>(this, SetClass);

				// Add to ASC for standard replication
				AbilitySystemComponent->AddSpawnedAttribute(NewSet);
			}
		}
	}
}

void ASP_CharacterMaster::RemoveCharacterAttributeSets(const TArray<TSubclassOf<UAttributeSet>>& SetsToRemove)
{

	// Removal must be authoritative
	if (!HasAuthority() || !IsValid(AbilitySystemComponent))
	{
		return;
	}

	for (const TSubclassOf<UAttributeSet>& ClassToRemove : SetsToRemove)
	{
		if (!ClassToRemove) continue;

		// Iterate backwards because we are modifying an array while looping, 
		// though GAS provides a specific function for removal.
		const TArray<UAttributeSet*> SpawnedAttributes = AbilitySystemComponent->GetSpawnedAttributes();

		for (UAttributeSet* ExistingSet : SpawnedAttributes)
		{
			if (ExistingSet && ExistingSet->IsA(ClassToRemove))
			{
				//Remove from the ASC's internal array and network replication
				AbilitySystemComponent->RemoveSpawnedAttribute(ExistingSet);

				//Clear any active Gameplay Effects that are modifying this specific set's attributes
				//This is psuedo code but is a good idea in the future. Might just remove all effects?
				//AbilitySystemComponent->RemoveActiveEffectsWithGrantedTags(TagContainer) 

				//Flag the object for Garbage Collection so it is destroyed in memory
				ExistingSet->MarkAsGarbage();

				break; // Found and removed this class type, move to the next
			}
		}
	}

}

float ASP_CharacterMaster::GetHealthStat() const
{
	//return CharacterAttributeSet->GetHealthFloatValue();
	return 0.0f;
}

float ASP_CharacterMaster::GetMaxHealthStat() const
{
	//return CharacterAttributeSet->GetMaxHealthFloatValue();
	return 0.0f;
}

#pragma endregion


#pragma region Movemcomponent Implementations

void ASP_CharacterMaster::PhysNetCustom_Implementation(float DeltaTime, int32 Iterations)
{
}

USP_MovementComponent* ASP_CharacterMaster::GetCharacterMovementComponent() const
{
	USP_MovementComponent* MyCharacterMovementComponent = static_cast<USP_MovementComponent*>(GetCharacterMovement());
	return MyCharacterMovementComponent;
}

void ASP_CharacterMaster::OnMovementUpdatedCustom_Implementation(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity)
{
}


void ASP_CharacterMaster::UpdateCharacterGroundMovespeed_Implementation(float NewMoveSpeed)
{
	GetCharacterMovementComponent()->SetMaxGroundSpeed(NewMoveSpeed);
}

void ASP_CharacterMaster::UpdateCharacterMoveSpeedUsingMultiplier_Implementation(float NewMoveSpeedMultiplier)
{
	float MoveSpeedAttributeValue = 0.0f;
	float NewMaxGroundSpeed;
	bool bHasAttribute = false;

	//MoveSpeedAttributeValue = GetAbilitySystemComponent()->GetGameplayAttributeValue(USP_AttributeSet::GetMoveSpeedAttribute(), bHasAttribute);

	NewMaxGroundSpeed = MoveSpeedAttributeValue * NewMoveSpeedMultiplier;

	//universally change the max speed for all movement modes (ground, swim, fly)
	GetCharacterMovementComponent()->SetMaxGroundSpeed(NewMaxGroundSpeed);
	//GetCharacterMovementComponent()->SetMaxSwimSpeed(GetAttributeMoveSpeed() * NewMoveSpeedMultiplier);
	//GetCharacterMovementComponent()->SetMaxFlySpeed(GetAttributeMoveSpeed() * NewMoveSpeedMultiplier);
}


#pragma endregion


#pragma region SP_GASInterface Implementations

/*
void ASP_CharacterMaster::UpdateMoveSpeedMultiplierValue_Implementation(float Value)
{
	UpdateCharacterMoveSpeedUsingMultiplier(Value);
}

void ASP_CharacterMaster::UpdateCharacterGroundSpeedValue_Implementation(float Value)
{
	UpdateCharacterGroundMovespeed(Value);
}
*/
#pragma endregion