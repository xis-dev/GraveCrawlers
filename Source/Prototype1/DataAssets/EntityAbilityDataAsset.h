// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Ability.h"

#include "EntityAbilityDataAsset.generated.h"


class UInputAction;

UCLASS()
class PROTOTYPE1_API UEntityAbilityDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<UInputAction*, TSubclassOf<UAbility>> Abilities; 
	
};
