#pragma once
#include "PathRequest.h"


#include "GC3DPathFindingAgent.generated.h"

UENUM(BlueprintType)
enum EAgentMovementMode
{
	None,
	FollowingPath,
	ManualAdjust
};

USTRUCT(BlueprintType)
struct FNavigationProperties
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool LastMoveWasBlocked{};
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FVector LastMoveDirection{};
	
};

class AGC3DPathFinder;
class USphereComponent;

// TODO: Make this inherit from PawnMovementComponent & IRVOAvoidanceInterface then use pawn movement + it's velocity
UCLASS(BlueprintType, Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROTOTYPE1_API UGC3DPathFindingAgent: public UActorComponent
{
	GENERATED_BODY()
public:
	
	UGC3DPathFindingAgent();
	

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AGC3DPathFinder* ParentPathFinder;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* Target;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LookAheadDistance{1.0f};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Adjustment")
	FVector2D LocalAdjustmentDistanceRange{250.0f, 400.0f};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Adjustment")
	FVector2D LocalAdjustmentConeAngleRange{30.0f, 45.0f};
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool IsLocallyAdjusting;
	
	UPROPERTY(BlueprintReadOnly)
	float AcceptanceRadius;
	
	UPROPERTY(BlueprintReadOnly)
	float AcceptanceRadiusSquared;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool UseAcceptanceRadius{true};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Speed{450.0f};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PreferredHeightOffset{400.0f};
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool StartMovement;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FPath CurrentPath;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 CurrentPathIndex;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector PreviousTargetPos{MAX_FLT};
	
	UPROPERTY(BlueprintReadWrite)
	FNavigationProperties LastMoveProperties{};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool ForceMovement;
	
	bool PathFinished{false};
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TEnumAsByte<EAgentMovementMode> MovementMode{None};
	
	virtual void PostInitProperties() override;
	
	virtual void BeginPlay() override;
	
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	/** 
	 * @param ContinuousMovement Will continue trying to move after reaching target
	 */
	UFUNCTION(BlueprintCallable)
	void MoveTo(AActor* ActorToMoveTo, float AcceptanceRad, bool ContinuousMovement = true, bool ForceMove = false);
	
	UFUNCTION(BlueprintCallable)
	void MoveToLocation(FVector Position, float AcceptanceRad);
	
	UFUNCTION(BlueprintCallable)
	void StopMovement();
	
	UFUNCTION(BlueprintCallable, Category = "Adjustment")
	void PerformLocalAdjustment();
	
	UFUNCTION(BlueprintCallable)
	void StartAdjustment(FVector TargetLoc, FVector PreferredDirection, FVector UpVector, float ConeAngle, float MinDist, float MaxDist, float PreferredDirectionWeight = 1.0f);

	UFUNCTION(BlueprintCallable)
	void StopManualAdjustment();

	UFUNCTION(BlueprintCallable)
	bool AgentIsAdjusting();
	
	UFUNCTION(BlueprintCallable)
	bool AgentIsLocallyAdjusting();
	
	UFUNCTION()
	void OnPathRequestCompleted(const FPath& Path);
	
	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnPathRequestCompleted(const FPath& Path);
	
	UFUNCTION(BlueprintCallable)
	void RequestPath(const FVector& start, const FVector& end);
	
	UFUNCTION(BlueprintCallable)
	void TryRequestPathToTarget();
	
	UFUNCTION()
	bool HasMovingPath();
	
	UFUNCTION(BlueprintCallable)
	bool HasNoPath();
	
	UFUNCTION(BlueprintCallable)
	void EnableMovement();
	
	UFUNCTION(BlueprintCallable)
	void DisableMovement();
	
	UFUNCTION(BlueprintCallable)
	bool IsMovementEnabled();
	
	virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;
	
private:
	UPROPERTY(VisibleAnywhere)
	bool MovementEnabled{true};
	
	UPROPERTY(VisibleAnywhere)
	bool IsAdjusting{false};

	bool ContinueTrackingTarget;
	
	TSharedPtr<FPathRequest> CurrentPathRequest{nullptr};

	FTimerHandle PathRequestTimerHandle;

	void TickFollowPath(float DeltaTime, AActor* Owner);
	
};
