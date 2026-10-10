// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ShooterProjectile.h"
#include "FlyingEnemyProjectile.generated.h"

UCLASS()
class PROTOTYPE1_API AFlyingEnemyProjectile : public AShooterProjectile
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AFlyingEnemyProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	
	virtual void ExplosionCheck(const FVector& ExplosionCenter) override;
	
	virtual void ProcessHit(AActor* HitActor, UPrimitiveComponent* HitComp, const FVector& HitLocation, const FVector& HitDirection) override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION(BlueprintCallable)
	void Initialize(float Damage);
};
