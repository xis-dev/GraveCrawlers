#include "EnemyDataAsset.h"

UEnemyDataAsset::UEnemyDataAsset()
{


	
	
}

void UEnemyDataAsset::UseDefaultDataAssetValues()
{
	if (DefaultDataAsset)
	{
		StatMap = DefaultDataAsset->StatMap;
		CostToSpawn = DefaultDataAsset->CostToSpawn;
		ScoreOnDeath = DefaultDataAsset->ScoreOnDeath;
	
		ExpDropMin = DefaultDataAsset->ExpDropMin;
		ExpDropMax = DefaultDataAsset->ExpDropMax;
	}
}