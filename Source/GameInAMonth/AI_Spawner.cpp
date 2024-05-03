// Fill out your copyright notice in the Description page of Project Settings.


#include "AI_Spawner.h"
#include "EnemyAIController.h"
#include "TrainingDummy.h"

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
		UE_LOG(LogTemp, Warning, TEXT("Updating Values"));
		CurrentEnemy = SpawnedEnemy; // Set the current enemy

		SpawnedEnemy->Spawner = this; // Set the spawner for the enemy so we can call the OnEnemyDeath function

		CurrentEnemy->Health = BaseHealth; // Set the health for the enemy
		CurrentEnemy->MaxHealth = BaseHealth; // Set the max health for the enemy
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

void AAI_Spawner::OnEnemyDeath()
{
	// Set a timer to spawn the next enemy, this function will be called in either the gamemode or the enemy class when the enemy dies, Note to self (probably enemy class would be better )
	GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &AAI_Spawner::SpawnEnemy, SpawnDelay, false);
}

