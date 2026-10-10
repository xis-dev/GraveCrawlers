#pragma once


#include "Upgrade.h"
#include "Stat.generated.h"

// TODO: Decouple upgradeable structs, stats should care about just stats, add upgradeable flag + struct to EntityDataAsset
USTRUCT(BlueprintType)
struct FStat
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StartAmount{};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxAmount{};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MinAmount{};
	
	// Stat is reductive, the lower the stat the better
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsReductive{};
	
	// Stat has a dynamic max or min(if it is reductive), such as health with maxhp and current hp and ammo
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool HasDynamicCap{};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool AffectedInSecondMode{};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SecondModeMultiplier{1.0f};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Upgradeable{};
	
	/** 
	* Curve defining how the probability of this upgrade changes as its between min and max, 
	* Value in curve depends on if the parent stat is reductive or not, interpolation is calculated Max -> Min if owning stat is reductive
	* Must have one or probability is 0
	* TODO: Add default probability progression curve
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UCurveFloat> UpgradeProbabilityProgressionCurve{nullptr};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FUpgrade> UpgradeSets{};
};
