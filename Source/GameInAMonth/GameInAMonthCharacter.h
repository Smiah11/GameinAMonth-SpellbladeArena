// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "MainSword.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "GameInAMonthCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);


/*
UENUM(BlueprintType)
enum class EAbilityType : uint8
{
	ManaBall UMETA(DisplayName = "Manaball"),
	AOE UMETA(DisplayName = "AOE"),
	Teleport UMETA(DisplayName = "Teleport"),
};
*/

UCLASS(config=Game)
class AGameInAMonthCharacter : public ACharacter
{
	GENERATED_BODY()



public:
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


	// Mage Mode Input Action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MageModeAction;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* CastAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* TeleportAcivationAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* AOEAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ManaBallAction;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mage Mode", meta = (AllowPrivateAccess = "true"))
	bool bIsMageModeActive = false;


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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties", meta = (AllowPrivateAccess = "true"))
	float CurrentMana = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties", meta = (AllowPrivateAccess = "true"))
	float MaxMana = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties", meta = (AllowPrivateAccess = "true"))
	UMaterial* AuraMaterial; // overlay material for the player

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties", meta = (AllowPrivateAccess = "true"))
	UNiagaraSystem* AuraParticles; // particles for the player

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties", meta = (AllowPrivateAccess = "true"))
	UNiagaraComponent* AuraParticlesComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties", meta = (AllowPrivateAccess = "true"))
	UMaterial* WarriorMaterial; // overlay material for the player

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties", meta = (AllowPrivate))
	UNiagaraComponent* WarriorAuraParticlesComponent;

public:
	AGameInAMonthCharacter();


	UFUNCTION(BlueprintCallable)
	void SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled); // Set the weapon collision enabled

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

	void SetUpWarriorControls(UInputComponent* Input); // Set up the warrior controls

	void AdjustInput();


	UFUNCTION(BlueprintCallable)
	void Block(); // Handle the player block
	UFUNCTION(BlueprintCallable)
	void StopBlock(); // Stop the player block
	void DrainStamina(); // Drain the player stamina while blocking
	void RegenStamina(); // Regen the player stamina while not blocking or attacking


	//Mage mode
	void RegenMana(); // Regen the player mana

	UFUNCTION(BlueprintCallable)
	void HandleMana(float ManaCost); // Handle the player mana

	void ResetMageModeToggle(); // Reset the mage mode toggle
	void SetUpMageControls(UInputComponent* Input); // Set up the mage controls

	//UFUNCTION(BlueprintCallable)
	//void ActivateAbilityMode(); // Activate the ability mode

	//UFUNCTION(BlueprintCallable)
	//void ExecuteAbility(); // Execute the ability

	UFUNCTION(BlueprintImplementableEvent, Category = "Magic")
	void CastManaBall();

	UFUNCTION(BlueprintImplementableEvent, Category = "Magic")
	void CastAOE();

	UFUNCTION(BlueprintImplementableEvent, Category = "Magic")
	void CastTeleport();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities", meta = (AllowPrivateAccess = "true"))
	bool bIsAbilityReady = false; // If the player can use the ability


	void ExecuteTeleport(); // Execute the teleport ability

	void ExecuteAOE(); // Execute the AOE ability

	void ExecuteManaBall(); // Execute the mana ball ability


	void PlayAttackAnimation(); // Play the attack animation


	// Attached swords on the player
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	TArray<AMainSword*> AttachedSwords; // Sword class // MAY NEED TO REMOVE TO FIX THE DOUBLE ATTACK BUG

	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities", meta = (AllowPrivateAccess = "true"))
	//TEnumAsByte<EAbilityType>CurrentAbilityType;


	void ToggleMageMode(); // Toggle the mage mode
	

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
	float DodgeStaminaCost = 10.f;



	//Combat System
	int ComboCounter = 0; // Combo counter
	float ComboResetTime = 2.f; // Time to reset the combo counter
	FTimerHandle ComboResetTimer; // Timer to reset the combo counter
	float AttackStaminaCost = 5.f; // Stamina cost for attacking
	float BaseDamage;
	float DamageMultiplier = 1.f; // Damage multiplier
	float AttackCooldown; // Attack cooldown
	float LastAttackTime = 0.f; // Last attack time
	bool bCanAttack = true; // If the player can attack


	//Block System
	bool bIsBlocking = false; // If the player is blocking
	float BlockStaminaCost = 5.f;
	float BlockStaminaDrainRate = 10.f; // Stamina drain rate while blocking needs to be double of the stamin regen to make it effective
	float BlockDamageReduction = 0.5f; // Damage reduction while blocking
	FTimerHandle StaminaDrainTimer; // Timer to drain the stamina while blocking
	FTimerHandle RegenStaminaTimer;



	//Mage Mode
	float MageModeManaCost = 20.f; // Stamina cost for mage mode
	float LastMageAttackTime = 0.f; // Last attack time for mage mode
	float MageModeCooldowm = 2.f; // Switch speed for mage mode
	bool bCanToggleMageMode = true; // If the player can toggle mage mode
	FTimerHandle MageModeCooldownTimer; // Timer to cooldown the mage mode
	FTimerHandle RegenManaTimer; // Timer to regen the mana will only use when not in mage mode






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

