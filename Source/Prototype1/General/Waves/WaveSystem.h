// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WaveSystem.generated.h"

class AAIManager;
class UWaveConfigDataAsset;
// Wave system component, handles logic for waves
UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROTOTYPE1_API UWaveSystem : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UWaveSystem();

public:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	
							   FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION(BlueprintCallable)
	void Initialize(AAIManager* Manager);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	UWaveConfigDataAsset* WaveSystemConfig;
	
public:
	virtual void PostInitProperties() override;
	
protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	AAIManager* AIManager;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int32 CurrentWave = 0;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int32 CurrentWaveBudget;
	
	// TODO: May be temp, just for cleanliness
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int32 NextWaveBudget;
	
	/** 
	 * @param MaxFailedSpawns Little bit of an odd parameter, AI manager will attempt to spawn multiple times on its own, this is to block an infinite loop while decrementing budget
	 */
	UFUNCTION(BlueprintCallable)
	void StartWave(int32 MaxFailedSpawns = 32);
	
	// Split incase theres anything that needs to be done prior to ending the wave
	UFUNCTION(BlueprintCallable)
	void EndWave();
	
};
