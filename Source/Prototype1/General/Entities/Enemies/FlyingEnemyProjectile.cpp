// Fill out your copyright notice in the Description page of Project Settings.


#include "FlyingEnemyProjectile.h"

#include "GCDefaultMacros.h"
#include "HealthComponent.h"
#include "PhysicsEngine/PhysicsSettings.h"


// Sets default values
AFlyingEnemyProjectile::AFlyingEnemyProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	bExplodeOnHit = true;
	bDamageOwner = true;
}

// Called when the game starts or when spawned
void AFlyingEnemyProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}



void AFlyingEnemyProjectile::ExplosionCheck(const FVector& ExplosionCenter)
{
	// do a sphere overlap check look for nearby actors to damage
	TArray<FOverlapResult> Overlaps;

	FCollisionShape OverlapShape;
	OverlapShape.SetSphere(ExplosionRadius);

	FCollisionObjectQueryParams ObjectParams;
	ObjectParams.AddObjectTypesToQuery(ECC_Pawn);
	ObjectParams.AddObjectTypesToQuery(GC_OBJECT_TYPE_ENEMY);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	if (!bDamageOwner)
	{
		QueryParams.AddIgnoredActor(GetInstigator());
	}

	GetWorld()->OverlapMultiByObjectType(Overlaps, ExplosionCenter, FQuat::Identity, ObjectParams, OverlapShape, QueryParams);

	TArray<AActor*> DamagedActors;

	// process the overlap results
	for (const FOverlapResult& CurrentOverlap : Overlaps)
	{
		// overlaps may return the same actor multiple times per each component overlapped
		// ensure we only damage each actor once by adding it to a damaged list
		if (DamagedActors.Find(CurrentOverlap.GetActor()) == INDEX_NONE)
		{
			DamagedActors.Add(CurrentOverlap.GetActor());
			// apply physics force away from the explosion
			if (CurrentOverlap.GetActor())
			{
				const FVector& ExplosionDir = CurrentOverlap.GetActor()->GetActorLocation() - GetActorLocation();
				// push and/or damage the overlapped actor
				ProcessHit(CurrentOverlap.GetActor(), CurrentOverlap.GetComponent(), GetActorLocation(), ExplosionDir.GetSafeNormal());
			}


		}
			
	}
	
	
}

void AFlyingEnemyProjectile::ProcessHit(AActor* HitActor, UPrimitiveComponent* HitComp, const FVector& HitLocation,
	const FVector& HitDirection)
{
	// have we hit a character?
	
	if (auto* HealthComponent = HitActor->GetComponentByClass<UHealthComponent>())
	{
		HealthComponent->DealDamage(HitDamage);
	}
	
}

// Called every frame
void AFlyingEnemyProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AFlyingEnemyProjectile::Initialize(float Damage)
{
	HitDamage = Damage;
}

