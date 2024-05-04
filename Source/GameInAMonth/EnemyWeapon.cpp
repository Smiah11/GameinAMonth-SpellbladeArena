// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyWeapon.h"
#include "EnemyAIController.h"
#include "Particles/ParticleSystemComponent.h"
#include <Kismet/GameplayStatics.h>

// Sets default values
AEnemyWeapon::AEnemyWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SwordMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SwordMesh"));
	RootComponent = SwordMesh;

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	BoxCollision->SetupAttachment(RootComponent);


	// Set up the collision 

	BoxCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision); // Set the collision to no collision anim notify will set it to query only******
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
void AEnemyWeapon::BeginPlay()
{
	Super::BeginPlay();

	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemyWeapon::OnWeaponOverlap);
	
}

// Called every frame
void AEnemyWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemyWeapon::PerformAttack(float Damage)
{
	Damage = NewDamage;
}

void AEnemyWeapon::OnWeaponOverlap(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	if (!bCanDamage || !OtherActor || OtherActor == GetOwner())
	{

		UE_LOG(LogTemp, Warning, TEXT("wEAPON AHHHH"));
		return;
	}


	UE_LOG(LogTemp, Warning, TEXT("OnWeaponOverlap"));

	APawn* OwnerPawn = Cast<APawn>(GetOwner()); // Get the pawn first so we can get the controller

	AEnemyAIController* OwnerController = Cast<AEnemyAIController>(OwnerPawn->GetController()); // Get the owner controller of the sword

	// Check if the owner controller is valid
	if (!OwnerController)
	{

		UE_LOG(LogTemp, Warning, TEXT("No Owner Controller"))
			return; // Return if the owner controller is not valid

	}

	//UE_LOG(LogTemp, Warning, TEXT("Hit %s"), *OtherActor->GetName());

	// BOX TRACE FOR DEBUGGING
	const FVector Start = TraceStart->GetComponentLocation();
	const FVector End = TraceEnd->GetComponentLocation();


	// Array of actors to ignore
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this); // Add the sword to the array of actors to ignore
	
	FHitResult BoxHit;
	// Box Trace for debugging
	//UKismetSystemLibrary::BoxTraceSingle(GetWorld(), Start, End, FVector(5, 5, 5), TraceStart->GetComponentRotation(), ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore, EDrawDebugTrace::ForDuration, BoxHit, true);


	if (OtherActor->ActorHasTag("Player"))
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit Player Character"));

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
	}

}

void AEnemyWeapon::SetCanDamage(bool CanDamage)
{
		bCanDamage = CanDamage;
}



