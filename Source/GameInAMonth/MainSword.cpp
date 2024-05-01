// Fill out your copyright notice in the Description page of Project Settings.


#include "MainSword.h"
#include "GameFramework/Character.h"
#include "GameInAMonthCharacter.h"
#include <Kismet/GameplayStatics.h>

// Sets default values
AMainSword::AMainSword()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	SwordMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SwordMesh"));
	RootComponent = SwordMesh;

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	BoxCollision->SetupAttachment(RootComponent);


	// Set up the collision 

	BoxCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//BoxCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoxCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	BoxCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	BoxCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	TraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("TraceStart"));
	TraceStart->SetupAttachment(RootComponent);

	TraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("TraceEnd"));
	TraceEnd->SetupAttachment(RootComponent);


	HitParticleComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("HitParticleComponent"));
	HitParticleComponent->SetupAttachment(RootComponent);


	NewDamage = BaseDamage;

}

// Called when the game starts or when spawned
void AMainSword::BeginPlay()
{
	Super::BeginPlay();

	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &AMainSword::OnSwordOverlap);

}

// Called every frame
void AMainSword::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void AMainSword::PerformAttack(float Damage)
{
	NewDamage = Damage;
}

void AMainSword::SetCanDamage(bool CanDamage)
{
	bCanDamage = CanDamage;
}

void AMainSword::OnSwordOverlap(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//UE_LOG(LogTemp, Warning, TEXT("oVERLAPPING"))

	if (!bCanDamage || !OtherActor || OtherActor == GetOwner())
	{

		UE_LOG(LogTemp, Warning, TEXT("Early Return"));
		return;
	}

	AController* OwnerController = UGameplayStatics::GetPlayerController(GetWorld(), 0); // Get the player controller

	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0); // Get the player character

	AGameInAMonthCharacter* Player = Cast<AGameInAMonthCharacter>(PlayerCharacter); // Cast the player character to the player character class



	// Check if the owner controller is valid
	if (!OwnerController)
	{

		UE_LOG(LogTemp, Warning, TEXT("No Owner Controller"))
			return; // Return if the owner controller is not valid

	}

	if (OtherActor == PlayerCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit Player"))
			return;
	}

	//UE_LOG(LogTemp, Warning, TEXT("Hit %s"), *OtherActor->GetName());

	// BOX TRACE FOR DEBUGGING
	const FVector Start = TraceStart->GetComponentLocation();
	const FVector End = TraceEnd->GetComponentLocation();


	// Array of actors to ignore
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this); // Add the sword to the array of actors to ignore
	ActorsToIgnore.Add(PlayerCharacter); // Add the player character to the array of actors to ignore had to cast to AActor because of the array


	//ActorsToIgnore.Add(Cast<AActor>(PlayerCharacter)); // Add the player character to the array of actors to ignore had to cast to AActor because of the array
	FHitResult BoxHit;

		// Box Trace for debugging
		UKismetSystemLibrary::BoxTraceSingle(GetWorld(), Start, End, FVector(5, 5, 5), TraceStart->GetComponentRotation(), ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore, EDrawDebugTrace::ForDuration, BoxHit, true);

		// Apply damage to the actor
		UGameplayStatics::ApplyDamage(OtherActor, NewDamage, OwnerController, this, UDamageType::StaticClass());

		bCanDamage = false;

		if (HitParticleComponent)
		{
			HitParticleComponent->Activate();
		}

		if (HitSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, HitSound, GetActorLocation());
		}
	


	// HitSound



}



