#pragma once

#include "ChosenUpgrade.generated.h"

// Struct to hold values when choosing upgrade, all variables are populated by stat or stat's upgradeable struct
USTRUCT(BlueprintType)
struct FChosenUpgrade
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float ChosenAmount{};
	
	// Chosen upgrade reduces the stat amount, fetched from stat IsReductive
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool IsReductive{};
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool HasDynamicCap{};
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool PercentageBased{};
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FText SuffixText{};	
};
