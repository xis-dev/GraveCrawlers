#pragma once

#include "RuntimeStats.generated.h"

USTRUCT(BlueprintType)
struct FRuntimeStats
{
	GENERATED_BODY()	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CurrentValue{};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MinValue{};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxValue{};
	
};
