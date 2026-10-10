#pragma once

#include "Upgrade.generated.h"

// Upgrade struct for stats
USTRUCT(BlueprintType)
struct FUpgrade
{
	GENERATED_BODY()
	
	// This is here to make it more obvious, ideally should be in FStat but would need a map
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ProbabilityWeight{1.0};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MinAmount{};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxAmount{};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ChosenAmount{};
	
	// Flag for if this upgrade should set ChosenAmount to a random value between Min and Max or use pre-set ChosenAmount 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool ChooseRandomAmount{true};
	
	// Flag for if this upgrade defines ChosenAmount as a percentage or flat value
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool PercentageBased{};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText SuffixText{};
	

	

};
