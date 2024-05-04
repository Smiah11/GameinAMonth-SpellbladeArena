// Fill out your copyright notice in the Description page of Project Settings.


#include "TrainingDummy.h"
#include "AI_Spawner.h"
#include "EnemyAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameInAMonthGameMode.h"
#include <Kismet/GameplayStatics.h>

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

	GetCharacterMovement()->MaxWalkSpeed = FMath::RandRange(250.f,600.f);


	
}

// Called every frame
void ATrainingDummy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//UE_LOG(LogTemp, Warning, TEXT("Enemy Health: %f"), Health);

}

// Called to bind functionality to input
void ATrainingDummy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float ATrainingDummy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{

	if (bIsDead) return 0.f; // If the enemy is already dead, don't take any more damage

	AEnemyAIController* AIController = Cast<AEnemyAIController>(GetController());

	float DamageApplied = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	Health -= DamageApplied; // Subtract the damage from the health

	//UAnimMontage* Montage1 = UAnimMontage::CreateSlotAnimationAsDynamicMontage(HitAnim, "DefaultSlot"); // Create a dynamic montage from the hit animation (Not needed)

	UE_LOG(LogTemp, Warning, TEXT("Enemy Health: %f"), Health);


	// clamp the health
	Health = FMath::Clamp(Health, 0.f, MaxHealth);



	if (DamageApplied > 0)
	{
		AIController->NotifyDamageTaken(); // Notify the AI controller that the enemy has taken damage
	}

	if (Health <= 0)
	{
		bIsDead = true;
		//AIController->bCanAttack = false;
		AIController->UnPossess(); // Unpossess the AI controller so it stops attacking and running the behaviour tree
		PlayAnimMontage(DeathAnim); // Play the death animation
		UGameplayStatics::PlaySound2D(GetWorld(), DeathCue); // Play the death sound
		GetWorldTimerManager().SetTimer(HandleDeathTimer, this, &ATrainingDummy::HandleDeath, 2.f, false); // Set the timer to the length of the death animation
		UE_LOG(LogTemp, Warning, TEXT("Enemy Died"));
	}

	return DamageApplied;
}

void ATrainingDummy::HandleDeath()
{

	AGameInAMonthGameMode* GameMode = Cast<AGameInAMonthGameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode)
	{
		GameMode->GameWin();
	}

	Destroy();

	if (Spawner)
	{
		Spawner->OnEnemyDeath(); // Notify the spawner that this enemy has died so it can spawn a new one after 5 seconds
	}


}

