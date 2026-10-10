// Fill out your copyright notice in the Description page of Project Settings.


#include "FlyingEnemy.h"

#include "GC3DPathFinder.h"
#include "GC3DPathFindingAgent.h"
#include "GCMovementComponent.h"
#include "Components/ArrowComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values
AFlyingEnemy::AFlyingEnemy()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	PathAgentComponent = CreateDefaultSubobject<UGC3DPathFindingAgent>(TEXT("Path Agent Component"));
	
	AttackSpawnPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("Attack Point"));
	AttackSpawnPoint->SetupAttachment(GetMesh());
	
	GetGCCharacterMovement()->Deactivate(); 
}

// Called when the game starts or when spawned
void AFlyingEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	if (PathAgentComponent)
	{
		PathAgentComponent->AddTickPrerequisiteActor(this); // Agent handles movement so state tick should wait for the movement for that frame to actually be done
	}
	
	
}

// Called every frame
void AFlyingEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (!TargetPlayer) return;
	
	FVector desiredTargetPos = TargetPlayer->GetActorLocation();
	desiredTargetPos.Z += PathAgentComponent->PreferredHeightOffset;
	
	switch (CurrentState)
	{
	case Flying:
		{
			float distSq = FVector::DistSquared(GetActorLocation(), desiredTargetPos);
			if (distSq < StrafeDistanceSquared)
			{
				SetCurrentState(Strafing);
				return;
			}
			else if (!CanSwapToAttackState && distSq < AttackRegisterRangeSquared) {
				SetToAttackState();
				return;
			}
			
			if (PathAgentComponent->HasNoPath())
			{
				{
					PathAgentComponent->EnableMovement();
					PathAgentComponent->MoveTo(TargetPlayer, 5.0f, true, true);
				}
			}
			
			FVector currentPos = GetActorLocation();
 
			FVector dirToPlayer = desiredTargetPos - currentPos; 
			FRotator fullRot = UKismetMathLibrary::MakeRotFromZX(FVector::UpVector, dirToPlayer);
			FRotator newRot = UKismetMathLibrary::RInterpTo(GetActorRotation(), fullRot, DeltaTime, 5.0f);
			SetActorRotation(newRot);
			
			break;			
		}
		
	case Strafing:
		{
			float distSq = FVector::DistSquared(GetActorLocation(), desiredTargetPos);
			if (distSq > StrafeExitDistanceSquared)
			{
				SetCurrentState(Flying);
				return;
			}
			else if (!CanSwapToAttackState && distSq < AttackRegisterRangeSquared) {
				SetToAttackState();
				return;
			}

			FVector currentPos = GetActorLocation();
			FVector properCurrentPos = currentPos;
			properCurrentPos.Z = desiredTargetPos.Z;
			FVector fromPlayerProper = properCurrentPos - desiredTargetPos;
			FVector fromPlayer = currentPos - desiredTargetPos;
			float distFromPlayer = fromPlayer.Length();
 
			FVector dirToPlayer = -fromPlayer; 
			FRotator fullRot = UKismetMathLibrary::MakeRotFromZX(FVector::UpVector, dirToPlayer);
			FRotator newRot = UKismetMathLibrary::RInterpTo(GetActorRotation(), fullRot, DeltaTime, 5.0f);
			SetActorRotation(newRot);
 
			if (distFromPlayer <= MinDistanceFromPlayer)
			{
				if (!PathAgentComponent->AgentIsAdjusting() && PathAgentComponent->HasNoPath())
				{
					PathAgentComponent->StartAdjustment(desiredTargetPos,fromPlayer, FVector::UpVector, AdjustmentConeAngle, MinDistanceFromPlayer, StrafeDistance, 0.8f);
				}
			}
			else
			{
				if (CanStrafe)
				{
					// Let any adjustment finish before trying new strafe directions
					// TODO: Not actually sure, this space only happens when we're within strafing distance anyways, 
					// I could probably comment this out so it doesnt have to wait for adjustment to finish and starts strafe attempts once its in range,
					// but might cause rapid switching when between min and not min
					if (!PathAgentComponent->HasNoPath()) return;
					
					FVector strafePoint;
					/** 
					 *  Strafe point is obtained by making an arc from player position to target position
					 * UseLeftHandedDir is for bias direction, since unreal uses left handed coord system, function bool flag is for whether or not it should make the segment from right to left
					 * Like said above, since UseLeftHandedDir is for bias, the segment will be created going from the preferred strafe direction to the other side in an arc
					 */
					bool found = PathAgentComponent->ParentPathFinder->GetBestAvailablePositionInCircleSegment(
						desiredTargetPos, FVector::UpVector, fromPlayerProper, FMath::RandRange(45.0f, 60.0f),
						MinDistanceFromPlayer, UseLeftHandedStrafeDir, strafePoint);

					if (!found) // No strafe point found, try to bias towards other strafe direction
					{
						found = PathAgentComponent->ParentPathFinder->GetBestAvailablePositionInCircleSegment(
							desiredTargetPos, FVector::UpVector, fromPlayerProper, FMath::RandRange(45.0f, 60.0f),
							MinDistanceFromPlayer, !UseLeftHandedStrafeDir, strafePoint);
						if (found) SwitchStrafeDirection_Manual();  // Switch strafe direction flag and restart swap timer
					}

					if (found)
					{
						PathAgentComponent->MoveToLocation(strafePoint, PathAgentComponent->ParentPathFinder->AgentRadius);
						CanStrafe = false;
					}
					else if (!PathAgentComponent->AgentIsAdjusting() && PathAgentComponent->HasNoPath())
					{
						// We have no path, so we either completed adjustment or some collision caused adjustment to fail, restart adjustment
						UE_LOG(LogTemp, Warning, TEXT("Can strafe and likely finished path"));
						PathAgentComponent->StartAdjustment(desiredTargetPos,fromPlayer, FVector::UpVector, AdjustmentConeAngle, MinDistanceFromPlayer, StrafeDistance, 0.8f);

						CanStrafe = false;
					}
				}
				else if (!PathAgentComponent->AgentIsAdjusting() && PathAgentComponent->HasNoPath()) // We have no path and we cannot strafe, will overall swap the CanStrafe to true to allow us to test
				{
					PathAgentComponent->StartAdjustment(desiredTargetPos,fromPlayer, FVector::UpVector, AdjustmentConeAngle, MinDistanceFromPlayer, StrafeDistance, 0.8f);
					CanStrafe = true;
				}
			}
			break;
			
		}
	case Attacking: 
		{
			FVector currentPos = GetActorLocation();
			FVector properCurrentPos = currentPos;
			properCurrentPos.Z = desiredTargetPos.Z;
			FVector fromPlayerProper = properCurrentPos - desiredTargetPos;
			FVector fromPlayer = currentPos - desiredTargetPos;
			float distFromPlayer = fromPlayer.Length();
 
			FVector dirToPlayer = -fromPlayer; 
			FRotator fullRot = UKismetMathLibrary::MakeRotFromZX(FVector::UpVector, dirToPlayer);
			FRotator newRot = UKismetMathLibrary::RInterpTo(GetActorRotation(), fullRot, DeltaTime, 5.0f);
			SetActorRotation(newRot);
			
			if (IsAttacking) return;
			// Not adjusting or has no path to adjust(collision cancelled or reached point) and out of attack range, start adjusting
			if (!PathAgentComponent->AgentIsAdjusting() && PathAgentComponent->HasNoPath() && (distFromPlayer < MinDistanceFromPlayer || distFromPlayer > StrafeDistance))
			{
				PathAgentComponent->EnableMovement();
				PathAgentComponent->StartAdjustment(desiredTargetPos, fromPlayerProper, FVector::UpVector, AdjustmentConeAngle, MinDistanceFromPlayer, StrafeDistance, 0.8f);
			}
			// Within attack range, stop adjusting and stop all movement for now(ill add attack later)
			else if (distFromPlayer > MinDistanceFromPlayer && 
				distFromPlayer < StrafeDistance)
			{

				PathAgentComponent->StopMovement();
				PathAgentComponent->DisableMovement();
				StartAttack();
				UE_LOG(LogTemp, Warning, TEXT("Enemy wants to attack"));
				// TODO: Finish attack function which sets to idle, idle should then check based on distance squared what state to be in
			}
			break;
		}
	case Idle:
		{
			float distSq = FVector::DistSquared(GetActorLocation(), desiredTargetPos);
			if (distSq > StrafeExitDistanceSquared)
			{
				SetCurrentState(Flying);
			}
			else if (distSq < StrafeDistanceSquared)
			{
				SetCurrentState(Strafing);
			}
			else if (!CanSwapToAttackState && distSq < AttackRegisterRangeSquared)
			{
				SetToAttackState();
			}
		break;
		}
	case EAIState::Stateless:
		{
			break;
		}
	default: ;
	}
	
}
void AFlyingEnemy::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
	StrafeDistanceSquared = StrafeDistance * StrafeDistance;
	MinDistanceFromPlayerSquared = MinDistanceFromPlayer * MinDistanceFromPlayer;
	StrafeExitDistanceSquared = (StrafeDistance + StrafeExitTolerance) * (StrafeDistance + StrafeExitTolerance);
	AttackRegisterRangeSquared = AttackRegisterRange * AttackRegisterRange;
}

