// Fill out your copyright notice in the Description page of Project Settings.


#include "StatComponent.h"

#include "Algo/RandomShuffle.h"
#include "DataAssets/EntityDataAsset.h"

// Sets default values for this component's properties
UStatComponent::UStatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UStatComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UStatComponent::Initialize()
{
	if (!StatDataAsset) return;
	FullStats = StatDataAsset->StatMap;

	for (const auto& [tag, statSet] : StatDataAsset->StatMap)
	{
		if (statSet.Upgradeable)
		{
			UpgradeableStats.Add(tag, statSet);
		}

		FRuntimeStats tempStats{};
		
		if (statSet.HasDynamicCap) // Set dynamic cap stats to start with start amount rather than default cap
		{
			if (statSet.IsReductive)
			{
				tempStats.CurrentValue = statSet.StartAmount;
				tempStats.MinValue = statSet.StartAmount;
				
				tempStats.MaxValue = statSet.MaxAmount;
			}
			else
			{
				tempStats.CurrentValue = statSet.StartAmount;
				tempStats.MaxValue = statSet.StartAmount;
				
				tempStats.MinValue = statSet.MinAmount;
			}
		}
		else
		{
			tempStats.CurrentValue = statSet.StartAmount;
			tempStats.MaxValue = statSet.MaxAmount;
			tempStats.MinValue = statSet.MinAmount;
		}


		RuntimeStats.Add(tag, tempStats);
		OnStatChanged.Broadcast(tag, tempStats);
	}
}


// Called every frame
void UStatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}



void UStatComponent::InitRandomUpgradeSets(TMap<FGameplayTag, FChosenUpgrade>& outMap, int32 count)
{
	if (UpgradeableStats.Num() <= 0) return; // Stat component has no upgradeable stats
	
	
	TArray<float> ProbabilityWeights{};
	float WeightSum = 0.;
	
	for (auto [tag, statSet]: UpgradeableStats)
	{
		bool success;
		FStat staticStatSet = GetDefaultStatSet(tag, success);
		if (!success) return;
		FRuntimeStats runtimeStatSet = GetRuntimeStatSet(tag, success);
		if (!success) return;
		 
		if (!statSet.UpgradeProbabilityProgressionCurve)
		{
			ProbabilityWeights.Add(0); // No probability curve, so no weight 
			continue;
		}
		
		float interpolationParam;
		if (statSet.IsReductive) // Stat is reductive, calculate interpolation as if it were Max -> Min
		{
			interpolationParam = UGCUtilities::GetInterpolationParameter
																(runtimeStatSet.CurrentValue, staticStatSet.StartAmount, staticStatSet.MinAmount);
		}
		else
		{
			interpolationParam = UGCUtilities::GetInterpolationParameter
													(runtimeStatSet.CurrentValue, staticStatSet.StartAmount, staticStatSet.MaxAmount);
		}
		
		if (interpolationParam > 0.9999f || FMath::IsNegativeOrNegativeZero(interpolationParam))
		{
			ProbabilityWeights.Add(0);
			continue;
		}; // Stat is either capped or in an invalid range
		
		float weight = statSet.UpgradeProbabilityProgressionCurve->GetFloatValue(interpolationParam);
		
		ProbabilityWeights.Add(weight);
		WeightSum += weight;
	}

	TArray<FGameplayTag> upgradeableStatTags;
	UpgradeableStats.GetKeys(upgradeableStatTags);
	
	if (upgradeableStatTags.Num() <= 0) return;
	
	// If caller wants more sets than upgradeable, randomly choose without caring about uniqueness
	bool chooseUniquelyRandomStats = count <= upgradeableStatTags.Num();
	TMap<FGameplayTag, FStat*> ChosenStatsToUpgrade;

	for (int32 i = 0; i < count; ++i)
	{
		int chosenWeightIdx = UGCUtilities::GetIndexBasedOnWeightsWithSum(ProbabilityWeights, WeightSum);
		if (chosenWeightIdx < 0) break;
		auto tag = upgradeableStatTags[chosenWeightIdx];
		if (FStat* statSet = UpgradeableStats.Find(tag))
		{
			ChosenStatsToUpgrade.Add(tag, statSet);
		}
		if (chooseUniquelyRandomStats)
		{
			WeightSum -= ProbabilityWeights[chosenWeightIdx];
			ProbabilityWeights[chosenWeightIdx] = 0; 
		} // Make weight 0 so it cant be chosen again
	}
	
	outMap.Empty(count);

	
	for (auto& [tag, statSet]: ChosenStatsToUpgrade)
	{
		TArray<float> UpgradeWeights{};
		float UpgradeWeightSum = 0.f;
		for (auto upgradeSet: statSet->UpgradeSets)
		{
			UpgradeWeights.Add(upgradeSet.ProbabilityWeight);
			UpgradeWeightSum += upgradeSet.ProbabilityWeight;
		}
		
		int32 chosenWeightIdx = UGCUtilities::GetIndexBasedOnWeightsWithSum(UpgradeWeights, UpgradeWeightSum);
		if (chosenWeightIdx <= -1) continue;
		const FUpgrade& ChosenUpgradeSet = statSet->UpgradeSets[chosenWeightIdx];
		
		FChosenUpgrade temp{};
		temp.IsReductive = statSet->IsReductive;
		temp.HasDynamicCap = statSet->HasDynamicCap;
		temp.SuffixText = ChosenUpgradeSet.SuffixText;
		temp.PercentageBased = ChosenUpgradeSet.PercentageBased;
		if (ChosenUpgradeSet.ChooseRandomAmount)
		{
			temp.ChosenAmount = FMath::RandRange(ChosenUpgradeSet.MinAmount, ChosenUpgradeSet.MaxAmount);
		}
		else
		{
			temp.ChosenAmount = ChosenUpgradeSet.ChosenAmount;
		}
				
		outMap.Add(tag, temp);
	}
}

