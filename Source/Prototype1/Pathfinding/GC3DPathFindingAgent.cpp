#include "GC3DPathFindingAgent.h"

#include "GC3DPathFinder.h"
#include "GCDefaultMacros.h"
#include "GCUtilities.h"
#include "Prototype1.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"


UGC3DPathFindingAgent::UGC3DPathFindingAgent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.0167f;
}

void UGC3DPathFindingAgent::PostInitProperties()
{
	Super::PostInitProperties();
	
}

void UGC3DPathFindingAgent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UGC3DPathFindingAgent::TickComponent(float DeltaTime, enum ELevelTick TickType,
                                          FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (!MovementEnabled) return;
	AActor* owner = GetOwner();
	if (!owner) return;

	switch (MovementMode)
	{
	case FollowingPath: TickFollowPath(DeltaTime, owner); break;
	case None:
	default: break;
	}
	
}

void UGC3DPathFindingAgent::MoveTo(AActor* ActorToMoveTo, float AcceptanceRad, bool ContinuousMovement, bool ForceMove)
{
	if (!MovementEnabled) return;
 
	Target = ActorToMoveTo;
	PreviousTargetPos = FVector{MAX_FLT};
	AcceptanceRadius = AcceptanceRad;
	AcceptanceRadiusSquared = AcceptanceRadius * AcceptanceRadius;
	ContinueTrackingTarget = ContinuousMovement;
	MovementMode = FollowingPath;
	ForceMovement = ForceMove;
 
	if (ContinueTrackingTarget)
	{
		// Timers to continuously try getting path to target
		if (PathRequestTimerHandle.IsValid()) GetWorld()->GetTimerManager().ClearTimer(PathRequestTimerHandle);
		const float randomStartDelay = FMath::FRandRange(0.0f, 0.5f); // Slightly delay requests so multiple enemies dont call on same frame
		GetWorld()->GetTimerManager().SetTimer(PathRequestTimerHandle, this, &UGC3DPathFindingAgent::TryRequestPathToTarget, 0.5f, true, randomStartDelay);
	}
	TryRequestPathToTarget();
}

void UGC3DPathFindingAgent::MoveToLocation(FVector Position, float AcceptanceRad)
{
	if (!MovementEnabled)
	{
		return;
	};
 
	MovementMode = FollowingPath;

	FVector clampedPosition = Position;
	if (ParentPathFinder)
	{
		clampedPosition = ParentPathFinder->ClampPositionToGrid(Position);
		if (Position.IsNearlyZero() || Position.Equals(FVector{-53.3f, 0.0f, 0.0f}, 5.0f))
		{
			UE_LOG(LogTemp, Warning, TEXT("Nearly zero clamp position when trying to move"));
		}
	}
 
	RequestPath(GetOwner()->GetActorLocation(), clampedPosition);
 
	AcceptanceRadius = AcceptanceRad;
	AcceptanceRadiusSquared = AcceptanceRadius * AcceptanceRadius;
}

void UGC3DPathFindingAgent::StopMovement()
{
	if (CurrentPathRequest) CurrentPathRequest->Invalidate();
	if (PathRequestTimerHandle.IsValid()) GetWorld()->GetTimerManager().ClearTimer(PathRequestTimerHandle);

	CurrentPath = FPath{};
	CurrentPathIndex = 0;
	PreviousTargetPos = FVector{MAX_FLT};
	ContinueTrackingTarget = false;
	IsAdjusting = false;
	IsLocallyAdjusting = false;
	MovementMode = None;
	ForceMovement = false;
}

void UGC3DPathFindingAgent::PerformLocalAdjustment()
{
	FRotator properlyAlignedAxes = UKismetMathLibrary::MakeRotFromX(-LastMoveProperties.LastMoveDirection);
	StartAdjustment(GetOwner()->GetActorLocation(), -LastMoveProperties.LastMoveDirection, UKismetMathLibrary::GetUpVector(properlyAlignedAxes), 
					FMath::RandRange(LocalAdjustmentConeAngleRange.X, LocalAdjustmentConeAngleRange.Y), 
					LocalAdjustmentDistanceRange.X, LocalAdjustmentDistanceRange.Y);
	IsLocallyAdjusting = true;
	ForceMovement = true;	
	
}

void UGC3DPathFindingAgent::StartAdjustment(FVector TargetLoc, FVector PreferredDirection, FVector UpVector, 
											float ConeAngle, float MinDist, float MaxDist,float PreferredDirectionWeight)
{
		
	if (!ParentPathFinder) return;

	FVector dir = PreferredDirection;
	dir.Z = 0.0f;
	dir = dir.GetSafeNormal();

	dir = UKismetMathLibrary::RandomBoolWithWeight(PreferredDirectionWeight) ? dir : -dir;

	FVector validPoint;
	if (ParentPathFinder->GetBestAvailablePositionInCirclesWithCones(TargetLoc, dir, UpVector,
												  MinDist, MaxDist, ConeAngle, ConeAngle, validPoint))
	{
		MoveToLocation(validPoint, AcceptanceRadius);
		IsAdjusting = true;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Tried to adjust but could not"));
	}
}


void UGC3DPathFindingAgent::StopManualAdjustment()
{
	IsAdjusting = false;
}

