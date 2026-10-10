// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "RuntimeStats.h"
#include "GameplayTags.h"
#include "Stat.h"
#include "ChosenUpgrade.h"
#include "GCUtilities.h"
#include "StatComponent.generated.h"


class UEntityDataAsset;

DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnStatChangedDelegate,
	FGameplayTag, Type,
	FRuntimeStats, StatValues);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStatChangedMulticast,
	FGameplayTag, Type,
	FRuntimeStats, StatValues);

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROTOTYPE1_API UStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UStatComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	


public:	
	
	// TODO:: Add delegate binding to init
	UFUNCTION(BlueprintCallable)
	void Initialize();
	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Stats")
	TObjectPtr<UEntityDataAsset> StatDataAsset{};
	
	// Stores mutable runtime values
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Stats")
	TMap<FGameplayTag, FRuntimeStats> RuntimeStats{};
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Upgrades")
	TMap<FGameplayTag, FStat> UpgradeableStats{};
	
	// Populated when getting random upgrade sets
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Upgrades")
	TMap<FGameplayTag, FChosenUpgrade> ChosenUpgradeSets{};
	
	// Mutable stat set, instances should not write to data asset
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Stats")
	TMap<FGameplayTag, FStat> FullStats;
	
	FOnStatChangedMulticast OnStatChanged;
	
	UFUNCTION(BlueprintCallable, Category = "Upgrades")
	void InitRandomUpgradeSets(TMap<FGameplayTag, FChosenUpgrade>& outMap, int32 count);
	
	UFUNCTION(BlueprintCallable, Category = "Stats")
	FStat GetDefaultStatSet(FGameplayTag tag, bool& successfullyFound);
	
	UFUNCTION(BlueprintCallable, Category = "Stats")
	FRuntimeStats SetRuntimeStatValue(FGameplayTag tag, float value, bool& successfullySet);
	
	UFUNCTION(BlueprintCallable, Category = "Stats")
	FRuntimeStats SetRuntimeMaxValue(FGameplayTag tag, float value, bool& successfullySet);
	
	/** 
	 * @param setCurrentFirst Sets current value first before max, if true current is clamped against the max before changing rather than the max after
	 * @param dontClampMax Runtime max may be separate from max in data asset, defines whether max should be clamped by that static max or not
	 */
	UFUNCTION(BlueprintCallable, Category = "Stats")
	FRuntimeStats SetRuntimeMaxAndCurrent(FGameplayTag tag, float currentVal, float maxVal, bool setCurrentFirst, bool dontClampMax, bool& successfullySet);
	
	/** 
	 * @param setCurrentFirst Sets current value first before min, if true current is clamped against the min before changing rather than the min after
	 * @param dontClampMin Runtime min may be separate from min in data asset, defines whether min should be clamped by that static min or not
	 */
	UFUNCTION(BlueprintCallable, Category = "Stats")
	FRuntimeStats SetRuntimeMinAndCurrent(FGameplayTag tag, float currentVal, float minVal, bool setCurrentFirst, bool dontClampMin, bool& successfullySet);
	
	UFUNCTION(BlueprintCallable, Category = "Stats")
	FRuntimeStats GetRuntimeStatSet(FGameplayTag tag, bool& successfullyFound);
	
	UFUNCTION(BlueprintCallable, Category = "Stats")
	FStat SetDefaultMax(FGameplayTag tag, float value, bool& successfullySet);
	
	UFUNCTION(BlueprintCallable, Category = "Stats")
	void ChangeStatsForMode(bool inSecondMode);
	
	// Utility function for blueprint functions to bind to on stat changed
	// TODO: Not needed since i now know of BlueprintAssignable but keeping here for previous usages
	UFUNCTION(BlueprintCallable, Category = "Util")
	void BindToOnStatChanged(FOnStatChangedDelegate onStatChanged);
	
	// Register external stat sets into this stat component, ie ability stats into parent
	UFUNCTION(BlueprintCallable, Category = "Stats")
	void RegisterExternalStats(TMap<FGameplayTag, FStat> externStatSet, bool& atLeastOneSuccessful);

};
