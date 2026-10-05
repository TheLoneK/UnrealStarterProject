// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/SP_Actor.h"

// Sets default values
ASP_Actor::ASP_Actor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASP_Actor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASP_Actor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

