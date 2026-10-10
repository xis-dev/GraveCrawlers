// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnPoint.generated.h"

UENUM(BlueprintType)
enum ESpawnPointType
{
	Ground,
	Air
};
class USphereComponent;

UCLASS()
class PROTOTYPE1_API ASpawnPoint : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASpawnPoint();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<ESpawnPointType> Type{Ground};
	



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void OnBeginDisableTriggerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
								int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	void OnEndDisableTriggerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	USphereComponent* DisableTrigger;
	
	// How varied is the spawn from the start position
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USphereComponent* SpawnSpreadRadius;
	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION(BlueprintCallable)
	float GetSpawnSpread();
	
	UFUNCTION(BlueprintCallable)
	bool IsPointEnabled();
	
private:
	
	UPROPERTY()
	bool IsAvailable{true};
	

};
