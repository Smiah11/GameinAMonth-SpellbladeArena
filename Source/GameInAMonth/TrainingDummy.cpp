// Fill out your copyright notice in the Description page of Project Settings.


#include "TrainingDummy.h"

// Sets default values
ATrainingDummy::ATrainingDummy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATrainingDummy::BeginPlay()
{
	Super::BeginPlay();

	Health = MaxHealth;
	
}

// Called every frame
void ATrainingDummy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ATrainingDummy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float ATrainingDummy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float DamageApplied = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	Health -= DamageApplied;

	UAnimMontage* Montage1 = UAnimMontage::CreateSlotAnimationAsDynamicMontage(HitAnim, "DefaultSlot");

	UE_LOG(LogTemp, Warning, TEXT("Health: %f"), Health);

	if (Montage1)
	{
		PlayAnimMontage(Montage1);
	}

	if (Health <= 0)
	{
		HandleDeath();
	}

	return DamageApplied;
}

void ATrainingDummy::HandleDeath()
{
		Destroy();
}

