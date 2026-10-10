#pragma once

#include "CoreMinimal.h"
#include "PathRequest.generated.h"

USTRUCT(BlueprintType)
struct FPath
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FVector> PositionsToFollow{};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsValid{};
};

class UGC3DPathFindingAgent;
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnPathRequestResolved, const FPath&, Path);

USTRUCT(BlueprintType)
struct FPathRequest
{
	GENERATED_BODY()
	
	FVector PathStart;
	FVector PathEnd;
	bool IsInvalid{false};
	FOnPathRequestResolved PathRequestResolved;
	
	void Invalidate() {IsInvalid = true;};
};


