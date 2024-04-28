// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameInAMonthCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/Character.h"


DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AGameInAMonthCharacter

AGameInAMonthCharacter::AGameInAMonthCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)



	// Set the default values for the character
	CurrentHealth = MaxHealth;
	CurrentStamina = MaxStamina;
	ComboCounter = 0;
	ComboResetTime = 1.5f;


}

void AGameInAMonthCharacter::Tick(float DeltaTime)
{
		Super::Tick(DeltaTime);


		if (CurrentStamina <= 0)
		{

			bCanAttack = false;
			bCanDodge = false;

		}
		else
		{
			bCanAttack = true;
		}

		
}

void AGameInAMonthCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	GetWorldTimerManager().SetTimer(RegenStaminaTimer, this, &AGameInAMonthCharacter::RegenStamina, 1.0f, true); //Regen stamina every 1 second
}

//////////////////////////////////////////////////////////////////////////
// Input

void AGameInAMonthCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AGameInAMonthCharacter::Move);

		// Dodging
		EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Completed, this, &AGameInAMonthCharacter::Dodge); //Dodge is a combo input

		// Attacking
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Completed, this, &AGameInAMonthCharacter::Attack); // Attack action binding


		// Blocking
		EnhancedInputComponent->BindAction(BlockAction, ETriggerEvent::Started, this, &AGameInAMonthCharacter::Block);
		EnhancedInputComponent->BindAction(BlockAction, ETriggerEvent::Completed, this, &AGameInAMonthCharacter::StopBlock);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AGameInAMonthCharacter::Look);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AGameInAMonthCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);



		// calculate the total direction
		FVector TotalDirection = (ForwardDirection * MovementVector.Y) + (RightDirection * MovementVector.X);
		if (!TotalDirection.IsNearlyZero()) // check if the total direction is not zero so checking if its valid
		{
			LastInputDirection = TotalDirection.GetSafeNormal(); // save the last input direction
			bHasValidInputDirection = true; // set the flag to true
		}



		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);

		//Double Tap input is in the mappings using UE5 combos

	}
}

void AGameInAMonthCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AGameInAMonthCharacter::Dodge()
{

	if (bHasValidInputDirection && bCanDodge)
	{
		FVector Direction = LastInputDirection; // Get the last input direction
		LaunchCharacter(Direction * DodgeStrength, true, true); // Launch the character in the direction of the dodge
		bHasValidInputDirection = false; // Reset the flag
		HandleStamina(10.f); // Subtract 10 stamina
		StartDodgeCooldown(); // Start the dodge cooldown

		if (DodgeMontage != nullptr)
		{
			PlayAnimMontage(DodgeMontage); // Play the dodge montage
		}

	}


}

void AGameInAMonthCharacter::StartDodgeCooldown()
{
	bCanDodge = false; // Set the flag to false
	GetWorldTimerManager().SetTimer(DodgeCooldown, this, &AGameInAMonthCharacter::ResetDodgeCooldown, 1.f, false); // 1 second cooldown
}

void AGameInAMonthCharacter::ResetDodgeCooldown()
{
		bCanDodge = true; // Set the flag to true so the player can dodge again
}



void AGameInAMonthCharacter::HandleDamage(float Damage)
{

	CurrentHealth -= Damage; // Subtract the damage from the health
	if (CurrentHealth <= 0) // Check if the health is less than or equal to 0
	{
		HandleDeath(); // Call the death function
	}
}

float AGameInAMonthCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (bIsBlocking)
	{
		DamageAmount*= BlockDamageReduction; // Reduce the damage by the block damage reduction
	}


	HandleDamage(DamageAmount); // Call the handle damage function

	return DamageAmount; // Return the damage amount
}

void AGameInAMonthCharacter::HandleStamina(float StaminaCost)
{

	UE_LOG(LogTemp, Warning, TEXT("Handling Stamina"))

		CurrentStamina -= StaminaCost; // Subtract the stamina cost from the stamina
		if (CurrentStamina <= 0) // Check if the stamina is less than or equal to 0
		{
			UE_LOG(LogTemp, Warning, TEXT("Out of Stamina"))
		}
}

