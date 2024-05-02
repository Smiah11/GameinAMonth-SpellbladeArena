// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TrainingDummy.h"
#include "AI_Spawner.generated.h"

UCLASS()
class GAMEINAMONTH_API AAI_Spawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAI_Spawner();

	//array to hold the different enemies that can be spawned
	UPROPERTY(EditDefaultsOnly, Category = "Spawning")
	TArray<TSubclassOf<ATrainingDummy>> EnemiesToSpawn;

	UPROPERTY(EditAnywhere, Category = "Spawning")
	float BaseDamage = 10.f;

	UPROPERTY(EditAnywhere, Category = "Spawning")
	float BaseHealth = 200.f;

	UPROPERTY(EditAnywhere, Category = "Spawning")
	ATrainingDummy* CurrentEnemy;

	UPROPERTY(EditAnywhere, Category = "Spawning")
	float SpawnDelay = 5.f;

	UPROPERTY(EditAnywhere, Category = "Spawning")
	float HealthIncrement = 75.f;

	UPROPERTY(EditAnywhere, Category = "Spawning")
	float DamageIncrement = 5.f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Spawning")
	void SpawnEnemy();

};
