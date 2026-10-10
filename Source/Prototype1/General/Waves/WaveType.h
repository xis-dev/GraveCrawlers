#pragma once

#include "WaveType.generated.h"

class AEnemy;
USTRUCT(BlueprintType, Blueprintable)
struct FWaveType
{
	GENERATED_BODY()
	
	// Wave threshold defining when this wave can start showing up
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 WaveThreshold;
	
	// Probability weight of this wave showing up every new wave
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float WaveProbabilityWeight;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<TSubclassOf<AEnemy>, float> EnemyTypesAndProbabilityInWave;
	
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float EnemyWeightsSum;
};
