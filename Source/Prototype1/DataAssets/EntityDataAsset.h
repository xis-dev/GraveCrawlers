// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "GameplayTags.h"
#include "Stat.h"
#include "EntityDataAsset.generated.h"


/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class PROTOTYPE1_API UEntityDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FGameplayTag, FStat> StatMap;
};
