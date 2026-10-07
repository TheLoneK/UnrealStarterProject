// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "SP_GameInstance.generated.h"

/**
 * 
 */
UCLASS()
class STARTUPPLUGIN_API USP_GameInstance : public UGameInstance
{
	GENERATED_BODY()

	// Constructor
	USP_GameInstance();



	//Initialization when the instance is created
	virtual void Init() override;

	//Called when the instance is shutting down
	virtual void Shutdown() override;


	UFUNCTION(BlueprintCallable)
	void InitAbilitySystemGlobals();
	
};
