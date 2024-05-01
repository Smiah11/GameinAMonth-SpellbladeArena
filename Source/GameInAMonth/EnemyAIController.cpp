// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"
#include "NavigationSystem.h"
#include "GameFramework/Character.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EnemyWeapon.h"
#include "Kismet/GameplayStatics.h"


AEnemyAIController::AEnemyAIController()
{
	PrimaryActorTick.bCanEverTick = true;

	bIsInterruptable = false; // Set the AI to not be interruptable

	//bCanAttack = true; // Set the AI to be able to attack
}

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();

	if (BehaviorTree)
	{
		RunBehaviorTree(BehaviorTree);
	}


	ACharacter* EnemyCharacter = Cast<ACharacter>(GetPawn()); // Get the character
	TArray<AActor*> AttachedComponents;
	EnemyCharacter->GetAttachedActors(AttachedComponents); // Get the attached components

	for (AActor* Component : AttachedComponents)
	{
		AEnemyWeapon* Weapon = Cast<AEnemyWeapon>(Component);

		if (Weapon)
		{

			Weapon->SetOwner(EnemyCharacter); // Set the owner
			EnemyWeapon = Weapon; // Set the weapon
			EnemyWeapon->NewDamage = Damage; // Set the damage
			
		}

	}

}


void AEnemyAIController::MoveToPlayer()
{
	// get the player character
	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	// early return if the player character is not found
	if (!PlayerCharacter)
	{
		return;
	}

	GetBlackboardComponent()->SetValueAsObject("Target Actor", PlayerCharacter); // set the player as the target actor in the blackboard 

	float AcceptanceRadius = 200.f; // set the acceptance radius to 0

	// get the player location
	FVector PlayerLocation = PlayerCharacter->GetActorLocation();

	// move to the player
	MoveToLocation(PlayerLocation,AcceptanceRadius);

	float Distance = FVector::Dist(GetPawn()->GetActorLocation(), PlayerLocation); // get the distance between the AI and the player

	if (Distance <= AcceptanceRadius) // if the AI is at the player location or closer 
	{
		bIsAtLocation = true;
	}
	else
	{
		bIsAtLocation = false;
	}

}

void AEnemyAIController::LightAttack()
{
	
	if (!bCanAttack)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot Attack"))
			return;
	}
	

	//GetBlackboardComponent()->SetValueAsInt("bIsAttacking", 1); // set the blackboard value to true

	bIsHeavyAttacking = false; // Set the AI to light attack


	if (ComboCounter == 0)
	{
		ComboCounter++; // Increment the combo counter
		PlayLightAttackAnimations(); // Play the attack animation
		UE_LOG(LogTemp, Warning, TEXT("Combo Counter: %d"), ComboCounter)

	}
	else
	{

		ComboCheck(); // Check the combo
		UE_LOG(LogTemp, Warning, TEXT("Combo Counter: %d"), ComboCounter); // Log the combo counter
	}

	GetWorld()->GetTimerManager().SetTimer(ComboTimerHandle, this, &AEnemyAIController::ResetCombo, ComboResetTime, false); // Set a timer to reset the combo counter 

}

void AEnemyAIController::HeavyAttack()
{

	
	if (!bCanAttack)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot Attack"))
			return;
	}
	

	bIsHeavyAttacking = true; // Set the AI to heavy attack


	if (ComboCounter == 0)
	{
		ComboCounter++; // Increment the combo counter
		PlayHeavyAttackAnimations(); // Play the attack animation
		UE_LOG(LogTemp, Warning, TEXT("Combo Counter: %d"), ComboCounter)

	}
	else
	{

		ComboCheck(); // Check the combo
		UE_LOG(LogTemp, Warning, TEXT("Combo Counter: %d"), ComboCounter); // Log the combo counter
	}

	GetWorld()->GetTimerManager().SetTimer(ComboTimerHandle, this, &AEnemyAIController::ResetCombo, ComboResetTime, false); // Set a timer to reset the combo counter 
}

void AEnemyAIController::ComboCheck()
{


	if (bIsHeavyAttacking) // If the AI is heavy attacking
	{
		ComboCounter = ComboCounter % 3 + 1; // Reset the combo counter to 1 if it reaches 3
		PlayHeavyAttackAnimations(); // Play the heavy attack animation
	}
	else
	{
		PlayLightAttackAnimations(); // Play the light attack animation#
		ComboCounter = ComboCounter % 5 + 1; // Reset the combo counter to 1 if it reaches 5
	}


}

