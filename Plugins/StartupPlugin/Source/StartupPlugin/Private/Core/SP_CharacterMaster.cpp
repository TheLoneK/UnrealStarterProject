// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/SP_CharacterMaster.h"

// Sets default values
ASP_CharacterMaster::ASP_CharacterMaster()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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

// Called to bind functionality to input
void ASP_CharacterMaster::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

