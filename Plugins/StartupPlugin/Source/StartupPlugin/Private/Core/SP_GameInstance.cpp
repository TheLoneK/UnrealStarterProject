// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/SP_GameInstance.h"

#include "AbilitySystemGlobals.h"

USP_GameInstance::USP_GameInstance()
{
}

void USP_GameInstance::Init()
{
	Super::Init();
	// Your initialization code here

	InitAbilitySystemGlobals();

}

void USP_GameInstance::Shutdown()
{
	Super::Shutdown();
	// Your shutdown code here


}

void USP_GameInstance::InitAbilitySystemGlobals()
{
	UAbilitySystemGlobals::Get().InitGlobalData();
}