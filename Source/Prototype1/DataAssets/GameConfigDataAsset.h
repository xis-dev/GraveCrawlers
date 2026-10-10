// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameConfigDataAsset.generated.h"

class UWaveConfigDataAsset;
struct FWaveType;
class AEnemy;
/**
 * Game Mode config
 */
UCLASS(BlueprintType, Blueprintable)
class PROTOTYPE1_API UGameConfigDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave Config")
	UWaveConfigDataAsset* WaveSystemConfig;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Config")
	float HealthLossRateInSecondMode;
	
};
