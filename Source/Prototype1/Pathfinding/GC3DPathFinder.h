// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GCDefaultMacros.h"
#include "GameFramework/Actor.h"
#include "GridNode.h"
#include "PathRequest.h"
#include "GC3DPathFinder.generated.h"

class UBoxComponent;
// 3D Pathfinder class for flying enemies
// TODO: Evaluate grid out of runtime
UCLASS(Blueprintable, BlueprintType)
class PROTOTYPE1_API AGC3DPathFinder : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGC3DPathFinder();
	
	virtual void OnConstruction(const FTransform& Transform) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Units = "s"))
	double MaxPathfindingTime{0.041};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxNodeExpansions{10000};
	
	// Grid Variables
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AgentRadius{50.0f};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool ShowGrid{};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FIntVector CellsPerAxis{1};
	
	// Full grid size
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* GridBox;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<ECollisionChannel> BlockingChannel{GC_TRACE_PATHFINDING};
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector CellSize{1};



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	int32 GridSize();
	
	UFUNCTION(CallInEditor)
	void ReEvaluateAndDisplayGrid();
	
	void EvaluateGrid();
	
	void EvaluateGridContinuously();
	
	void CalculateCellSizes();
	
	TArray<int32> GetNeighbourNodes(const FGridNode& Node);
	
	TArray<int32> GetNeighbourNodeIndicesByIndex(int32 Idx);
	
	bool IsDiagonalMoveClear(int32 fromIdx, int32 x, int32 y, int32 z);
	
	int32 GetDistance(int32 nodeA, int32 nodeB);
	
	TArray<int32> GetStepsToNode(int32 startIdx, int32 endIdx);
	
	TArray<FVector> NodesToPositions(const TArray<int32> nodes);
	
	TArray<FVector> SimplifyPositions(const TArray<FVector>& positions);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	FVector IndexToWorldPosition(int32 Idx);
	
	FVector IndexToLocalPosition(int32 Idx);
	
	int32 WorldPositionToIndex(const FVector& WorldPos);
	
	int32 LocalPositionToIndex(const FVector& LocalPos);
	
	FVector ClampPositionToGrid(const FVector& WorldPos);
	
	void FindPath(const FVector& StartPos, const FVector& EndPos);
	
	void VisualizePath(const TArray<int32>& path);
	
	/** 
	 * Gets available position in a circular segment from origin to a target, in some [minTestDistance, scaledDirection(Length)
	 * Samples along the end of the segment by @param  numberOfStepsAlongSegment and from each segment sample, samples towards target to minimum distance
	 */
	bool GetBestAvailablePositionInCircleSegment(FVector origin, FVector normalAxis, FVector scaledDirection, float angle, float minTestDistance,
													 bool preferLocalRight, FVector& outPoint, int32 numberOfStepsAlongSegment = 4, int32 numberOfStepsToTarget = 4);
	
	/** 
	 * Gets position within min and max distance in direction @param forwardAxis along a circular path 
	 * Samples in a cone from origin to chosen position  
	 */
	bool GetBestAvailablePositionInCirclesWithCones(const FVector& origin, const FVector& forwardAxis, const FVector& upAxis, 
							float minDistance, float maxDistance, float minConeAngle, float maxConeAngle, FVector& outPoint, int32 numCircularSamples = 4, int32 maxConeSamples = 2);
	
	/** 
	* Gets position within min and max distance in direction @param forwardAxis with given angular displacements @param sampleAngles in the plane it lies on 
	* Samples in a cone from origin to chosen position  
	*/
	bool GetBestAvailablePositionFromAngularDisplacementsWithCones(const FVector& origin, const FVector& forwardAxis, const FVector& upAxis, const TArray<float>& sampleAngles,
						float minDistance, float maxDistance, float minConeAngle, float maxConeAngle, FVector& outPoint, int32 maxConeSamples = 1);
	
	bool GetValidPositionInCone(FVector origin, FVector coneDir, float coneAngle,
							 float minDist, float maxDist, FVector& outPoint,
							 int32 numberOfSamples = 8);
	
	// TODO: Swap back to function pointer over name(see if i dont need this in bp at all then stop using dynamic delegate) 
	TSharedPtr<FPathRequest> RequestPath(const FVector& start, const FVector& end, UObject* owner, const FName& FuncName);
	
	
private:
	
	TArray<FGridNode> GridNodes;
	
	bool GridHasBeenEvaluated{};
	
	// Queue to handle path requests sequentially 
	TQueue<TSharedPtr<FPathRequest>> PathRequests;
	
	TSharedPtr<FPathRequest> CurrentPathRequest;
	
	int32 PathRequestsCount;
	
	bool IsProcessingPath;
	
	GCHeap<FGridNode> OpenSet{100};
	
	FTimerHandle PathfindingTimerHandle;
	
	void ResetNodes();
	
	void TryProcessNextPath();
	
	void FinishProcessingPath(const FPath& Path);
	
	
};