void AGameInAMonthCharacter::Attack()
{

	if (!bCanAttack)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot Attack"))
		return;
	}

	float CurrentTime = GetWorld()->GetTimeSeconds(); // Get the current time

	if (CurrentTime - LastAttackTime < AttackCooldown ) // Check if the current time is less than the attack cooldown
	{
		UE_LOG(LogTemp, Warning, TEXT("Attack on Cooldown"))
		return; // Return if the attack is on cooldown
	}

	LastAttackTime = CurrentTime; // Set the last attack time to the current time

	HandleStamina(5);


	if (ComboCounter == 0)
	{
		ComboCounter++; // Increment the combo counter
		PlayAttackAnimation(); // Play the attack animation
		UE_LOG(LogTemp, Warning, TEXT("Combo Counter: %d"), ComboCounter)
		
	}
	else
	{
		ComboCheck(); // Check the combo
		UE_LOG(LogTemp, Warning, TEXT("Combo Counter: %d"), ComboCounter); // Log the combo counter
	}

	GetWorld()->GetTimerManager().SetTimer(ComboResetTimer, this, &AGameInAMonthCharacter::ResetCombo, ComboResetTime, false); // Set a timer to reset the combo counter 


}

void AGameInAMonthCharacter::ComboCheck()
{
	ComboCounter = ComboCounter % 5 + 1; // Reset the combo counter to 1 if it reaches 5
	PlayAttackAnimation(); // Play the attack animation
	

}

void AGameInAMonthCharacter::ResetCombo()
{
	ComboCounter = 0;
}

void AGameInAMonthCharacter::Block()
{
	if (CurrentStamina > 0)
	{
		bIsBlocking = true; // Set the flag to true
		PlayAnimMontage(BlockAnimation); // Play the block animation
		HandleStamina(10.f); // Subtract 10 stamina for blocking will be used for parrying

		//GetWorldTimerManager().SetTimer(StaminaDrainTimer, this, &AGameInAMonthCharacter::DrainStamina, 1.f, true); // Set a timer to drain the stamina

	}

}

void AGameInAMonthCharacter::StopBlock()
{
	bIsBlocking = false; // Set the flag to false;
	StopAnimMontage(BlockAnimation); // Stop the block animation
	GetWorldTimerManager().ClearTimer(StaminaDrainTimer); // Clear the stamina drain timer
}

void AGameInAMonthCharacter::DrainStamina()
{
	HandleStamina(BlockStaminaDrainRate); // Drain the stamina

	if (CurrentStamina <= 0)
	{
		StopBlock(); // Stop the block if the stamina is less than or equal to 0
	}

}

void AGameInAMonthCharacter::RegenStamina()
{


	CurrentStamina += 5.f; // Regen by 2
	CurrentStamina = FMath::Clamp(CurrentStamina, 0.f, MaxStamina); // Clamp the stamina
	UE_LOG(LogTemp, Warning, TEXT("Stamina: %f"), CurrentStamina)

	if (CurrentStamina >= MaxStamina)
	{
		CurrentStamina = MaxStamina; // Set the stamina to the max stamina
	}
}

void AGameInAMonthCharacter::OnSwordHit(AActor* HitActor, AActor* OtherActor)
{
}

void AGameInAMonthCharacter::PlayAttackAnimation()
{

	/*if (IsAnyMontagePlaying())
	{

		UE_LOG(LogTemp, Warning, TEXT("Montage Playing"))
		return;

	}*/

	UAnimMontage* MontageToPlay = nullptr;


	// bit of a cheat here, but we're going to create a dynamic montage from the slot animations
	UAnimMontage* Montage1 = UAnimMontage::CreateSlotAnimationAsDynamicMontage(AttackMontage1, "DefaultSlot");
	UAnimMontage* Montage2 = UAnimMontage::CreateSlotAnimationAsDynamicMontage(AttackMontage2, "DefaultSlot");
	UAnimMontage* Montage3 = UAnimMontage::CreateSlotAnimationAsDynamicMontage(AttackMontage3, "DefaultSlot");
	UAnimMontage* Montage4 = UAnimMontage::CreateSlotAnimationAsDynamicMontage(AttackMontage4, "DefaultSlot");
	UAnimMontage* Montage5 = UAnimMontage::CreateSlotAnimationAsDynamicMontage(AttackMontage5, "DefaultSlot");


	// Switch Statement using the ComboCounter to determine which attack animation to play

	switch (ComboCounter)
	{
	case 1:
		MontageToPlay = Montage1;
		break;
	case 2:
		MontageToPlay = Montage2;
		break;
	case 3:
		MontageToPlay = Montage3;
		break;
	case 4:
		MontageToPlay = Montage4;
		break;
	case 5:
		MontageToPlay = Montage5;
		break;
	default:
		MontageToPlay = Montage1;
		break;
	}

	if (MontageToPlay != nullptr)
	{
		PlayAnimMontage(MontageToPlay);
		UE_LOG(LogTemp, Warning, TEXT("Playing Attack Animation"))
	}
}


void AGameInAMonthCharacter::ResetPlayer()
{
}

void AGameInAMonthCharacter::ApplyPowerup()
{
}

void AGameInAMonthCharacter::HandleDeath()
{
	// Handle the death of the character
	// For now, we will just reset the player

	
	Destroy();
}


