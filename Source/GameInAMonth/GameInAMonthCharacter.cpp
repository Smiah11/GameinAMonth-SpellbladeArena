// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameInAMonthCharacter.h"
#include "GameInAMonthGameMode.h"
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
#include "MainSword.h"
#include <NiagaraFunctionLibrary.h>
#include <Kismet/GameplayStatics.h>


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
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
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
	TargetCameraPosition = FVector(170.f, 110.f, 70.f);

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)


	//niagara for MageAura
	AuraParticlesComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("AuraParticlesComponent"));
	AuraParticlesComponent->SetupAttachment(GetMesh());
	AuraParticlesComponent->SetAutoActivate(false);

	//niagara for WarriorAura
	WarriorAuraParticlesComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("WarriorAuraParticlesComponent"));
	WarriorAuraParticlesComponent->SetupAttachment(GetMesh());
	WarriorAuraParticlesComponent->SetAutoActivate(true);


	GetMesh()->SetOverlayMaterial(WarriorMaterial);

	// Set the default values for the character



	CurrentHealth = MaxHealth;
	CurrentStamina = MaxStamina;
	ComboCounter = 0;
	ComboResetTime = 1.5f;


	

}



void AGameInAMonthCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);



	if (!FollowCamera->GetRelativeLocation().Equals(TargetCameraPosition), 0.1f) // If the camera is not at the target position
	{
		FVector NewCamPos = FMath::Lerp(FollowCamera->GetRelativeLocation(), TargetCameraPosition, DeltaTime* CameraLerpSpeed); // Lerp the camera to the target position
		FollowCamera->SetRelativeLocation(NewCamPos); // Set the new camera position
	}

		// If the player has enough stamina, they can attack set to 5
		//bCanAttack = true;

		if (CurrentStamina < DodgeStaminaCost)
		{
			// If the player does not have enough stamina to dodge, they can't dodge
			bCanDodge = false;
		}
	
	if (bIsMageModeActive)
	{


		if (CurrentMana < 20.f) // If the player does not have enough mana to use an ability
		{
			bIsAbilityReady = false;

		}
		else
		{
			bIsAbilityReady = true;
		}

		GetWorldTimerManager().ClearTimer(RegenManaTimer);
	}

	


}

void AGameInAMonthCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	bCanAttack = true;

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	//Find the attached swords

	// Actor method didnt work???
	TArray<USceneComponent*> AttachedComponents;
	GetMesh()->GetChildrenComponents(true, AttachedComponents);

	for (USceneComponent* Component : AttachedComponents)
	{
		AMainSword* Sword = Cast<AMainSword>(Component->GetOwner());
		if (Sword)
		{
			AttachedSwords.Add(Sword);

			// set the properties that i want
			AttackCooldown = Sword->AttackSpeed;
			Sword->NewDamage = BaseDamage;


		}
	}



	GetWorldTimerManager().SetTimer(RegenStaminaTimer, this, &AGameInAMonthCharacter::RegenStamina, 2.0f, true); //Regen stamina every 1 second
}

//////////////////////////////////////////////////////////////////////////
// Input

