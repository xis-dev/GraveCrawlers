// Fill out your copyright notice in the Description page of Project Settings.


#include "AIManager.h"

#include "Enemy.h"
#include "FlyingEnemy.h"
#include "NavigationSystem.h"
#include "GroundEnemy.h"
#include "Algo/RandomShuffle.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AAIManager::AAIManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
}

// Called when the game starts or when spawned
void AAIManager::BeginPlay()
{
	Super::BeginPlay();
	
	NavigationSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
}

// Called every frame
void AAIManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAIManager::AddSpawnPoints(const FSpawnPointArrayWrapper& WrappedSpawnPointArray)
{
	for (auto* spawnPoint: WrappedSpawnPointArray.SpawnPoints)
	{
		SpawnPointsByType.FindOrAdd(spawnPoint->Type).SpawnPoints.Add(spawnPoint);
	}
}

void AAIManager::Clear()
{
	for (auto* enemy: EnemiesAlive)
	{
		if (!enemy) continue;
		UClass* enemyType = enemy->GetClass();
		if (int* count = EnemyCountsByType.Find(enemyType); count && *count > 0)
		{
			--(*count);
		}
		enemy->Destroy();
	}
	
	EnemiesAlive.Empty();
	
}

AEnemy* AAIManager::SpawnEnemy(TSubclassOf<AEnemy> enemyType, int32 MaxSpawnAttempts)
{
	AEnemy* ReturnPtr = nullptr;
	TEnumAsByte<ESpawnPointType> SpawnPointType{Ground};
	if (enemyType->IsChildOf(AFlyingEnemy::StaticClass()))
	{
		SpawnPointType = Air;
	}

	FSpawnPointArrayWrapper* WrappedSpawnPoints = SpawnPointsByType.Find(SpawnPointType);
	if (!WrappedSpawnPoints) return ReturnPtr;
	
	TArray<ASpawnPoint*> AvailableSpawnPoints = WrappedSpawnPoints->SpawnPoints;
	// Shuffle to keep it random every spawn
	Algo::RandomShuffle(AvailableSpawnPoints);
	
	ASpawnPoint* ChosenPoint{nullptr};
	for (auto* SpawnPoint: AvailableSpawnPoints)
	{
		if (SpawnPoint->IsPointEnabled()) // Get first enabled point from shuffled array then break
		{
			ChosenPoint = SpawnPoint;
			break;
		}
	}
	if (!ChosenPoint) return ReturnPtr;
	
	
	int32 Iterations = 0;
	bool EnemyWasSpawned{false};
	
	FActorSpawnParameters SpawnParameters{};
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
	while (!EnemyWasSpawned && Iterations < MaxSpawnAttempts)
	{
		FVector LocationInSpawnRadius;
		// TODO: Add ground enemy c++ class for this
		if (enemyType->IsChildOf(AGroundEnemy::StaticClass()) && NavigationSystem)
		{
			FNavLocation NavLoc;
			if (NavigationSystem->GetRandomPointInNavigableRadius(ChosenPoint->GetActorLocation(), ChosenPoint->GetSpawnSpread(), NavLoc))
			{
				LocationInSpawnRadius = NavLoc.Location;
			}
		}
		else // TODO: Add singleton pattern to pathfinder then use ::Get() and get available location
		{
			LocationInSpawnRadius = ChosenPoint->GetActorLocation() + UKismetMathLibrary::RandomUnitVector() * 
																 UKismetMathLibrary::RandomFloatInRange(0.0f, 
																				 ChosenPoint->GetSpawnSpread());
		}
		FRotator Rotation = FRotator{0.0f};
		// For some reason location & rotation need to be pointers-to-const
		FVector* const LocPtr = &LocationInSpawnRadius;
		FRotator* const RotPtr = &Rotation;
		auto* SpawnedActor = GetWorld()->SpawnActor(enemyType, LocPtr, RotPtr, SpawnParameters);
		if (auto* SpawnedEnemy = Cast<AEnemy>(SpawnedActor))
		{
			EnemiesAlive.Add(SpawnedEnemy);
			SpawnedEnemy->OnEnemyKilled.AddUniqueDynamic(this, &AAIManager::OnEnemyKilled); // Bind OnEnemyKilled, AI Manager should both spawn and destroy
			SpawnedEnemy->SetTarget(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)); // Set target for ai
			++EnemyCountsByType.FindOrAdd(enemyType);
			EnemyWasSpawned = true;
			ReturnPtr = SpawnedEnemy;
			break;
		}
		
		++Iterations;
	}
	
	return ReturnPtr;
}

int32 AAIManager::GetEnemyCountByType(TSubclassOf<AEnemy> Type)
{
	return EnemyCountsByType.FindOrAdd(Type);
}

void AAIManager::OnEnemyKilled(AEnemy* Enemy, AActor* KillingActor)
{
	int32 numberOfRemovedEnemies = EnemiesAlive.Remove(Enemy);
	
	if (numberOfRemovedEnemies >= 1)
	{
		UClass* enemyType = Enemy->GetClass();
		if (int* count = EnemyCountsByType.Find(enemyType); count && *count > 0)
		{
			--(*count);
		}
		if (KillingActor) OnEnemyVerifiedKill.Broadcast(Enemy, KillingActor);
		Enemy->Destroy();
	}
}

void AAIManager::OnChangedToFirstMode()
{
	for (auto* enemy: EnemiesAlive)
	{
		enemy->ChangeStatsForFirstMode();
	}
}

void AAIManager::OnChangedToSecondMode()
{
	for (auto* enemy: EnemiesAlive)
	{
		enemy->ChangeStatsForSecondMode();
	}
}






