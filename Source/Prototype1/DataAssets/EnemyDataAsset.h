#pragma once

#include "CoreMinimal.h"
#include "EntityDataAsset.h"
#include "EnemyDataAsset.generated.h"

UCLASS(BlueprintType, Blueprintable)
class UEnemyDataAsset: public UEntityDataAsset
{
	GENERATED_BODY()
public:
	
	UEnemyDataAsset();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Defaults")
	TObjectPtr<UEnemyDataAsset> DefaultDataAsset;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	int32 CostToSpawn{1};
	
	// TODO: Can be added as a stat with gameplay tag
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Score")
	int32 ScoreOnDeath{};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Score")
	float ScoreMultiplierInSecondMode{2.0f};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Exp")
	float ExpDropMin{};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Exp")
	float ExpDropMax{};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status Effects")
	float MaxSlowEffectiveness{};
	
	// Should be [0,1], defines how much of slow is applied depending on the enemy's current slowed amount the enemy has
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status Effects")
	class UCurveFloat* SlowEffectivenessCurve{nullptr};
	
	UFUNCTION(CallInEditor)
	void UseDefaultDataAssetValues();
	
};
