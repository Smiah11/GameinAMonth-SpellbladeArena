// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystem.h"
#include "EnemyWeapon.generated.h"

UCLASS()
class GAMEINAMONTH_API AEnemyWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemyWeapon();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties")
	UStaticMeshComponent* SwordMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties")
	float BaseDamage = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties")
	float NewDamage;


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon Properties")
	UBoxComponent* BoxCollision;

	void PerformAttack(float Damage); // assign new damage and other stats


	UFUNCTION(BlueprintCallable, Category = "Sword")
	void OnWeaponOverlap(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);



	UFUNCTION(BlueprintCallable)
	void SetCanDamage(bool CanDamage);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties")
	USoundCue* HitSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties")
	UParticleSystemComponent* HitParticleComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties")
	UParticleSystem* HitParticle;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


private:


	bool bCanDamage = true; // false  to prevent multiple hits
	// USED FOR BOX TRACE

	UPROPERTY(VisibleAnywhere)
	USceneComponent* TraceStart;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* TraceEnd;

};