void AFlyingEnemy::SetCurrentState(TEnumAsByte<EAIState> NewState)
{
	if (CurrentState == NewState) return;
	CurrentState = NewState;

	switch (CurrentState)
	{
	case Flying:
		PathAgentComponent->StopMovement();
		PathAgentComponent->EnableMovement();
		break;
	case Strafing:
		PathAgentComponent->StopMovement();
		PathAgentComponent->EnableMovement();
		CanStrafe = true;
		if (IsNewStrafeRequest)
		{
			RandomStrafeTime = UKismetMathLibrary::RandomFloatInRange(StrafeTimePerDirection_Ranged.X, StrafeTimePerDirection_Ranged.Y);
			GetWorld()->GetTimerManager().SetTimer(StrafeDirectionSwitchTimerHandle, this, &AFlyingEnemy::SwitchStrafeDirection, RandomStrafeTime);
			IsNewStrafeRequest = false;
		}
		break;
	case Attacking:
	if (!CanSwapToAttackState) return;
		PathAgentComponent->StopMovement();
		PathAgentComponent->EnableMovement();
	case Idle:
		PathAgentComponent->StopMovement();
		PathAgentComponent->DisableMovement();
		break;
	case EAIState::Stateless:
		break;
	default:
		break;
	}

}

TEnumAsByte<EAIState> AFlyingEnemy::GetCurrentState()
{
	return CurrentState;
}

