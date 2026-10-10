// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GCGameMode.generated.h"

class AEnemy;
class AAIManager;
class UWaveSystem;
/**
 * 
 */
UCLASS()
class PROTOTYPE1_API AGCGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	
	AGCGameMode();
	
protected:	
	// Extremely unreliable for some reason, gets called after AIManager's BeginPlay???
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UWaveSystem* WaveSystem;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AAIManager> AIManagerClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AAIManager* AIManager;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Game")
	void StartGame();
	
	UFUNCTION()
	void OnVerifiedEnemyKill(AEnemy* Enemy, AActor* Killer);
};
