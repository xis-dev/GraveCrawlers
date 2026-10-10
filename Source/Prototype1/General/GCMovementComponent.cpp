// Fill out your copyright notice in the Description page of Project Settings.


#include "GCMovementComponent.h"

#include "Enemy.h"
#include "GCCharacter.h"
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values for this component's properties
UGCMovementComponent::UGCMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	
	if (auto* character = Cast<AGCCharacter>(GetOwner()))
	{
		OwningCharacter = character;
	}
	
	FeetLocation = CreateDefaultSubobject<UArrowComponent>(TEXT("FeetLocation"));
	
	AvoidanceGroup.bGroup0 = true;
	GroupsToAvoid.Packed = 0xFFFFFFFF;
	GroupsToIgnore.Packed = 0;
	AvoidanceConsiderationRadius = 500.0f;
	
}


// Called when the game starts
void UGCMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	// ..
	
	
}


// Called every frame
void UGCMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                         FActorComponentTickFunction* ThisTickFunction)
{
	if (ShouldSkipUpdate(DeltaTime))
	{
		return;
	}
	
	// Tick back to movement component, we want to ignore FloatingPawnMovement's tick
	UMovementComponent::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!PawnOwner || !UpdatedComponent)
	{
		return;
	}

	const AController* Controller = PawnOwner->GetController();
	if (Controller && Controller->IsLocalController())
	{
		// apply input for local players but also for AI that's not following a navigation path at the moment
		if (Controller->IsLocalPlayerController() == true || Controller->IsFollowingAPath() == false || NavMovementProperties.bUseAccelerationForPaths)
		{
			ApplyControlInputToVelocity(DeltaTime);
		}
		// if it's not player controller, but we do have a controller, then it's AI
		// (that's not following a path) and we need to limit the speed
		else if (IsExceedingMaxSpeed(MaxSpeed) == true)
		{
			Velocity = Velocity.GetUnsafeNormal() * MaxSpeed;
		}

		LimitWorldBounds();
		bPositionCorrected = false;


		

		// Apply Gravity
		Velocity = FVector{Velocity.X, Velocity.Y, Velocity.Z + GetGravityZ()};
		// Move actor
		FVector Delta = Velocity * DeltaTime;
		

		if (!Delta.IsNearlyZero(1e-6f))
		{
			FVector VelocityNoGravity = Velocity;
			VelocityNoGravity.Z = 0.0f;
			if (VelocityNoGravity.SizeSquared() < UE_KINDA_SMALL_NUMBER)
			{
				VelocityNoGravity = GetForwardVector();
			}
			const FVector OldLocation = UpdatedComponent->GetComponentLocation();
			const FQuat Rotation = VelocityNoGravity.GetSafeNormal().Rotation().Quaternion();

			FHitResult Hit(1.f);
			SafeMoveUpdatedComponent(Delta, Rotation, true, Hit);

			if (Hit.IsValidBlockingHit())
			{
				HandleImpact(Hit, DeltaTime, Delta);
				// Try to slide the remaining distance along the surface.
				SlideAlongSurface(Delta, 1.f-Hit.Time, Hit.Normal, Hit, true);
			}

			// Update velocity
			// We don't want position changes to vastly reverse our direction (which can happen due to penetration fixups etc)
			if (!bPositionCorrected)
			{
				const FVector NewLocation = UpdatedComponent->GetComponentLocation();
				Velocity = ((NewLocation - OldLocation) / DeltaTime);
			}
		}
		
		// Finalize
		UpdateComponentVelocity();
	}
}

void UGCMovementComponent::PostInitProperties()
{
	Super::PostInitProperties();
	
	if (auto* owner = GetOwner()) {
		FeetLocation->SetupAttachment(owner->GetRootComponent());
	}
}


void UGCMovementComponent::SetRVOAvoidanceUID(int32 UID)
{
	AvoidanceUID = UID;
}

int32 UGCMovementComponent::GetRVOAvoidanceUID()
{
	return AvoidanceUID;
}

void UGCMovementComponent::SetRVOAvoidanceWeight(float Weight)
{
	AvoidanceWeight = Weight;
}

float UGCMovementComponent::GetRVOAvoidanceWeight()
{
	return AvoidanceWeight;
}

FVector UGCMovementComponent::GetRVOAvoidanceOrigin()
{
	return GetActorFeetLocation();
}

float UGCMovementComponent::GetRVOAvoidanceRadius()
{
	UCapsuleComponent* CapsuleComp = GetCharacterOwner()->GetCapsuleComponent();
	return CapsuleComp ? CapsuleComp->GetScaledCapsuleRadius() : 0.0f;
}

float UGCMovementComponent::GetRVOAvoidanceConsiderationRadius()
{
	return AvoidanceConsiderationRadius;
}

float UGCMovementComponent::GetRVOAvoidanceHeight()
{
	UCapsuleComponent* CapsuleComp = GetCharacterOwner()->GetCapsuleComponent();
	return CapsuleComp ? CapsuleComp->GetScaledCapsuleHalfHeight() : 0.0f;
}

FVector UGCMovementComponent::GetVelocityForRVOConsideration()
{
	return Velocity;
}

void UGCMovementComponent::SetAvoidanceGroupMask(int32 GroupFlags)
{
	AvoidanceGroup.SetFlagsDirectly(GroupFlags);
}

int32 UGCMovementComponent::GetAvoidanceGroupMask()
{
	return AvoidanceGroup.Packed;
}

void UGCMovementComponent::SetGroupsToAvoidMask(int32 GroupFlags)
{
	GroupsToAvoid.SetFlagsDirectly(GroupFlags);
}

int32 UGCMovementComponent::GetGroupsToAvoidMask()
{
	return GroupsToAvoid.Packed;
}

void UGCMovementComponent::SetGroupsToIgnoreMask(int32 GroupFlags)
{
	GroupsToIgnore.SetFlagsDirectly(GroupFlags);
}

int32 UGCMovementComponent::GetGroupsToIgnoreMask()
{
	return GroupsToIgnore.Packed;
}

FVector UGCMovementComponent::GetFeetLocation() const
{
	return FeetLocation->GetComponentLocation();
}

