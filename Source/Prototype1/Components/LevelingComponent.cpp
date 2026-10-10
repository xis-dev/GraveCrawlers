#include "LevelingComponent.h"

ULevelingComponent::ULevelingComponent()
{
}

void ULevelingComponent::Initialize(float baseXp, float& outCurrentExp, float& outMaxExp, int& outLevel)
{
	PrimaryComponentTick.bCanEverTick = true;
	BaseExp = baseXp;
	IncrementingMaxExp = baseXp;
	CurrentExp = 0.0;
	MaxExp = baseXp;
	Level = 1;
	IncrementingLevel = Level;
	IncrementingSpeed = MaxExp / secondsToReachMaxExp;
	
	
	outCurrentExp = CurrentExp;
	outMaxExp = MaxExp;
	outLevel = Level;
}

void ULevelingComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
                                       FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (CanIncrementEXP)
	{
		if (TotalExpGained < TotalExpToGain)
		{	
			float remainingExpToGain = TotalExpToGain - TotalExpGained;
			float expGain = FMath::Min(DeltaTime * IncrementingSpeed, remainingExpToGain);
			
			TotalExpGained += expGain;
			IncrementingExpValue += expGain;
			
			while (IncrementingExpValue >= IncrementingMaxExp && CurrentMaxExpTargetIdx < TargetMaxEXPs.Num())
			{
				IncrementingExpValue -= IncrementingMaxExp;
				++IncrementingLevel;
				LevelChanged.Broadcast(IncrementingLevel);
				IncrementingMaxExp = TargetMaxEXPs[CurrentMaxExpTargetIdx];
				IncrementingSpeed = IncrementingMaxExp / secondsToReachMaxExp;
				++CurrentMaxExpTargetIdx;
			}
			ExpChanged.Broadcast(IncrementingExpValue, IncrementingMaxExp);
			return;
		}
		ExpChanged.Broadcast(CurrentExp, MaxExp);
		CanIncrementEXP = false;
		TargetMaxEXPs.Empty();
		TotalExpGained = 0.0;
		TotalExpToGain = 0.0;
		CurrentMaxExpTargetIdx = 0;
	}
	
}

float ULevelingComponent::GetExpByLevel(int32 currentLevel)
{
	return FMath::Pow(1.5, currentLevel - 1) * BaseExp;
}


void ULevelingComponent::TryGiveExp(float expAmount)
{
	ExpGiven.Broadcast(expAmount);

}

void ULevelingComponent::GiveExp(float expAmount)
{
	CurrentExp += expAmount;
	if (ExpGainIsIncremental)
	{
		CanIncrementEXP = true;
		TotalExpToGain += expAmount;
		while (CurrentExp >= MaxExp)
		{
			CurrentExp -= MaxExp;
			++Level;
			MaxExp = GetExpByLevel(Level);
			TargetMaxEXPs.Add(MaxExp);
		}
		return;
	}
	
	while (CurrentExp >= MaxExp)
	{
		CurrentExp -= MaxExp;
		++Level;
		LevelChanged.Broadcast(Level);
		MaxExp = GetExpByLevel(Level);
		ExpChanged.Broadcast(CurrentExp, MaxExp);
	}
	ExpChanged.Broadcast(CurrentExp, MaxExp);

}