FStat UStatComponent::GetDefaultStatSet(FGameplayTag tag, bool& successfullyFound)
{
	
	if (FStat* statSet = FullStats.Find(tag))
	{
		successfullyFound = true;
		return *statSet;		
	}
	successfullyFound = false;
	return FStat{};
}

FRuntimeStats UStatComponent::SetRuntimeStatValue(FGameplayTag tag, float value, bool& successfullySet)
{
	if (FRuntimeStats* statSet = RuntimeStats.Find(tag))
	{
		statSet->CurrentValue = FMath::Clamp(value, statSet->MinValue, statSet->MaxValue);
		
		successfullySet = true;
		OnStatChanged.Broadcast(tag, *statSet);
		return *statSet;
	}
	
	successfullySet = false;
	return FRuntimeStats{};
}

FRuntimeStats UStatComponent::SetRuntimeMaxValue(FGameplayTag tag, float value, bool& successfullySet)
{
	if (FRuntimeStats* statSet = RuntimeStats.Find(tag))
	{
		if (FStat* fullStatSet = FullStats.Find(tag))
		{
			statSet->MaxValue = FMath::Min(value, fullStatSet->MaxAmount);
			successfullySet = true;
			OnStatChanged.Broadcast(tag, *statSet);
			return *statSet;
		}
	}
	
	successfullySet = false;
	return FRuntimeStats{};
}

FRuntimeStats UStatComponent::SetRuntimeMaxAndCurrent(FGameplayTag tag, float currentVal, float maxVal, bool setCurrentFirst, bool dontClampMax,
	bool& successfullySet)
{
	float val = maxVal;
	if (FRuntimeStats* statSet = RuntimeStats.Find(tag))
	{
		if (FStat* fullStatSet = FullStats.Find(tag))
		{
			if (setCurrentFirst)
			{
				statSet->CurrentValue = FMath::Clamp(currentVal, statSet->MinValue, statSet->MaxValue);
				statSet->MaxValue = dontClampMax ? maxVal : FMath::Min(maxVal, fullStatSet->MaxAmount) ;

			}
			else
			{
				statSet->MaxValue = dontClampMax ? maxVal : FMath::Min(maxVal, fullStatSet->MaxAmount);
				statSet->CurrentValue = FMath::Clamp(currentVal, statSet->MinValue, statSet->MaxValue);	
			}
		OnStatChanged.Broadcast(tag, *statSet);
			successfullySet = true;
			return *statSet;
		}
	}
	
	successfullySet = false;
	return FRuntimeStats{};
}