void AGameInAMonthCharacter::ToggleMageMode()
{

	// Toggle the mage mode
	if (bCanToggleMageMode) // cool down check
	{

		bIsMageModeActive = !bIsMageModeActive;

		// Set the camera
		//CameraBoom->TargetArmLength = bIsMageModeActive ? 500.f : 400.f;


		UAnimMontage* TransitionMontage = UAnimMontage::CreateSlotAnimationAsDynamicMontage(TransitionAnim, "DefaultSlot");

		if (TransitionMontage)
		{
			PlayAnimMontage(TransitionMontage);
		}
		
		// Play Sound
		UGameplayStatics::PlaySound2D(GetWorld(), SwitchSound);


	
		bCanAttack = bIsMageModeActive ? false : true; // If the player is in mage mode, they can't attack


		TargetCameraPosition = (bIsMageModeActive ? FVector(280.f, 70.f, 60.f) : FVector(170.f, 110.f, 70.f)); // Set the camera position gonna be done in tick to lerp


		//activate a niagara system at the actor location
	

		GetMesh()->SetOverlayMaterial(bIsMageModeActive ? AuraMaterial : WarriorMaterial); // Set the material overlay

		//AuraParticlesComponent->SetVisibility(bIsMageModeActive);

		EnableMageWidget();// Enable the mage widget
		
		if (bIsMageModeActive) // If the player is in mage mode, activate the particles
		{
			AuraParticlesComponent->Activate();
			WarriorAuraParticlesComponent->Deactivate();
			StaminaRegenRate = 8.f; // Set the stamina regen rate to 8

			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), WarriorParticles, GetActorLocation(), FRotator::ZeroRotator, FVector(1.f), true, true, ENCPoolMethod::AutoRelease, true);

		}
		else // If the player is not in mage mode, deactivate the particles and activate the warrior particles
		{
			AuraParticlesComponent->Deactivate();
			WarriorAuraParticlesComponent->Activate();
			StaminaRegenRate = 4.f; // Set the stamina regen rate back to normal

			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), MageParticles, GetActorLocation(), FRotator::ZeroRotator, FVector(1.f), true, true, ENCPoolMethod::AutoRelease, true);
		}

		if (bIsMageModeActive)
		{
			StopBlock();
		}


		if (!bIsMageModeActive) // If the player is not in mage mode, regen mana
		{
			GetWorldTimerManager().SetTimer(RegenManaTimer, this, &AGameInAMonthCharacter::RegenMana, 2.0f, true); //Regen Mana every 2 seconds
		}

		// make swords invisible
		for (AMainSword* Sword : AttachedSwords)
		{
			Sword->SetActorHiddenInGame(bIsMageModeActive);
		}


		// Set the movement speed
		GetCharacterMovement()->MaxWalkSpeed = bIsMageModeActive ? 800.f : 600.f;

		AdjustInput(); // Adjust the input bindings BASED ON the mode

		bIsAbilityReady = true; // Set the ability to be ready

		bCanToggleMageMode = false; // Set the ability to be on cooldown


		GetWorldTimerManager().SetTimer(MageModeCooldownTimer, this, &AGameInAMonthCharacter::ResetMageModeToggle, MageModeCooldowm, false); //Reset the timer after 2 second


	}

}

void AGameInAMonthCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{

	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up action bindings

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{

		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AGameInAMonthCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AGameInAMonthCharacter::Look);

	}

	AdjustInput(); // Adjust the input bindings BASED ON the mode

}

void AGameInAMonthCharacter::AdjustInput()
{
	// Get the enhanced input component
	UEnhancedInputComponent* NewInputComponent = Cast<UEnhancedInputComponent>(GetComponentByClass(UEnhancedInputComponent::StaticClass()));

	if (NewInputComponent)
	{
		// Clear current bindings specific to the modes
		NewInputComponent->ClearActionBindings();

		// Conditionally set up controls based on the active mode
		if (bIsMageModeActive)
		{
			SetUpMageControls(NewInputComponent);
			UE_LOG(LogTemp, Warning, TEXT("Mage Mode Active"))
		}
		else
		{
			SetUpWarriorControls(NewInputComponent);
			UE_LOG(LogTemp, Warning, TEXT("Warrior Mode Active"))
		}
	}
}

void AGameInAMonthCharacter::SetUpWarriorControls(UInputComponent* Input)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(Input)) {


		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AGameInAMonthCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AGameInAMonthCharacter::Look);


		// Dodging
		EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Completed, this, &AGameInAMonthCharacter::Dodge); //Dodge is a combo input

		// Attacking
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Completed, this, &AGameInAMonthCharacter::Attack); // Attack action binding


		// MageMode
		EnhancedInputComponent->BindAction(MageModeAction, ETriggerEvent::Triggered, this, &AGameInAMonthCharacter::ToggleMageMode);

		// Not using enhanced input for blocking as it is a hold action

		/*
			PlayerInputComponent->BindAction("Block", IE_Pressed, this, &AGameInAMonthCharacter::Block);
			PlayerInputComponent->BindAction("Block", IE_Released, this, &AGameInAMonthCharacter::StopBlock);
		*/


	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}



void AGameInAMonthCharacter::SetUpMageControls(UInputComponent* Input)
{


	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(Input)) {



		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AGameInAMonthCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AGameInAMonthCharacter::Look);


		// Dodging
		EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Completed, this, &AGameInAMonthCharacter::Dodge); // Activate ability mode action binding


		//EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &AGameInAMonthCharacter::ExecuteTeleport); // Attack action binding replaces Attack Action swapping for blueprint


		// MageMode
		EnhancedInputComponent->BindAction(MageModeAction, ETriggerEvent::Triggered, this, &AGameInAMonthCharacter::ToggleMageMode);

		EnhancedInputComponent->BindAction(TeleportAcivationAction, ETriggerEvent::Completed, this, &AGameInAMonthCharacter::ExecuteTeleport); // Activate Teleport

		EnhancedInputComponent->BindAction(AOEAction, ETriggerEvent::Completed, this, &AGameInAMonthCharacter::ExecuteAOE); // Activate AOE

		EnhancedInputComponent->BindAction(ManaBallAction, ETriggerEvent::Completed, this, &AGameInAMonthCharacter::ExecuteManaBall); // Activate Mana Ball

		// Not using enhanced input for blocking as it is a hold action

	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}

}



