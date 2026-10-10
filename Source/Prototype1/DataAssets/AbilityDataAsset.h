// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"


#include "EntityDataAsset.h"
#include "AbilityDataAsset.generated.h"

struct FGameplayTag;

/**
 * 
 */
UCLASS()
class PROTOTYPE1_API UAbilityDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
	public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UEntityDataAsset> StatDataAsset;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool AbilityNeedsConfirmation;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Categories = "Stats.Ability"))
	FGameplayTag AbilityStatTag;
	
	
	
	
};
