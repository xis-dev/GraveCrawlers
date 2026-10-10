// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnPoint.h"

#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ASpawnPoint::ASpawnPoint()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	DisableTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("Disable Trigger"));
	DisableTrigger->SetupAttachment(RootComponent);
	
	DisableTrigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	DisableTrigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	
	DisableTrigger->OnComponentBeginOverlap.AddUniqueDynamic(this, &ASpawnPoint::OnBeginDisableTriggerOverlap);
	DisableTrigger->OnComponentEndOverlap.AddUniqueDynamic(this, &ASpawnPoint::OnEndDisableTriggerOverlap);
	

	
	SpawnSpreadRadius = CreateDefaultSubobject<USphereComponent>(TEXT("Spawn Spread"));
	SpawnSpreadRadius->SetupAttachment(RootComponent);
	
	SpawnSpreadRadius->SetSphereRadius(100.0f);
	SpawnSpreadRadius->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SpawnSpreadRadius->SetCollisionResponseToAllChannels(ECR_Ignore);
}

bool ASpawnPoint::IsPointEnabled()
{
	return IsAvailable;
}

// Called when the game starts or when spawned
void ASpawnPoint::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASpawnPoint::OnBeginDisableTriggerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == UGameplayStatics::GetPlayerPawn(GetWorld(), 0)) IsAvailable = false;
}

void ASpawnPoint::OnEndDisableTriggerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == UGameplayStatics::GetPlayerPawn(GetWorld(), 0)) IsAvailable = true;
}

// Called every frame
void ASpawnPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

float ASpawnPoint::GetSpawnSpread()
{
	return SpawnSpreadRadius->GetScaledSphereRadius();
}

