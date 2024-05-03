// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AI_Spawner.h"
#include "TrainingDummy.generated.h"




/// <summary>
/// NOTE: This class was originally going to be a training dummy, however it was later decided to be a generic enemy class because it became a solid base for the enemy
/// </summary>
UCLASS()
class GAMEINAMONTH_API ATrainingDummy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATrainingDummy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimSequence* HitAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Properties)
	float Health = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Properties)
	float MaxHealth = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* DeathAnim;

	UPROPERTY(EditAnywhere, Category = "Spawning")
	AAI_Spawner* Spawner; // reference to the spawner that spawned this enemy 


private:

	FTimerHandle HandleDeathTimer;

	void HandleDeath();

	bool bIsDead = false;

};
