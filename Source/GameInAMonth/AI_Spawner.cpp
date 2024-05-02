// Fill out your copyright notice in the Description page of Project Settings.


#include "AI_Spawner.h"
#include "EnemyAIController.h"

// Sets default values
AAI_Spawner::AAI_Spawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AAI_Spawner::BeginPlay()
{
	Super::BeginPlay();

	SpawnEnemy(); // Spawn the first enemy
	
}

// Called every frame
void AAI_Spawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAI_Spawner::SpawnEnemy()
{

	/// Check if there are any enemies to spawn
	if (EnemiesToSpawn.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No enemies to spawn"));
		return;
	}
 

	// if we have an enemy already spawned or the enemy's health is greater than 0 dont spawn
	if ((CurrentEnemy && CurrentEnemy->IsValidLowLevelFast()) && CurrentEnemy->Health > 0)
	{
		return;
	}

	// Get a random index
	int RandomIndex = FMath::RandRange(0, EnemiesToSpawn.Num() - 1);
	TSubclassOf<ATrainingDummy> SelectedEnemy = EnemiesToSpawn[RandomIndex];

	FVector SpawnLocation = GetActorLocation();
	FRotator SpawnRotation = FRotator::ZeroRotator;

	// Spawn the enemy
	ATrainingDummy* SpawnedEnemy = GetWorld()->SpawnActor<ATrainingDummy>(SelectedEnemy, SpawnLocation, SpawnRotation);

	if (SpawnedEnemy)
	{
		CurrentEnemy = SpawnedEnemy; // Set the current enemy

		CurrentEnemy->MaxHealth = BaseHealth; // Set the health for the enemy
		BaseHealth += HealthIncrement; // Increase the health for the next enemy

		
		AEnemyAIController* AIController = Cast<AEnemyAIController>(CurrentEnemy->GetController());
		if (AIController)
		{
			AIController->Damage = BaseDamage; // Set the damage for the enemy
			BaseDamage += DamageIncrement; // Increase the damage for the next enemy
		}
		
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to spawn enemy"));
	}


}