/*
void AGameInAMonthCharacter::ExecuteAbility()
{
	if (!bIsAbilityReady)
	{
		return;
	}

	UE_LOG(LogTemplateCharacter, Log, TEXT("Ability Executed"));

	switch (CurrentAbilityType)
	{
	case EAbilityType::ManaBall:
		CastManaBall();
		break;
	case EAbilityType::AOE:
		CastAOE();
		break;
	case EAbilityType::Teleport:
		CastTeleport();
		break;

		default:
			UE_LOG(LogTemplateCharacter, Error, TEXT("Ability not implemented"));
			break;
	}
}

*/


void AGameInAMonthCharacter::SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{
	// Set the collision of the box collision of the sword will be used wwith anim Notifys

	for (AMainSword* MainSword : AttachedSwords)
	{
		if (MainSword)
		{
			MainSword->BoxCollision->SetCollisionEnabled(CollisionEnabled);
		}
		else
		{
			return;
		}
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
	if (!bCanDodge) // Check if the character can dodge
	{
		return;
	}

	if (bHasValidInputDirection && bCanDodge)
	{
		FVector Direction = LastInputDirection; // Get the last input direction
		LaunchCharacter(Direction * DodgeStrength, true, true); // Launch the character in the direction of the dodge
		bHasValidInputDirection = false; // Reset the flag
		HandleStamina(DodgeStaminaCost); // Subtract 10 stamina
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
	UE_LOG(LogTemp, Warning, TEXT("TakingDamage: %f"), Damage);

	CurrentHealth -= Damage; // Subtract the damage from the health
	if (CurrentHealth <= 0) // Check if the health is less than or equal to 0
	{
		bIsDead = true; // Set the flag to true
		PlayAnimMontage(DeathAnimation); // Play the death montage

		UGameplayStatics::PlaySound2D(GetWorld(), DeathCue); // Play the death sound

		//disableinput
		GetController()->DisableInput(GetWorld()->GetFirstPlayerController()); // Disable the input

		GetWorldTimerManager().SetTimer(DeathTimer, this, &AGameInAMonthCharacter::HandleDeath, 2.f, false); // Call the handle death function after the delay
	}
}

float AGameInAMonthCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (bIsDead) // Check if the character is dead
	{
		return 0.f; // Return 0
	}


	if (bIsBlocking)
	{
		DamageAmount *= BlockDamageReduction; // Reduce the damage by the block damage reduction
	}

	if (bHasDamageReduction)
	{
		DamageAmount *= (1 - DamageReduction); // Reduce the damage by the damage reduction as a percentage
	}

	// ignore if its characters weapon
	if (DamageCauser == this)
	{
		return 0.f;
	}

	HandleDamage(DamageAmount); // Call the handle damage function

	return DamageAmount; // Return the damage amount
}

void AGameInAMonthCharacter::HandleStamina(float StaminaCost)
{

	//UE_LOG(LogTemp, Warning, TEXT("Handling Stamina"))

		CurrentStamina -= StaminaCost; // Subtract the stamina cost from the stamina
	if (CurrentStamina < 5) // Check if the stamina is less than or equal to 0
	{
		bCanAttack = false; // Set the can attack flag to false
		UE_LOG(LogTemp, Warning, TEXT("Out of Stamina"))
	}
}

void AGameInAMonthCharacter::Attack()
{

	if (!bCanAttack) // Check if the player can attack
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot Attack"))
			return;
	}

	if (bIsBlocking) // Check if the player is blocking before attacking
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot Attack while Blocking"))
			return;
	}

	float CurrentTime = GetWorld()->GetTimeSeconds(); // Get the current time

	if (CurrentTime - LastAttackTime < AttackCooldown) // Check if the current time is less than the attack cooldown
	{
		//UE_LOG(LogTemp, Warning, TEXT("Attack on Cooldown"))
			return; // Return if the attack is on cooldown
	}

	LastAttackTime = CurrentTime; // Set the last attack time to the current time

	HandleStamina(AttackStaminaCost);

	for (AMainSword* Sword : AttachedSwords)
	{
		Sword->SetCanDamage(true); // Can damage as long as succesful entry into attack
	}


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

		//PlayAnimMontage(BlockAnimation); // Play the block animation
		//HandleStamina(10.f); // Subtract 10 stamina for blocking will be used for parrying
		GetCharacterMovement()->MaxWalkSpeed = 200.f; // Set the max walk speed to 200

		GetWorldTimerManager().SetTimer(StaminaDrainTimer, this, &AGameInAMonthCharacter::DrainStamina, 1.f, true); // Set a timer to drain the stamina

	}


}

