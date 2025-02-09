// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyWeapon.h"
#include "AIController.h"

#include "EnemyAIController.generated.h"

/**
 * 
 */
UCLASS()
class GAMEINAMONTH_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

	
		AEnemyAIController();


public:

		virtual void BeginPlay() override;

		//virtual void Tick(float DeltaTime) override;
		//virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


		UFUNCTION(BlueprintCallable, Category = "AI")
		void MoveToPlayer();


		UFUNCTION(BlueprintCallable, Category = "AI")
		void LightAttack();

		UFUNCTION(BlueprintCallable, Category = "AI")
		void HeavyAttack();

		UFUNCTION(BlueprintCallable, Category = AI)
		void Attack();

		void ComboCheck();

		void PlayHeavyAttackAnimations();

		void PlayLightAttackAnimations();



		FTimerHandle DelayedInitialisationTimer;
		void DelayedInitialisation();

		void ResetAttack();


		UFUNCTION(BlueprintCallable, Category = "AI")
		void ResetCombo();

		UFUNCTION(BlueprintCallable, Category = "AI")
		void Interrupt();


		bool CheckForInterruption(float InterruptionChance); // Check if the enemy should be interrupted


		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		AEnemyWeapon* EnemyWeapon;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
		float Damage = 10.f;


		UPROPERTY(EditDefaultsOnly, Category = "AI")
		UBehaviorTree* BehaviorTree;

		UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "AI")
		bool bIsAtLocation = false;


		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
		bool bIsAttacking = false;


		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
		bool bIsLightAttacking = false;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
		bool bIsHeavyAttacking = false;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
		bool bIsInterruptable = false;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
		bool bCanAttack = true;

		 
		bool bIsTakingDamage = false; // Check if the enemy is taking damage

		void NotifyDamageTaken();



public:
	//animations

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations", meta = (AllowPrivate))
	UAnimSequence* LightAttackMontage1; // Attack Montage

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations", meta = (AllowPrivate))
	UAnimSequence* LightAttackMontage2; // Attack Montage 2

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations", meta = (AllowPrivate))
	UAnimSequence* LightAttackMontage3; // Attack Montage 3

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations", meta = (AllowPrivate))
	UAnimSequence* LightAttackMontage4; // Attack Montage 4 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations", meta = (AllowPrivate))
	UAnimSequence* LightAttackMontage5; // Attack Montage 5


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations", meta = (AllowPrivate))
	UAnimSequence* HeavyAttackMontage1; // Attack Montage

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations", meta = (AllowPrivate))
	UAnimSequence* HeavyAttackMontage2; // Attack Montage 2

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations", meta = (AllowPrivate))
	UAnimSequence* HeavyAttackMontage3; // Attack Montage 3

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations", meta = (AllowPrivate))
	UAnimSequence* HeavyAttackMontage4; // Attack Montage 4 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations", meta = (AllowPrivate))
	UAnimSequence* HeavyAttackMontage5; // Attack Montage 5

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations", meta = (AllowPrivate))
	UAnimSequence* InterruptAnim; // Interrup Animation

	UFUNCTION(BlueprintCallable)
	void SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled); // Set the weapon collision enabled


private:

	float ComboResetTime = 10.f; // Time before combo resets
	int ComboCounter = 0;
	FTimerHandle ComboTimerHandle; // Timer for combo reset

	FTimerHandle InterruptTimerHandle; // Timer for interruption


	float LastAttackTime; // Time of last attack


	
};
