// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "GameplayTags.h"
#include "Stat.h"
#include "Components/StatComponent.h"
#include "MainPlayer.generated.h"

class UAbilityUserComponent;
class ULevelingComponent;
class UStatComponent;

UCLASS()
class PROTOTYPE1_API AMainPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMainPlayer();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAbilityUserComponent> AbilityComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UStatComponent> StatComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USceneComponent> MeshHolder;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<ULevelingComponent> LevelComponent;

protected:
	
	// TODO: Feels kinda weird here, might move to player state but not really worth it tbh
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int32 KillCount{};
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int32 Score{};

public:	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	UFUNCTION(BlueprintCallable, Category = "Upgrades")
	void UpgradeChosen(const TMap<FGameplayTag, FChosenUpgrade>& chosenSets, UStatComponent* statComp, int32 idx);
	
		
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Stats", meta  = (DisplayName = "On Kills Changed"))
	void BP_OnKillsChanged(int32 Kills);
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Stats", meta  = (DisplayName = "On Score Changed"))
    void BP_OnScoreChanged(int32 NewScore);
	
	UFUNCTION(BlueprintCallable, Category = "Stats")
	void AddKills(int32 Kills);
	
	UFUNCTION(BlueprintCallable, Category = "Stats")
	void AddScore(int32 ScoreToAdd);
};
