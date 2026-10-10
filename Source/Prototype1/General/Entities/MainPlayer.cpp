// Fill out your copyright notice in the Description page of Project Settings.


#include "MainPlayer.h"

#include "Components/AbilityUserComponent.h"
#include "Components/LevelingComponent.h"
#include "Components/StatComponent.h"

// Sets default values
AMainPlayer::AMainPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	AbilityComponent = CreateDefaultSubobject<UAbilityUserComponent>(TEXT("AbilityComponent"));
	
	StatComponent = CreateDefaultSubobject<UStatComponent>(TEXT("StatComponent"));
	
	LevelComponent = CreateDefaultSubobject<ULevelingComponent>(TEXT("LevelComponent"));
	LevelComponent->PrimaryComponentTick.bCanEverTick = true;
	
	MeshHolder = CreateDefaultSubobject<USceneComponent>(TEXT("MeshHolder"));
	MeshHolder->SetupAttachment(RootComponent);
	
	
	auto* mesh = GetMesh();
	if (!mesh) return;
	
	mesh->SetupAttachment(MeshHolder);

}

// Called when the game starts or when spawned
void AMainPlayer::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMainPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMainPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	UE_LOG(LogTemp, Warning, TEXT("Player input component set up"));
}

void AMainPlayer::UpgradeChosen(const TMap<FGameplayTag, FChosenUpgrade>& chosenSets, UStatComponent* statComp, int32 idx)
{
	if (idx > chosenSets.Num() - 1 || idx < 0) return;
	TArray<FGameplayTag> keys;
	TArray<FChosenUpgrade> values;
	
	chosenSets.GenerateKeyArray(keys);
	chosenSets.GenerateValueArray(values);
	
	if (keys.IsEmpty() || values.IsEmpty() ) return;

	float chosenAmount = values[idx].ChosenAmount;
	
	bool successfullyFoundStat;
	
	const FRuntimeStats runtimeStatSet = statComp->GetRuntimeStatSet(keys[idx], successfullyFoundStat);
	if (!successfullyFoundStat) return;
	
	const FStat& defaultStatSet = statComp->GetDefaultStatSet(keys[idx], successfullyFoundStat);
	if (!successfullyFoundStat) return;
	if (values[idx].HasDynamicCap) // Chosen stat has a dynamic cap, change runtime max/min depending on whether its reductive or not
	{
		if (values[idx].IsReductive) // Chosen stat is reductive, the min must be dynamic in this case, rather than max
		{
			float p = !values[idx].PercentageBased ? chosenAmount : 
			(chosenAmount / 100.0f) * defaultStatSet.MinAmount;
			statComp->SetRuntimeMinAndCurrent(keys[idx], p + runtimeStatSet.CurrentValue, runtimeStatSet.MinValue - p, false, false, successfullyFoundStat);
		}
		else
		{
			float p = !values[idx].PercentageBased ? chosenAmount : 
						(chosenAmount / 100.0f) * defaultStatSet.MaxAmount;
			statComp->SetRuntimeMaxAndCurrent(keys[idx], p + runtimeStatSet.CurrentValue, runtimeStatSet.MaxValue + p, false, false, successfullyFoundStat);
		}
		
	}
	else
	{
		float p = !values[idx].PercentageBased ? chosenAmount : 
				  values[idx].IsReductive     ?	(chosenAmount / 100.0f) * runtimeStatSet.MinValue :
												(chosenAmount / 100.0f) * runtimeStatSet.MaxValue;
		float newCurrentValue = values[idx].IsReductive ? runtimeStatSet.CurrentValue - p : // Choose new value based on reductive flag
														  runtimeStatSet.CurrentValue + p; 	
		statComp->SetRuntimeStatValue(keys[idx], newCurrentValue, successfullyFoundStat);	
		
	}
	
}



void AMainPlayer::AddKills(int32 Kills)
{
	KillCount += Kills;
	BP_OnKillsChanged(KillCount);
}

void AMainPlayer::AddScore(int32 ScoreToAdd)
{
	Score += ScoreToAdd;
	BP_OnScoreChanged(Score);
}

