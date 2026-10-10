// Fill out your copyright notice in the Description page of Project Settings.

#include "WaveConfigDataAsset.h"

#include "Waves/WaveType.h"


int32 UWaveConfigDataAsset::GetNewWaveBudget_Implementation(int32 CurrentBudget)
{
	return FMath::FloorToInt32(CurrentBudget * 1.5);
}

#if WITH_EDITOR
void UWaveConfigDataAsset::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	WaveWeightSum = 0.0;
	for (FWaveType& waveType: WaveTypes)
	{
		WaveWeightSum += waveType.WaveProbabilityWeight;
		waveType.EnemyWeightsSum = 0.0;
		for (auto& [enemyType, weight]: waveType.EnemyTypesAndProbabilityInWave)
		{
			waveType.EnemyWeightsSum += weight;
		}
	}
}
#endif
