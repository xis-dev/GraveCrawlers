#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "LevelingComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLevelChanged, int32, NewLevel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnExpChanged, float, NewCurrentEXP, float, NewMaxEXP);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnExpGiven, float, ExpGiven);
UCLASS(BlueprintType, Blueprintable)
class PROTOTYPE1_API ULevelingComponent: public UActorComponent
{
	GENERATED_BODY()
public:
	
	ULevelingComponent();
	
	UFUNCTION(BlueprintCallable)
	void Initialize(float baseXp, float& outCurrentExp, float& outMaxExp, int& outLevel);
	
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
protected:

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ExpIncrementation")
	bool ExpGainIsIncremental;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ExpIncrementation")
	float secondsToReachMaxExp;
	
	UPROPERTY(VisibleAnywhere, Category = "ExpIncrementation")
	bool CanIncrementEXP;
	
	UPROPERTY(VisibleAnywhere, Category = "ExpIncrementation")
	TArray<float> TargetMaxEXPs;
	
	UPROPERTY(VisibleAnywhere, Category = "ExpIncrementation")
	int32 CurrentMaxExpTargetIdx;
	
	UPROPERTY(VisibleAnywhere, Category = "ExpIncrementation")
	float TotalExpGained;
	
	UPROPERTY(VisibleAnywhere, Category = "ExpIncrementation")
	float TotalExpToGain;
	
	UPROPERTY(VisibleAnywhere, Category = "ExpIncrementation")
	float IncrementingExpValue;
	
	UPROPERTY(VisibleAnywhere, Category = "ExpIncrementation")
	float IncrementingSpeed;
	
	UPROPERTY(VisibleAnywhere, Category = "ExpIncrementation")
	float IncrementingMaxExp;
	
	UPROPERTY(VisibleAnywhere, Category = "ExpIncrementation")
	int IncrementingLevel;
	
	UPROPERTY(BlueprintReadOnly)
	float ExpIncrementSpeed;
	
	
public:
	UPROPERTY(BlueprintReadWrite)
	float BaseExp;
	
	UPROPERTY(BlueprintReadWrite)
	float CurrentExp;
	
	UPROPERTY(BlueprintReadWrite)
	float MaxExp;
	
	UPROPERTY(BlueprintReadWrite)
	int Level{1};
	
	UPROPERTY(BlueprintAssignable)
	FOnLevelChanged LevelChanged;
	
	UPROPERTY(BlueprintAssignable)
    FOnExpChanged ExpChanged;
	
	UPROPERTY(BlueprintAssignable)
	FOnExpGiven ExpGiven;
	

	

	
	UFUNCTION(BlueprintCallable)
	float GetExpByLevel(int32 currentLevel);
	
	
	UFUNCTION(BlueprintCallable)
	void TryGiveExp(float expAmount);
	
	UFUNCTION(BlueprintCallable)
	void GiveExp(float expAmount);
};
