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

	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0); // Get the player character

	if (!PlayerCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player Character Not Found"))
		return;
	}

	GetBlackboardComponent()->SetValueAsObject("Target Actor", PlayerCharacter); // set the player as the target actor in the blackboard 


	ACharacter* EnemyCharacter = Cast<ACharacter>(GetPawn()); // Get the character
	if (!EnemyCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("Enemy Character Not Found"));

		GetWorld()->GetTimerManager().SetTimer(DelayedInitialisationTimer, this, &AEnemyAIController::DelayedInitialisation, 0.5f, false); // Call the delayed initialisation

		return;
	}

	TArray<AActor*> AttachedComponents;
	EnemyCharacter->GetAttachedActors(AttachedComponents); // Get the attached components

	UE_LOG(LogTemp, Warning, TEXT("Attached Components: %d"), AttachedComponents.Num())

	
	for (AActor* Component : AttachedComponents)
	{
		AEnemyWeapon* Weapon = Cast<AEnemyWeapon>(Component);

		if (Weapon)
		{

			Weapon->SetOwner(EnemyCharacter); // Set the owner
			Weapon->NewDamage = Damage; // Set the damage
			EnemyWeapon = Weapon; // Set the weapon
			
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Weapon Not Found"))
		}

	}

	if (EnemyWeapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("Weapon Found"))
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Weapon Notttt Found"))
	}

}


void AEnemyAIController::DelayedInitialisation()
{
	

	// 2 DAYS Later... just realised I should've set this up in the character class instead of the controller :(

	ACharacter* EnemyCharacter = Cast<ACharacter>(GetPawn()); // Get the character
	TArray<AActor*> AttachedComponents;
	EnemyCharacter->GetAttachedActors(AttachedComponents); // Get the attached components

	UE_LOG(LogTemp, Warning, TEXT("Attached Components: %d"), AttachedComponents.Num())


		for (AActor* Component : AttachedComponents)
		{
			AEnemyWeapon* Weapon = Cast<AEnemyWeapon>(Component);

			if (Weapon)
			{

				Weapon->SetOwner(EnemyCharacter); // Set the owner
				Weapon->NewDamage = Damage; // Set the damage
				EnemyWeapon = Weapon; // Set the weapon

			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Weapon Not Found"))
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



	float AcceptanceRadius = 120; // set the acceptance radius to 0

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
		UE_LOG(LogTemp, Warning, TEXT("Light attack Combo Counter: %d"), ComboCounter)

	}
	else
	{

		ComboCheck(); // Check the combo
		UE_LOG(LogTemp, Warning, TEXT("Light Attack Combo Counter: %d"), ComboCounter); // Log the combo counter
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
		UE_LOG(LogTemp, Warning, TEXT("Heavy attack Combo Counter: %d"), ComboCounter)

	}
	else
	{

		ComboCheck(); // Check the combo
		UE_LOG(LogTemp, Warning, TEXT("Heavy Attack Combo Counter: %d"), ComboCounter); // Log the combo counter
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
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Interrupting"))
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

	if (RandomNumber < InterruptionChance)
	{
		bIsInterruptable = true;
	}
	else
	{
		bIsInterruptable = false;

	}

	return RandomNumber < InterruptionChance; // Return true if the random number is less than the interruption chance
}

void AEnemyAIController::NotifyDamageTaken()
{
	bIsTakingDamage = true;
}

void AEnemyAIController::SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{

	if (EnemyWeapon)
	{

		UE_LOG(LogTemp, Warning, TEXT("collsion enable"));
		EnemyWeapon->BoxCollision->SetCollisionEnabled(CollisionEnabled); // Set the collision of the sword
	}
	

}

void AEnemyAIController::Attack()
{
	float AttackCooldown = FMath::RandRange(0.5f, 2.f);// Set the AI to attack speed to be a bit random 

	float CurrentTime = GetWorld()->GetTimeSeconds(); // Get the current time

	UE_LOG(LogTemp, Warning, TEXT("Attack Called"))

	// This sets up the attack cool down so i wont have to keep using a wait node in the behaviour tree
	if (CurrentTime - LastAttackTime < AttackCooldown) // Check if the current time is less than the attack cooldown
	{
		UE_LOG(LogTemp, Warning, TEXT("Attack on Cooldown"))
		return; // Return if the attack is on cooldown
	}

	LastAttackTime = CurrentTime; // Set the last attack time to the current time


	float InterruptionChance = 10.f;// Chance to be interrupted

	CheckForInterruption(InterruptionChance);// Check if the AI can be interrupted

	// if the character takes damage whilst interrruptable then call interrupt

	if (bIsInterruptable && bIsTakingDamage)
	{
		Interrupt();
		return;
	}



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


