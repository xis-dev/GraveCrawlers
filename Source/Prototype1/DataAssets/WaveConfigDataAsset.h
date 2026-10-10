// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WaveType.h"
#include "WaveConfigDataAsset.generated.h"

/**
 * Wave system configuration
 */
UCLASS()
class PROTOTYPE1_API UWaveConfigDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave Config")
	float WaveInterval;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave Config")
	int32 WavesTillBudgetGrowth;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave Config")
	int32 BaseWaveBudget;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave Config")
	TMap<TSubclassOf<AEnemy>, int32> MaxConcurrentEnemiesByType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave Config")
	float SpawnSpreadRadius;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave Config")
	TArray<FWaveType> WaveTypes;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave Config")
	float WaveWeightSum;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	int32 GetNewWaveBudget(int32 CurrentBudget);
	
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	
#endif
};