bool UGC3DPathFindingAgent::AgentIsAdjusting()
{
	return IsAdjusting;
}

bool UGC3DPathFindingAgent::AgentIsLocallyAdjusting()
{
	return IsLocallyAdjusting;
}

void UGC3DPathFindingAgent::OnPathRequestCompleted(const FPath& Path)
{
	CurrentPath = Path;
	CurrentPathIndex = 0;
	CurrentPathRequest = nullptr;
}

void UGC3DPathFindingAgent::RequestPath(const FVector& start, const FVector& end)
{
	if (ParentPathFinder)
	{
		CurrentPathRequest = ParentPathFinder->RequestPath(start, end, this, "OnPathRequestCompleted");
	}
}

void UGC3DPathFindingAgent::TryRequestPathToTarget()
{
	if (!Target) return;
	FVector targetLoc = Target->GetActorLocation();
	targetLoc.Z += PreferredHeightOffset;
	
	if (!ContinueTrackingTarget)
	{
		if (CurrentPathRequest)
		{
			// Invalidate our previous request so we can stop it
			CurrentPathRequest->Invalidate();
		}
		RequestPath(GetOwner()->GetActorLocation(), targetLoc);
	}
	else
	{
		if (FVector::DistSquared(PreviousTargetPos, targetLoc) > AcceptanceRadiusSquared) // Only retry path request if new location is out of acceptable range
		{
			if (CurrentPathRequest)
			{
				// Invalidate our previous request so we can stop it
				CurrentPathRequest->Invalidate();
			}
			PreviousTargetPos = targetLoc;
			RequestPath(GetOwner()->GetActorLocation(), targetLoc);
		}
	}
}

bool UGC3DPathFindingAgent::HasMovingPath()
{
	return CurrentPath.IsValid && CurrentPathIndex < CurrentPath.PositionsToFollow.Num();
}

bool UGC3DPathFindingAgent::HasNoPath()
{
	return !CurrentPath.IsValid || CurrentPathIndex >= CurrentPath.PositionsToFollow.Num();
}

void UGC3DPathFindingAgent::EnableMovement()
{
	MovementEnabled = true;
}

void UGC3DPathFindingAgent::DisableMovement()
{
	MovementEnabled = false;
}

bool UGC3DPathFindingAgent::IsMovementEnabled()
{
	return MovementEnabled;
}

void UGC3DPathFindingAgent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
	Super::OnComponentDestroyed(bDestroyingHierarchy);
	
	if (CurrentPathRequest)
	{
		CurrentPathRequest->Invalidate();
	}
}

void UGC3DPathFindingAgent::TickFollowPath(float DeltaTime, AActor* Owner)
{

	TRACE_CPUPROFILER_EVENT_SCOPE(PathfindingAgentTick);
	if (!CurrentPath.IsValid || CurrentPathIndex >= CurrentPath.PositionsToFollow.Num())
	{
		IsAdjusting = false;
		IsLocallyAdjusting = false;
		return;
	};
 
	FVector currentPos = Owner->GetActorLocation();
	FVector targetPos = CurrentPath.PositionsToFollow[CurrentPathIndex];
	FVector moveDir = (targetPos - currentPos);
	float dist = moveDir.Length();
	float distToMovePerFrame = Speed * DeltaTime;
	
	FHitResult Hit;
	FVector newLocation;
 
	if (dist < distToMovePerFrame) // Closer to target(node not main target) than step size
	{
		newLocation = targetPos;
		++CurrentPathIndex;
	}
	else if (dist < AcceptanceRadius) // Continue as if we were at the position
	{
		++CurrentPathIndex;
		return;
	}
	else
	{
		newLocation = currentPos + (moveDir.GetSafeNormal() * distToMovePerFrame);
	}
	
	if (!ForceMovement && !IsLocallyAdjusting)
	{
		TRACE_CPUPROFILER_EVENT_SCOPE(GCAgentPathAvoidance);
		FCollisionQueryParams CollisionQueryParams = FCollisionQueryParams::DefaultQueryParam;
		CollisionQueryParams.AddIgnoredActor(Owner);
		FVector normalizedMoveDir = moveDir.GetSafeNormal();
		FVector lookAheadLocation = currentPos + (normalizedMoveDir * distToMovePerFrame * LookAheadDistance);
		if (GetWorld()->LineTraceSingleByChannel(Hit, currentPos, lookAheadLocation, GC_TRACE_ENEMYAVOIDANCE, CollisionQueryParams))
		{
			// Reset path as if it were done if we it something, locally adjust after
			StopMovement();
			LastMoveProperties.LastMoveWasBlocked = true;
			LastMoveProperties.LastMoveDirection = moveDir.GetSafeNormal();
			PerformLocalAdjustment();
			
     		
			return;
		}
	}
	
	if (CurrentPathIndex >= CurrentPath.PositionsToFollow.Num())
	{
		// Just explicitly setting to false so we know adjustment is done or at least it is not adjusting
		IsAdjusting = false;
		if (IsLocallyAdjusting)
		{
			ForceMovement = false;
		}
		IsLocallyAdjusting = false;
	}

	Owner->SetActorLocation(newLocation);
	
	
	
}