FRuntimeStats UStatComponent::SetRuntimeMinAndCurrent(FGameplayTag tag, float currentVal, float minVal,
	bool setCurrentFirst, bool dontClampMin, bool& successfullySet)
{
	if (FRuntimeStats* statSet = RuntimeStats.Find(tag))
	{
		if (FStat* fullStatSet = FullStats.Find(tag))
		{
			if (setCurrentFirst)
			{
				statSet->CurrentValue = FMath::Clamp(currentVal, statSet->MinValue, statSet->MaxValue);
				statSet->MinValue = dontClampMin ? minVal : FMath::Max(minVal, fullStatSet->MinAmount) ;
			}
			else
			{
				statSet->MinValue = dontClampMin ? minVal : FMath::Max(minVal, fullStatSet->MinAmount);
				statSet->CurrentValue = FMath::Clamp(currentVal, statSet->MinValue, statSet->MaxValue);	
			}
			OnStatChanged.Broadcast(tag, *statSet);
			successfullySet = true;
			return *statSet;
		}
	}
	
	successfullySet = false;
	return FRuntimeStats{};
}

FRuntimeStats UStatComponent::GetRuntimeStatSet(FGameplayTag tag, bool& successfullyFound)
{
	if (FRuntimeStats* statSet = RuntimeStats.Find(tag))
	{
		successfullyFound = true;
		return *statSet;
	}
	
	successfullyFound = false;
	return FRuntimeStats{};
}

FStat UStatComponent::SetDefaultMax(FGameplayTag tag, float value, bool& successfullySet)
{
	if (FStat* statSet = FullStats.Find(tag))
	{
		statSet->MaxAmount = value;
		successfullySet = true;
		return *statSet;
	}
	
	successfullySet = false;
	return FStat{};
}

void UStatComponent::ChangeStatsForMode(bool inSecondMode)
{
	for (auto [tag, statSet] : FullStats)
	{	
		bool success{};
		if (statSet.AffectedInSecondMode)
		{
			if (inSecondMode)
			{
				SetDefaultMax(tag, statSet.MaxAmount * statSet.SecondModeMultiplier, success);
				if (FRuntimeStats* runtimeStat = RuntimeStats.Find(tag); runtimeStat && success)
				{
					SetRuntimeMaxAndCurrent(tag,  runtimeStat->CurrentValue * statSet.SecondModeMultiplier, 
													runtimeStat->MaxValue * statSet.SecondModeMultiplier, false, false, success);
				}
			}
			else
			{
				
				SetDefaultMax(tag, statSet.MaxAmount / statSet.SecondModeMultiplier, success);
				if (FRuntimeStats* runtimeStat = RuntimeStats.Find(tag); runtimeStat && success)
				{
					// Set current first as function is reductive
					// TODO: Consider better solution without relying on setting order, may need another container for first mode runtime stats
					SetRuntimeMaxAndCurrent(tag,  runtimeStat->CurrentValue / statSet.SecondModeMultiplier, 
													runtimeStat->MaxValue / statSet.SecondModeMultiplier, true, false, success);
				}
			}
		}

	}

}

void UStatComponent::BindToOnStatChanged(FOnStatChangedDelegate onStatChanged)
{
	OnStatChanged.AddUnique(onStatChanged);
}

void UStatComponent::RegisterExternalStats(TMap<FGameplayTag, FStat> externStatSet, bool& atLeastOneSuccessful)
{
	atLeastOneSuccessful = false;
	for (auto [tag, statSet] : externStatSet)
	{
		// Stats from data asset should have more prio, 
		// they wouldnt fall under the same group regardless though
		if (FullStats.Contains(tag)) continue;
		FullStats.Add(tag, statSet);
		if (statSet.Upgradeable)
		{
			UpgradeableStats.Add(tag, statSet);
		}
		FRuntimeStats tempStats{};
		tempStats.CurrentValue = statSet.StartAmount;
		tempStats.MaxValue = statSet.MaxAmount;
		tempStats.MinValue = statSet.MinAmount;

		RuntimeStats.Add(tag, tempStats);
		OnStatChanged.Broadcast(tag, tempStats);
		atLeastOneSuccessful = true;
	}
}





