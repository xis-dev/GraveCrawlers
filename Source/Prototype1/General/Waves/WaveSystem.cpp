// Fill out your copyright notice in the Description page of Project Settings.


#include "WaveSystem.h"

#include "AIManager.h"
#include "Enemy.h"
#include "GCUtilities.h"
#include "WaveConfigDataAsset.h"
#include "WaveType.h"


// Sets default values for this component's properties
UWaveSystem::UWaveSystem()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UWaveSystem::BeginPlay()
{
	Super::BeginPlay();
	
	
}


// Called every frame
void UWaveSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UWaveSystem::Initialize(AAIManager* Manager)
{
	if (!WaveSystemConfig)
	{
		UE_LOG(LogTemp, Error, TEXT("Wave system has no config."));
		return;
	}
	CurrentWave = 0;
	CurrentWaveBudget = NextWaveBudget = WaveSystemConfig->BaseWaveBudget;
	AIManager = Manager;
}

void UWaveSystem::PostInitProperties()
{
	Super::PostInitProperties();
	

}

void UWaveSystem::StartWave(int32 MaxFailedSpawns)
{
	if (!WaveSystemConfig) return;
	++CurrentWave;
	CurrentWaveBudget = NextWaveBudget;
	
	TArray<FWaveType> PossibleWaves;
	TArray<float> PossibleWavesWeights;
	
	// Get possible wave types that can be spawned based on starting thresholds
	for (const FWaveType& WaveType : WaveSystemConfig->WaveTypes)
	{
		if (WaveType.WaveThreshold < CurrentWave)
		{
			PossibleWaves.Add(WaveType);
			PossibleWavesWeights.Add(WaveType.WaveProbabilityWeight);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Found wave with higher threshold"));
		}
	}
		
	int32 ChosenWaveIndex = UGCUtilities::GetIndexBasedOnWeights(PossibleWavesWeights);
	if (ChosenWaveIndex == -1) return; // Invalid idx
		
	FWaveType ChosenWaveType = PossibleWaves[ChosenWaveIndex];
	
	TArray<float> PossibleEnemyWeightsByWave;
	TArray<TSubclassOf<AEnemy>> EnemyTypesArray;
	
	ChosenWaveType.EnemyTypesAndProbabilityInWave.GetKeys(EnemyTypesArray);
	if (EnemyTypesArray.IsEmpty()) return;
	ChosenWaveType.EnemyTypesAndProbabilityInWave.GenerateValueArray(PossibleEnemyWeightsByWave);
	
	float EnemyWeightsSum = ChosenWaveType.EnemyWeightsSum;

	
	bool AllEnemyCountsMaxed = false;
	int32 FailedIterations = 0;
	while (FailedIterations < MaxFailedSpawns && CurrentWaveBudget > 0)
	{
		TArray<float> TempEnemyWeightsForIteration = PossibleEnemyWeightsByWave;
		int32 ChosenEnemyIndex = UGCUtilities::GetIndexBasedOnWeightsWithSum(TempEnemyWeightsForIteration, EnemyWeightsSum);
		if (ChosenEnemyIndex == -1) return;
	
		TSubclassOf<AEnemy>& ChosenEnemyType = EnemyTypesArray[ChosenEnemyIndex];
		int32 CurrentEnemyCountByType = AIManager->GetEnemyCountByType(ChosenEnemyType);
		// TODO: Fix later, use find instead of brackets
		int32* maxConcurrent = WaveSystemConfig->MaxConcurrentEnemiesByType.Find(ChosenEnemyType);
		while (maxConcurrent && CurrentEnemyCountByType >= *maxConcurrent) // Allowed enemy count is maxed, try get another
		{
			EnemyWeightsSum -= TempEnemyWeightsForIteration[ChosenEnemyIndex]; // Reduce sum for next randomization
			TempEnemyWeightsForIteration[ChosenEnemyIndex] = 0.0;
			
			ChosenEnemyIndex = UGCUtilities::GetIndexBasedOnWeightsWithSum(TempEnemyWeightsForIteration, EnemyWeightsSum);
			if (ChosenEnemyIndex == -1) // All enemy counts are likely maxed, "skip" this wave
			{
				UE_LOG(LogTemp, Warning, TEXT("Invalid weights array, enemy counts are either maxed or data asset is ill formed"));
				return;
			};
			
			ChosenEnemyType = EnemyTypesArray[ChosenEnemyIndex];
			CurrentEnemyCountByType = AIManager->GetEnemyCountByType(ChosenEnemyType);
			maxConcurrent = WaveSystemConfig->MaxConcurrentEnemiesByType.Find(ChosenEnemyType);
		}
		
		if (auto* SpawnedEnemy = AIManager->SpawnEnemy(ChosenEnemyType))
		{
			// TODO: Move cost to wave system config and disable spawning for costs higher than wave budget
			CurrentWaveBudget -= SpawnedEnemy->GetCost();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Unable to spawn enemy from wave system"));
			++FailedIterations;
		}
	}
	
	EndWave();
}

void UWaveSystem::EndWave()
{
	if (!WaveSystemConfig) return;
	if (CurrentWave % WaveSystemConfig->WavesTillBudgetGrowth)
	{
		NextWaveBudget = WaveSystemConfig->GetNewWaveBudget(NextWaveBudget);
	}
}