void AFlyingEnemy::SetToAttackState()
{
	if (!CanSwapToAttackState)
	{
		CanSwapToAttackState = true;
		GetWorld()->GetTimerManager().SetTimer(AttackStateHandle, this, &AFlyingEnemy::SetToAttackState, 
										FMath::RandRange(DelayAfterRegisteringAttack_Ranged.X, DelayAfterRegisteringAttack_Ranged.Y));
		return;
	};
	SetCurrentState(Attacking);
}

void AFlyingEnemy::InvalidateEnemy()
{
	SetCurrentState(EAIState::Stateless);
}

void AFlyingEnemy::StartAttack_Implementation()
{
	EndAttack();
}

void AFlyingEnemy::EndAttack_Implementation()
{
	IsAttacking = false;
	GetWorld()->GetTimerManager().SetTimer(AttackStateHandle, this, &AFlyingEnemy::SetToAttackState, AttackCooldown + 2.0f);
	SetCurrentState(Idle);
}

void AFlyingEnemy::SetTarget_Implementation(AActor* TargetActor)
{
	Super::SetTarget_Implementation(TargetActor);
	
	TArray<AActor*> PathfinderActors;
	
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGC3DPathFinder::StaticClass(), PathfinderActors);
	
	if (PathfinderActors.IsEmpty()) return;
	
	PathAgentComponent->ParentPathFinder = Cast<AGC3DPathFinder>(PathfinderActors[0]);
	
	TargetPlayer = TargetActor;
	SetCurrentState(Flying);
}


void AFlyingEnemy::SwitchStrafeDirection()
{
	if (CurrentState != Strafing) return; 
	
	UE_LOG(LogTemp, Warning, TEXT("SwitchingDir Automatically"));
	
	UseLeftHandedStrafeDir = !UseLeftHandedStrafeDir;
	
	RandomStrafeTime = UKismetMathLibrary::RandomFloatInRange(StrafeTimePerDirection_Ranged.X, StrafeTimePerDirection_Ranged.Y);
	GetWorld()->GetTimerManager().SetTimer(StrafeDirectionSwitchTimerHandle, this, &AFlyingEnemy::SwitchStrafeDirection, RandomStrafeTime);
}

void AFlyingEnemy::SwitchStrafeDirection_Manual()
{
	if (CurrentState != Strafing) return;
	
	if (StrafeDirectionSwitchTimerHandle.IsValid()) GetWorld()->GetTimerManager().ClearTimer(StrafeDirectionSwitchTimerHandle);
	
	UE_LOG(LogTemp, Warning, TEXT("SwitchingDir manually"));
	SwitchStrafeDirection();
}


