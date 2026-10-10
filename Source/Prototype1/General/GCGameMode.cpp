// Fill out your copyright notice in the Description page of Project Settings.


#include "GCGameMode.h"

#include "AIManager.h"
#include "MainPlayer.h"
#include "WaveConfigDataAsset.h"
#include "WaveSystem.h"
#include "Kismet/GameplayStatics.h"

AGCGameMode::AGCGameMode()
{
	WaveSystem = CreateDefaultSubobject<UWaveSystem>(TEXT("Wave System Component"));
}

void AGCGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	AIManager = AIManagerClass ? Cast<AAIManager>(GetWorld()->SpawnActor(AIManagerClass)) : GetWorld()->SpawnActor<AAIManager>();
	if (!AIManager)
	{
		UE_LOG(LogTemp, Error, TEXT("Somehow could not spawn AIManager"));
		return;
	}
	
	FSpawnPointArrayWrapper wrappedSpawnPoints{};
	TArray<AActor*> spawnPointsAsActors;
	
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnPoint::StaticClass(), spawnPointsAsActors);
	
	for (auto* spawnPointActor: spawnPointsAsActors)
	{
		if (auto* spawnPoint = Cast<ASpawnPoint>(spawnPointActor))
		{
			wrappedSpawnPoints.SpawnPoints.Add(spawnPoint);
		}
	}
	
	AIManager->AddSpawnPoints(wrappedSpawnPoints);
	AIManager->OnEnemyVerifiedKill.AddUniqueDynamic(this, &AGCGameMode::OnVerifiedEnemyKill);
	
	// Defer start game to next frame, after every beginplay should be done
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &AGCGameMode::StartGame);
}


void AGCGameMode::StartGame_Implementation()
{
	// TODO: Get player config and set player base exp from here
	WaveSystem->Initialize(AIManager);
}

void AGCGameMode::OnVerifiedEnemyKill(AEnemy* Enemy, AActor* Killer)
{
	if (auto* playerCharacter = Cast<AMainPlayer>(Killer))
	{
		playerCharacter->AddKills(1);
		playerCharacter->AddScore(Enemy->GetScoreOnDeath());
	}
	
}

