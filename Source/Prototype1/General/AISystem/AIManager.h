// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SpawnPoint.h"
#include "Enemy.h"
#include "GameFramework/Actor.h"
#include "AIManager.generated.h"

USTRUCT(BlueprintType)
struct FSpawnPointArrayWrapper
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<ASpawnPoint*> SpawnPoints;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEnemyVerifiedKill, AEnemy*, Enemy, AActor*, KillingActor);

class UNavigationSystemV1;
UCLASS()
class PROTOTYPE1_API AAIManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAIManager();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<TEnumAsByte<ESpawnPointType>, FSpawnPointArrayWrapper> SpawnPointsByType;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<AEnemy*> EnemiesAlive;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TMap<TSubclassOf<AEnemy>, int32> EnemyCountsByType{};

public:	

	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "Events")
	FOnEnemyVerifiedKill OnEnemyVerifiedKill;
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION(BlueprintCallable)
	void AddSpawnPoints(const FSpawnPointArrayWrapper& WrappedSpawnPointArray);
	
	UFUNCTION(BlueprintCallable)
	AEnemy* SpawnEnemy(TSubclassOf<AEnemy> enemyType, int32 MaxSpawnAttempts = 16);
	
	UFUNCTION(BlueprintCallable)
	int32 GetEnemyCountByType(TSubclassOf<AEnemy> Type);
	
	UFUNCTION(BlueprintCallable)
	void Clear();

protected:

	UPROPERTY(BlueprintReadOnly)
	UNavigationSystemV1* NavigationSystem;
	
	UFUNCTION()
	void OnEnemyKilled(AEnemy* Enemy, AActor* KillingActor);
	
	UFUNCTION(BlueprintCallable)
	void OnChangedToFirstMode();
	
	UFUNCTION(BlueprintCallable)
	void OnChangedToSecondMode();
	


};
