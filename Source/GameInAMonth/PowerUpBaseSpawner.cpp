// Fill out your copyright notice in the Description page of Project Settings.


#include "PowerUpBaseSpawner.h"

// Sets default values
APowerUpBaseSpawner::APowerUpBaseSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APowerUpBaseSpawner::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &APowerUpBaseSpawner::SpawnActorTimer, SpawnDelay, true); // Call the function every 7.5 seconds
	
}

void APowerUpBaseSpawner::SpawnActorTimer()
{
	// If the actor is already spawned dont spawn another one until it is destroyed

	if (!SpawnedActor || SpawnedActor->IsPendingKill())
	{
		SpawnedActor = GetWorld()->SpawnActor<AActor>(ActorToSpawn, GetActorLocation(), GetActorRotation());
	}
}



