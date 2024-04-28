// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "GameInAMonthCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class AGameInAMonthCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	/** Dodge Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* DodgeAction;

	/** Attack Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* AttackAction;

	/** Block Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* BlockAction;


	/** Dodge strength */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat System", meta = (AllowPrivateAccess = "true"))
	float DodgeStrength = 1500.f;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties", meta = (AllowPrivateAccess = "true"))
	float CurrentHealth = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties", meta = (AllowPrivateAccess = "true"))
	float MaxHealth = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties", meta = (AllowPrivateAccess = "true"))
	float CurrentStamina = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties", meta = (AllowPrivateAccess = "true"))
	float MaxStamina = 100.f;




public:
	AGameInAMonthCharacter();



	

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	void Dodge(); // Called for dodge input

	void StartDodgeCooldown(); // Start the dodge cooldown

	void ResetDodgeCooldown(); // Reset the dodge cooldown

	void ResetPlayer(); // Reset the player stats for next level

	void ApplyPowerup(); // Apply powerup to the player

	void HandleDeath(); // Handle the player death

	void HandleDamage(float Damage); // Handle the player damage

	float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override; // Take damage function

	void HandleStamina(float StaminaCost); // Handle the player stamina

	void Attack(); // Handle the player attack

	void ComboCheck(); // Check if the player is doing a combo

	void ResetCombo(); // Reset the combo counter

	UFUNCTION(BlueprintCallable)
	void Block(); // Handle the player block
	UFUNCTION(BlueprintCallable)
	void StopBlock(); // Stop the player block
	void DrainStamina(); // Drain the player stamina while blocking
	void RegenStamina(); // Regen the player stamina while not blocking or attacking


	UFUNCTION()
	void OnSwordHit(AActor* HitActor, AActor* OtherActor); // Function to handle the sword hit


	void PlayAttackAnimation(); // Play the attack animation



	
			

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

	virtual void Tick(float DeltaTime) override;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

private:

	// Dodge System
	FVector LastInputDirection = FVector::ZeroVector; // Last direction of the input
	bool bHasValidInputDirection = false; // If the input direction is valid
	bool bCanDodge = true; // If the character can dodge
	FTimerHandle DodgeCooldown;



	//Combat System
	int ComboCounter = 0; // Combo counter
	float ComboResetTime = 1.5f; // Time to reset the combo counter
	FTimerHandle ComboResetTimer; // Timer to reset the combo counter
	float BaseDamage = 10.f;
	float DamageMultiplier = 1.f; // Damage multiplier
	float AttackCooldown = 0.5f; // Attack cooldown
	float LastAttackTime = 0.f; // Last attack time
	bool bCanAttack = true; // If the player can attack


	//Block System
	bool bIsBlocking = false; // If the player is blocking
	float BlockStaminaDrainRate = 10.f; // Stamina drain rate while blocking
	float BlockDamageReduction = 0.5f; // Damage reduction while blocking
	FTimerHandle StaminaDrainTimer; // Timer to drain the stamina while blocking
	FTimerHandle RegenStaminaTimer;



	//




public:

	//Animations
	 
	// Combo Animations

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations", meta = (AllowPrivate))
	UAnimSequence* AttackMontage1; // Attack Montage

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations", meta = (AllowPrivate))
	UAnimSequence* AttackMontage2; // Attack Montage 2

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations", meta = (AllowPrivate))
	UAnimSequence* AttackMontage3; // Attack Montage 3

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations", meta = (AllowPrivate))
	UAnimSequence* AttackMontage4; // Attack Montage 4 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations", meta = (AllowPrivate))
	UAnimSequence* AttackMontage5; // Attack Montage 5

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations", meta = (AllowPrivate))
	UAnimMontage* DodgeMontage; // DodgeMontage

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations", meta = (AllowPrivate))
	UAnimMontage* BlockAnimation; // Block Montage

};