void AGameInAMonthCharacter::StopBlock()
{
	bIsBlocking = false; // Set the flag to false;
	GetCharacterMovement()->MaxWalkSpeed = 600.f; // Set the max walk speed to 600
	//StopAnimMontage(BlockAnimation); // Stop the block animation
	GetWorldTimerManager().ClearTimer(StaminaDrainTimer); // Clear the stamina drain timer
}

void AGameInAMonthCharacter::DrainStamina()
{
	UE_LOG(LogTemp, Warning, TEXT("Draining Stamina"))

	HandleStamina(BlockStaminaDrainRate); // Drain the stamina

	if (CurrentStamina <= 0)
	{
		StopBlock(); // Stop the block if the stamina is less than or equal to 0
	}

}

void AGameInAMonthCharacter::RegenStamina()
{

	CurrentStamina += StaminaRegenRate; // Regen by 2
	CurrentStamina = FMath::Clamp(CurrentStamina, 0, MaxStamina); // Clamp the stamina
	UE_LOG(LogTemp, Warning, TEXT("Stamina: %f"), CurrentStamina)

		if (CurrentStamina >= MaxStamina)
		{
			CurrentStamina = MaxStamina; // Set the stamina to the max stamina
		}


	if (CurrentStamina >=5)
	{
		bCanAttack = true; // Set the can attack flag to true
	}
}

void AGameInAMonthCharacter::RegenMana()
{

	CurrentMana += 5.f; // Regen by 5
	UE_LOG(LogTemplateCharacter, Log, TEXT("CurrentMana: %f"), CurrentMana);
	CurrentMana = FMath::Clamp(CurrentMana, 0.f, MaxMana); // Clamp the mana

	if (CurrentMana >= MaxMana)
	{
		CurrentMana = MaxMana; // Set the mana to the max mana
	}
}

void AGameInAMonthCharacter::HandleMana(float ManaCost)
{
		CurrentMana -= ManaCost; // Subtract the mana cost from the mana
		if (CurrentMana <= 0) // Check if the mana is less than or equal to 0
		{
			CurrentMana = 0; // Set the mana to 0
		}
}

void AGameInAMonthCharacter::ResetMageModeToggle()
{
	bCanToggleMageMode = true; // Set the flag to true
}



void AGameInAMonthCharacter::ExecuteTeleport()
{
	if (!bIsAbilityReady)
	
	{
		return;
	}

	CastTeleport(); // Cast the teleport

}

void AGameInAMonthCharacter::ExecuteAOE()
{
	if (!bIsAbilityReady)
	{
		return;
	}


	CastAOE(); // Cast the AOE

}

void AGameInAMonthCharacter::ExecuteManaBall()
{
	if (!bIsAbilityReady)
	{
		return;
	}

	CastManaBall(); // Cast the mana ball

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
		//UE_LOG(LogTemp, Warning, TEXT("Playing Attack Animation"))
	}


	float DamageCritIncrease = FMath::RandRange(0.f, 12.f); // Random number between 0 and 12 for crit damage

	float Damage = (BaseDamage + ComboCounter + DamageCritIncrease) * DamageMultiplier; // Calculate the damage

	for (AMainSword* Sword : AttachedSwords)
	{
		Sword->PerformAttack(Damage); // Set the damage of the sword
	}
}


void AGameInAMonthCharacter::ResetPlayer()
{
	CurrentHealth = MaxHealth; // Set the health to the max health
	CurrentStamina = MaxStamina; // Set the stamina to the max stamina
	CurrentMana = MaxMana; // Set the mana to the max mana
	ComboCounter = 0; // Reset the combo counter
	bIsBlocking = false; // Set the block flag to false
	bCanAttack = true; // Set the can attack flag to true
	bCanToggleMageMode = true; // Set the can toggle mage mode flag to true
	bIsAbilityReady = true; // Set the ability ready flag to true


}


/*
void AGameInAMonthCharacter::PlaySound(USoundCue* Sound)
{
	if (Sound)
	{
		MusicComponent->SetSound(Sound); // Set the sound
		MusicComponent->Play(); // Play the sound
	}
}
*/

void AGameInAMonthCharacter::ApplyPowerup()
{
}

void AGameInAMonthCharacter::HandleDeath()
{
	// Handle the death of the character

	AGameInAMonthGameMode* GameMode = Cast<AGameInAMonthGameMode>(GetWorld()->GetAuthGameMode());

	GameMode->GameLose(); // Call the game lose function in the game mode

	Destroy();
}


