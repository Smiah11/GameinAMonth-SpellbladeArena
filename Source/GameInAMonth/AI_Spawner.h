// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AI_Spawner.generated.h"

class ATrainingDummy; // forward declaration to avoid circular dependency (error came up prior to adding this)

UCLASS()
class GAMEINAMONTH_API AAI_Spawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAI_Spawner();

	
	UPROPERTY(EditDefaultsOnly, Category = "Spawning")
	TArray<TSubclassOf<ATrainingDummy>> EnemiesToSpawn; //array to hold the different enemies that can be spawned

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Spawning")
	float BaseDamage = 10.f; // base damage of the enemy weapon

	UPROPERTY(EditAnywhere, Category = "Spawning")
	float BaseHealth = 200.f; // base health of the enemy

	UPROPERTY(EditAnywhere, Category = "Spawning")
	ATrainingDummy* CurrentEnemy;

	UPROPERTY(EditAnywhere, Category = "Spawning")
	float SpawnDelay = 5.f; // 5 seconds

	UPROPERTY(EditAnywhere, Category = "Spawning")
	float HealthIncrement = 75.f; // how much health to add to the enemy

	UPROPERTY(EditAnywhere, Category = "Spawning")
	float DamageIncrement = 5.f; // how much damage to add to the enemy 

	


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Spawning")
	void SpawnEnemy();

	UFUNCTION(BlueprintCallable, Category = "Spawning")
	void OnEnemyDeath();



private:

	FTimerHandle SpawnTimerHandle;
};