void AEnemyAIController::PlayHeavyAttackAnimations()
{
	UAnimMontage* MontageToPlay = nullptr;


	// bit of a cheat here, but we're going to create a dynamic montage from the slot animations
	UAnimMontage* HeavyMontage1 = UAnimMontage::CreateSlotAnimationAsDynamicMontage(HeavyAttackMontage1, "DefaultSlot");
	UAnimMontage* HeavyMontage2 = UAnimMontage::CreateSlotAnimationAsDynamicMontage(HeavyAttackMontage2, "DefaultSlot");
	UAnimMontage* HeavyMontage3 = UAnimMontage::CreateSlotAnimationAsDynamicMontage(HeavyAttackMontage3, "DefaultSlot");


	// Switch Statement using the ComboCounter to determine which attack animation to play

	switch (ComboCounter)
	{
	case 1:
		MontageToPlay = HeavyMontage1;
		break;
	case 2:
		MontageToPlay = HeavyMontage2;
		break;
	case 3:
		MontageToPlay = HeavyMontage3;
		break;
	default:
		MontageToPlay = HeavyMontage1;
		break;
	}



	auto EnemyCharacter = Cast<ACharacter>(GetPawn()); // Get the character

	if (MontageToPlay != nullptr)
	{
		EnemyCharacter->PlayAnimMontage(MontageToPlay); // Play the animation
		
	}

	float InterruptionChance = 25.f;

	if (CheckForInterruption(InterruptionChance))
	{
		bIsInterruptable = true;
	}




	/*
	float Damage = BaseDamage + ComboCounter * DamageMultiplier; // Calculate the damage

	for (AMainSword* Sword : AttachedSwords)
	{
		Sword->PerformAttack(Damage); // Set the damage of the sword
	}
	*/

}

void AEnemyAIController::PlayLightAttackAnimations()
{

	UAnimMontage* MontageToPlay = nullptr;


	// bit of a cheat here, but we're going to create a dynamic montage from the slot animations
	UAnimMontage* LightMontage1 = UAnimMontage::CreateSlotAnimationAsDynamicMontage(LightAttackMontage1, "DefaultSlot");
	UAnimMontage* LightMontage2 = UAnimMontage::CreateSlotAnimationAsDynamicMontage(LightAttackMontage2, "DefaultSlot");
	UAnimMontage* LightMontage3 = UAnimMontage::CreateSlotAnimationAsDynamicMontage(LightAttackMontage3, "DefaultSlot");
	UAnimMontage* LightMontage4 = UAnimMontage::CreateSlotAnimationAsDynamicMontage(LightAttackMontage4, "DefaultSlot");
	UAnimMontage* LightMontage5 = UAnimMontage::CreateSlotAnimationAsDynamicMontage(LightAttackMontage5, "DefaultSlot");


	// Switch Statement using the ComboCounter to determine which attack animation to play

	switch (ComboCounter)
	{
	case 1:
		MontageToPlay = LightMontage1;
		break;
	case 2:
		MontageToPlay = LightMontage2;
		break;
	case 3:
		MontageToPlay = LightMontage3;
		break;
	case 4:
		MontageToPlay = LightMontage4;
		break;
	case 5:
		MontageToPlay = LightMontage5;
		break;

	default:
		MontageToPlay = LightMontage1;
		break;
	}



	auto EnemyCharacter = Cast<ACharacter>(GetPawn()); // Get the character

	if (MontageToPlay != nullptr)
	{
		EnemyCharacter->PlayAnimMontage(MontageToPlay); // Play the animation

	}

}

void AEnemyAIController::ResetAttack()
{
	bCanAttack = true; // Set the AI to attack
	//maybe reset combo counter idk yet
}

void AEnemyAIController::ResetCombo()
{
		ComboCounter = 0; // Reset the combo counter
}

void AEnemyAIController::Interrupt()
{
	if (!bIsInterruptable)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot Interrupt"))
			return;
	}

	UAnimMontage* InterruptMontage = UAnimMontage::CreateSlotAnimationAsDynamicMontage(InterruptAnim, "DefaultSlot"); // Create a dynamic montage from the slot animation
	auto EnemyCharacter = Cast<ACharacter>(GetPawn()); // Get the character

	if (InterruptMontage != nullptr)
	{
		EnemyCharacter->PlayAnimMontage(InterruptMontage); // Play the animation
	}

	bCanAttack = false; // Set the AI to not attack
	GetWorld()->GetTimerManager().SetTimer(InterruptTimerHandle, this, &AEnemyAIController::ResetAttack, 1.f, false); // Set a timer to reset the attack
	bIsInterruptable = false; // Set the AI to not interrupt
}

bool AEnemyAIController::CheckForInterruption(float InterruptionChance)
{
	float RandomNumber = FMath::RandRange(0.f, 100.f); // Generate a random number between 0 and 100
	return RandomNumber < InterruptionChance; // Return true if the random number is less than the interruption chance
}

void AEnemyAIController::Attack()
{


	if (EnemyWeapon)
	{
		EnemyWeapon->SetCanDamage(true); // Set the weapon to be able to damage
	}

	int AttackType = FMath::RandRange(0, 1); // Generate a random number between 0 and 100

	if (AttackType == 0) // If the attack type is 0
	{
		LightAttack(); // Set the AI to heavy attack
	}
	else
	{
		HeavyAttack(); // Set the AI to light attack
	}


	
}


