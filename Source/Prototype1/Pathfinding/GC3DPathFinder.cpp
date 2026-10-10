// Fill out your copyright notice in the Description page of Project Settings.


#include "GC3DPathFinder.h"

#include "GCUtilities.h"
#include "GridNode.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AGC3DPathFinder::AGC3DPathFinder()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	GridBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Grid Box"));
	RootComponent = GridBox;
	GridBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
}

void AGC3DPathFinder::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	

	CalculateCellSizes();
	

}

// Called when the game starts or when spawned
void AGC3DPathFinder::BeginPlay()
{
	Super::BeginPlay();
	
	// Calc cell sizes prior as it wont update if no changes are made
	CalculateCellSizes();
	
	EvaluateGrid();
	
}

int32 AGC3DPathFinder::GridSize()
{
	return CellsPerAxis.X * CellsPerAxis.Y * CellsPerAxis.Z;
}

void AGC3DPathFinder::ReEvaluateAndDisplayGrid()
{
	int32 totalCells = CellsPerAxis.X * CellsPerAxis.Y * CellsPerAxis.Z;
	GridNodes.SetNum(totalCells);
	for (int32 i = 0; i < totalCells; ++i)
	{
		GridNodes[i].Index = i;
		GridNodes[i].NeighbourNodes = GetNeighbourNodeIndicesByIndex(i);
		FVector cellWorldCenter = IndexToWorldPosition(i);
 

		FVector cellHalfExtent = CellSize * 0.5f;
		FCollisionShape sphereShape = FCollisionShape::MakeSphere((AgentRadius));
		
		bool success = GetWorld()->OverlapBlockingTestByChannel(
			cellWorldCenter, GridBox->GetComponentQuat(), BlockingChannel, sphereShape);
        
		GridNodes[i].IsAvailable = !success;
		GridNodes[i].ResetParametersToPathfind();
	}
	
	GridHasBeenEvaluated = true;
	
	for (auto node: GridNodes)
	{
		DrawDebugBox(GetWorld(), IndexToWorldPosition(node.Index), CellSize * 0.45, node.IsAvailable ? FColor::Green : FColor::Red, false, 0.5f);
	}
	
	
}

void AGC3DPathFinder::EvaluateGrid()
{
	int32 totalCells = CellsPerAxis.X * CellsPerAxis.Y * CellsPerAxis.Z;
	GridNodes.SetNum(totalCells);
	for (int32 i = 0; i < totalCells; ++i)
	{
		GridNodes[i].Index = i;
		GridNodes[i].NeighbourNodes = GetNeighbourNodeIndicesByIndex(i);
		FVector cellWorldCenter = IndexToWorldPosition(i);
 

		FVector cellHalfExtent = CellSize * 0.5f;
		FCollisionShape sphereShape = FCollisionShape::MakeSphere((AgentRadius));
		
		bool success = GetWorld()->OverlapBlockingTestByChannel(
			cellWorldCenter, GridBox->GetComponentQuat(), BlockingChannel, sphereShape);
        
		GridNodes[i].IsAvailable = !success;
		
		GridNodes[i].ResetParametersToPathfind();
	}
	
	GridHasBeenEvaluated = true;
}

void AGC3DPathFinder::EvaluateGridContinuously()
{
	// Makes sure node reevaluation happens when nothing should be going on, ie start of next frame
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &AGC3DPathFinder::EvaluateGrid);
}

void AGC3DPathFinder::CalculateCellSizes()
{	
	FVector GridExtent = GridBox->GetScaledBoxExtent();
	
	CellSize.X = CellsPerAxis.X > 0 ? (GridExtent.X * 2.0f) / CellsPerAxis.X : 0;
	CellSize.Y = CellsPerAxis.Y > 0 ? (GridExtent.Y * 2.0f) / CellsPerAxis.Y : 0;
	CellSize.Z = CellsPerAxis.Z > 0 ? (GridExtent.Z * 2.0f) / CellsPerAxis.Z : 0;
}

TArray<int32> AGC3DPathFinder::GetNeighbourNodes(const FGridNode& Node)
{
	if (Node.Index <= -1) return TArray<int32>{};
	
	return GetNeighbourNodeIndicesByIndex(Node.Index);
}

TArray<int32> AGC3DPathFinder::GetNeighbourNodeIndicesByIndex(int32 Idx)
{
	TArray<int32> Neighbours{};
	
	FVector nodePos = IndexToLocalPosition(Idx);
	for (int x = -1; x <= 1; ++x)
	{
		for (int y = -1; y <= 1; ++y)
		{
			for (int z = -1; z <= 1; ++z)
			{
				if (x == 0 && y == 0 && z == 0) continue;

				if (!IsDiagonalMoveClear(Idx, x, y, z)) continue;
				
				FVector pos;
				pos.X = nodePos.X + x * CellSize.X;
				pos.Y = nodePos.Y + y * CellSize.Y;
				pos.Z = nodePos.Z + z * CellSize.Z;
				
				int32 neighbourIdx = LocalPositionToIndex(pos);
				if (neighbourIdx <= -1) continue;
				
				Neighbours.Add(neighbourIdx);
			}
		}
	}
	
	return Neighbours;
}

bool AGC3DPathFinder::IsDiagonalMoveClear(int32 fromIdx, int32 x, int32 y, int32 z)
{
	int32 axesUsed = FMath::Abs(x) + FMath::Abs(y) + FMath::Abs(z);
	if (axesUsed <= 1) return true;
 
	FVector nodePos = IndexToLocalPosition(fromIdx);
	const int32 steps[3] = { x, y, z };
	
	for (int32 axis = 0; axis < 3; ++axis)
	{
		if (steps[axis] == 0) continue;
 
		FVector pos = nodePos;
		if (axis == 0) pos.X += steps[axis] * CellSize.X;
		else if (axis == 1) pos.Y += steps[axis] * CellSize.Y;
		else pos.Z += steps[axis] * CellSize.Z;
 
		int32 idx = LocalPositionToIndex(pos);
		if (idx <= -1 || !GridNodes[idx].IsAvailable) return false;
	}
 
	return true;
}

int32 AGC3DPathFinder::GetDistance(int32 nodeA, int32 nodeB)
{
	// Get relative positions of nodes, no need to do full conversion
	int xA = nodeA % CellsPerAxis.X;
	int yA = nodeA % (CellsPerAxis.X * CellsPerAxis.Y) / CellsPerAxis.X;
	int zA = nodeA / (CellsPerAxis.X * CellsPerAxis.Y);
	
	int xB = nodeB % CellsPerAxis.X;
	int yB = nodeB % (CellsPerAxis.X * CellsPerAxis.Y) / CellsPerAxis.X;
	int zB = nodeB / (CellsPerAxis.X * CellsPerAxis.Y);
	
	int32 dist[3] = { FMath::Abs(xA - xB), FMath::Abs(yA - yB), FMath::Abs(zA - zB) };
	
	// Sort lowest to highest
	Algo::Sort(dist, [&](int32 a, int32 b)
	{
		return a < b;
	});
	
	return 17 * dist[0] + 14 * (dist[1] - dist[0]) + 10 * (dist[2] - dist[1]);
}

TArray<int32> AGC3DPathFinder::GetStepsToNode(int32 startIdx, int32 endIdx)
{
	TArray<int32> nodeIndicesToFollow{};
	int32 currentIdx = endIdx;
	
	while (currentIdx != startIdx)
	{
		if (currentIdx <= -1)
		{
			UE_LOG(LogTemp, Error, TEXT("Ill formed steps to node"));
			return TArray<int32>{};
		}
		nodeIndicesToFollow.Add(currentIdx);
		currentIdx = GridNodes[currentIdx].NodeParentIndex;
	}
	
	UGCUtilities::ReverseTArray(nodeIndicesToFollow);
	return nodeIndicesToFollow;
}

TArray<FVector> AGC3DPathFinder::NodesToPositions(const TArray<int32> nodes)
{
	TArray<FVector> Positions;
	for (int32 idx: nodes)
	{
		if (idx == -1)
		{
			UE_LOG(LogTemp, Warning, TEXT("Trying to convert ill formed node to world position, will be skipped"));
			continue;
		}
		Positions.Add(IndexToWorldPosition(idx));
	}
	
	return SimplifyPositions(Positions);
}

TArray<FVector> AGC3DPathFinder::SimplifyPositions(const TArray<FVector>& positions)
{
	TArray<FVector> result;
	if (positions.Num() == 0) return result;
	if (positions.Num() == 1)
	{
		result.Add(positions[0]);
		return result;
	}
 
	result.Add(positions[0]); // Keep first original pos
 
	FVector oldDirection = (positions[1] - positions[0]).GetSafeNormal();
	for (int i = 1; i < positions.Num() - 1; ++i)
	{
		FVector newDirection = (positions[i + 1] - positions[i]).GetSafeNormal();
		if (!newDirection.Equals(oldDirection, 0.001)) // Keep only when direction changes
		{
			result.Add(positions[i]); 
		}
		oldDirection = newDirection;
	}
 
	result.Add(positions.Last()); // Keep last original pos
	return result;
}

// Called every frame
void AGC3DPathFinder::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (!ShowGrid) return;
	for (auto node: GridNodes)
	{
		DrawDebugBox(GetWorld(), IndexToWorldPosition(node.Index), CellSize * 0.45, node.IsAvailable ? FColor::Green : FColor::Red, false, 0.5f);
	}
}

FVector AGC3DPathFinder::IndexToWorldPosition(int32 idx)
{
	return GridBox->GetComponentTransform().TransformPosition(IndexToLocalPosition(idx));
}

FVector AGC3DPathFinder::IndexToLocalPosition(int32 Idx)
{
	if (Idx == -1) return FVector{};
	int32 x = Idx % CellsPerAxis.X;
	int32 y = (Idx % (CellsPerAxis.X * CellsPerAxis.Y)) / CellsPerAxis.X;
	int32 z = Idx / (CellsPerAxis.X * CellsPerAxis.Y);

	FVector LocalCellCorner(x * CellSize.X, y * CellSize.Y, z * CellSize.Z);

	// Move pos to center
	FVector LocalCellCenter = LocalCellCorner + FVector(CellSize.X, CellSize.Y, CellSize.Z) * 0.5f;

	// Offset the position range from [0, 2ex] back to [-extent, extent]
	FVector result = LocalCellCenter - GridBox->GetScaledBoxExtent();
	return result;
}

int32 AGC3DPathFinder::WorldPositionToIndex(const FVector& WorldPos)
{
	FVector LocalPos = GridBox->GetComponentTransform().InverseTransformPosition(WorldPos);
	
	return LocalPositionToIndex(LocalPos);
	
}

int32 AGC3DPathFinder::LocalPositionToIndex(const FVector& LocalPos)
{
	// Offset position by extent, moving range from [-ex, ex] to [0, ex * 2]
	FVector OffsetPos = LocalPos + GridBox->GetScaledBoxExtent();
	
	if (CellSize.X <= 0 || CellSize.Y <= 0 || CellSize.Z <= 0) return -1;
	int32 x = FMath::FloorToInt32(OffsetPos.X / CellSize.X);
	if (x < 0 || x > CellsPerAxis.X - 1) return -1; 
	
	int32 y = FMath::FloorToInt32(OffsetPos.Y / CellSize.Y);
	if (y < 0 || y > CellsPerAxis.Y - 1) return -1;
	
	int32 z = FMath::FloorToInt32(OffsetPos.Z / CellSize.Z);
	if (z < 0 || z > CellsPerAxis.Z - 1) return -1;

	return x + y * CellsPerAxis.X + z * CellsPerAxis.X * CellsPerAxis.Y;
}


FVector AGC3DPathFinder::ClampPositionToGrid(const FVector& WorldPos)
{
	FVector localPos = GridBox->GetComponentTransform().InverseTransformPosition(WorldPos);
	FVector extent = GridBox->GetScaledBoxExtent();
 
	localPos.X = FMath::Clamp(localPos.X, -extent.X, extent.X);
	localPos.Y = FMath::Clamp(localPos.Y, -extent.Y, extent.Y);
	localPos.Z = FMath::Clamp(localPos.Z, -extent.Z, extent.Z);
 
	return GridBox->GetComponentTransform().TransformPosition(localPos);
}

void AGC3DPathFinder::FindPath(const FVector& StartPos, const FVector& EndPos)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(Pathfinding);
	
	
	FPath Path{{}, false};
	
	int32 startIdx = WorldPositionToIndex(StartPos);
	if (startIdx <= -1)
	{
		FinishProcessingPath(Path);
		return ;
	};
	
	int32 endIdx  = WorldPositionToIndex(EndPos);
	if (endIdx <= -1)
	{
		FinishProcessingPath(Path);
		return ;
	};

	ResetNodes();
	OpenSet.Clear();
	
	GridNodes[startIdx].GCost = 0;
	GridNodes[startIdx].HCost = GetDistance(startIdx, endIdx);
	
	OpenSet.Add(&GridNodes[startIdx]);
	
	bool foundFullPath = false;
	
	int32 lastClosestNode = startIdx;
	
	int32 maxNodeExpansions = 10000;
	int32 nodesExpanded{};
	
	uint64 startTime = FPlatformTime::Cycles64();
	double currentTime = 0.0;
	while (OpenSet.Num() > 0 && nodesExpanded < maxNodeExpansions && currentTime < MaxPathfindingTime)
	{
		if (CurrentPathRequest->IsInvalid) break; // Cancel invalidated path searches
		
		++nodesExpanded;
		int32 currentNodeIdx = OpenSet.PopFirstItem()->Index;
		GridNodes[currentNodeIdx].IsInClosedSet = true;
		
		if (currentNodeIdx == endIdx && !CurrentPathRequest->IsInvalid)
		{
			foundFullPath = true;
			Path.IsValid = true;
			Path.PositionsToFollow = NodesToPositions(GetStepsToNode(startIdx, currentNodeIdx));
			break; 
		}
		
		if (GridNodes[currentNodeIdx].HCost < GridNodes[lastClosestNode].HCost) 
		{
			// Cache the closest node last found, incase we find no path
			lastClosestNode = currentNodeIdx;
		}
		
			
		for (int32 neighbourIdx: GridNodes[currentNodeIdx].NeighbourNodes)
		{
			auto* neighbourPtr = &GridNodes[neighbourIdx];
			if (!GridNodes[neighbourIdx].IsAvailable || GridNodes[neighbourIdx].IsInClosedSet) continue;
				
			int32 newMovementCostToNeighbour = GridNodes[currentNodeIdx].GCost + GetDistance(currentNodeIdx, neighbourIdx);
			if (newMovementCostToNeighbour < GridNodes[neighbourIdx].GCost || !OpenSet.Contains(neighbourPtr))
			{
				GridNodes[neighbourIdx].GCost = newMovementCostToNeighbour;
				GridNodes[neighbourIdx].HCost = GetDistance(neighbourIdx, endIdx);
				GridNodes[neighbourIdx].NodeParentIndex = currentNodeIdx;
				
					
				if (!OpenSet.Contains(neighbourPtr))
				{
					OpenSet.Add(neighbourPtr);
				}
				else OpenSet.UpdateItem(neighbourPtr);
			}
				
		}
		
		currentTime = FPlatformTime::ToMilliseconds64(FPlatformTime::Cycles64() - startTime);
	}
	
	if (!foundFullPath && lastClosestNode != startIdx) // Use partial path
	{
		Path.IsValid = true;
		Path.PositionsToFollow = NodesToPositions(GetStepsToNode(startIdx, lastClosestNode));
	}
	FinishProcessingPath(Path);
}

void AGC3DPathFinder::VisualizePath(const TArray<int32>& path)
{
	if (path.Num() <= 0) return;
	FVector previousIdxPos{IndexToWorldPosition(path[0])};
	for (int32 idx: path)
	{
		FVector currentIdxPos = IndexToWorldPosition(idx);
		DrawDebugLine(GetWorld(), previousIdxPos, currentIdxPos, FColor::Green, false, 5.0f, 0, 5.0f);
		previousIdxPos = currentIdxPos;	
	}
}

bool AGC3DPathFinder::GetBestAvailablePositionInCircleSegment(FVector origin, FVector normalAxis, FVector scaledDirection,
                                                              float angle, float minTestDistance, bool preferLocalRight, FVector& outPoint, int32 numberOfStepsAlongSegment,
                                                              int32 numberOfStepsToTarget)
{
	bool successfullyFound{false};
	float distanceToTarget = scaledDirection.Length();
	
	if (distanceToTarget < minTestDistance)
	{
		outPoint = FVector::Zero();
		return successfullyFound;
	}
	
	float testingDistance = distanceToTarget - minTestDistance;
	float stepDistance = testingDistance/numberOfStepsToTarget;
	FVector directionBackToTarget = -scaledDirection.GetSafeNormal();
	float startAngle = angle / 2.0f; 
	float stepAngle = -(angle / numberOfStepsAlongSegment);
	
	if (!preferLocalRight) startAngle = - startAngle;
	if (!preferLocalRight) stepAngle = -stepAngle;
	
	TArray<FVector> mainPositionsAlongSegment{};
	for (int i = 0; i < numberOfStepsAlongSegment; ++i)
	{
		FVector point = origin + UKismetMathLibrary::RotateAngleAxis(scaledDirection, startAngle + (i * stepAngle), normalAxis);
		mainPositionsAlongSegment.Add(point);
	}
	
	for (const FVector& pointAlongSegment: mainPositionsAlongSegment)
	{
		for (int i = 0; i < numberOfStepsToTarget; ++i)
		{
			FVector testingPoint = (directionBackToTarget * i * stepDistance) + pointAlongSegment;
			int32 idx = WorldPositionToIndex(testingPoint);
			if (idx == -1) continue;
			if (GridNodes[idx].IsAvailable) // Should be using agent size to determine so no need for radius
			{
				outPoint = testingPoint;
				successfullyFound = true;
				break;
			} 
		}
		if (successfullyFound) break;
	}
	
	return successfullyFound;
}

bool AGC3DPathFinder::GetBestAvailablePositionInCirclesWithCones(const FVector& origin,
                                                                 const FVector& forwardAxis, const FVector& upAxis, float minDistance, float maxDistance, float minConeAngle, float maxConeAngle,
                                                                 FVector& outPoint, int32 numCircularSamples, int32 maxConeSamples)
{
	float angleStep = 360.0f / numCircularSamples;
	
	TArray<FVector> mainSampleDirections;
	for (int i = 0; i < numCircularSamples; ++i)
	{
		FVector direction = UKismetMathLibrary::RotateAngleAxis(forwardAxis, i * angleStep, upAxis);
		mainSampleDirections.Add(direction.GetSafeNormal());
	}
	
	for (const auto& sampleDir: mainSampleDirections)
	{
		float sampleDist = FMath::RandRange(minDistance, maxDistance);
		float sampleConeAngle = FMath::RandRange(minConeAngle, maxConeAngle);
		
		for (int i = 0; i < maxConeSamples; ++i)
		{
			FVector trueDir = UKismetMathLibrary::RandomUnitVectorInConeInDegrees(sampleDir, sampleConeAngle);
			FVector testPoint = origin + trueDir * sampleDist;
		
		
			int32 ind = WorldPositionToIndex(testPoint);
			if (ind == -1 || !GridNodes[ind].IsAvailable) continue;
		
			outPoint = IndexToWorldPosition(ind);
			
			return true;
		}
		
	}
	
	return false;
}

bool AGC3DPathFinder::GetBestAvailablePositionFromAngularDisplacementsWithCones(const FVector& origin,
	const FVector& forwardAxis, const FVector& upAxis, const TArray<float>& sampleAngles, float minDistance, float maxDistance,
	float minConeAngle, float maxConeAngle, FVector& outPoint, int32 maxConeSamples)
{
	
	TArray<FVector> mainSampleDirections;
	
	for (float angle: sampleAngles)
	{
		FVector direction = UKismetMathLibrary::RotateAngleAxis(forwardAxis, angle, upAxis);
		mainSampleDirections.Add(direction.GetSafeNormal());
	}
	
	for (const auto& sampleDir: mainSampleDirections)
	{
		float sampleDist = FMath::RandRange(minDistance, maxDistance);
		
		for (int i = 0; i < maxConeSamples; ++i)
		{
			float sampleConeAngle = FMath::RandRange(minConeAngle, maxConeAngle);
			FVector trueDir = UKismetMathLibrary::RandomUnitVectorInConeInDegrees(sampleDir, sampleConeAngle);
			FVector testPoint = origin + trueDir * sampleDist;
		
		
			int32 ind = WorldPositionToIndex(testPoint);
			if (ind == -1 || !GridNodes[ind].IsAvailable) continue;
		
			outPoint = IndexToWorldPosition(ind);
			return true;
		}
	}
	
	return false;
}


bool AGC3DPathFinder::GetValidPositionInCone(FVector origin, FVector coneDir, float coneAngle, float minDist,
                                             float maxDist, FVector& outPoint, int32 numberOfSamples)
{
	FVector dir = coneDir.GetSafeNormal();
	if (dir.IsNearlyZero()) dir = FVector::ForwardVector;

	for (int32 i = 0; i < numberOfSamples; ++i)
	{
		FVector sampleDir = UKismetMathLibrary::RandomUnitVectorInConeInDegrees(dir, coneAngle);
		float sampleDist = UKismetMathLibrary::RandomFloatInRange(minDist, maxDist);
		FVector testPoint = origin + sampleDir * sampleDist;

		int32 idx = WorldPositionToIndex(testPoint);
		if (idx == -1 || !GridNodes[idx].IsAvailable) continue;

		outPoint = IndexToWorldPosition(idx);
		return true;
	}
	return false;
}



TSharedPtr<FPathRequest> AGC3DPathFinder::RequestPath(const FVector& start, const FVector& end, UObject* owner, const FName& FuncName)
{
	if (!GridHasBeenEvaluated) EvaluateGrid();
	auto NewRequest = MakeShared<FPathRequest>();
	NewRequest->PathStart = start;
	NewRequest->PathEnd = end;
	NewRequest->PathRequestResolved.BindUFunction(owner, FuncName);
	
	PathRequests.Enqueue(NewRequest);
	++PathRequestsCount;
	TryProcessNextPath();
	
	return NewRequest;
}


void AGC3DPathFinder::ResetNodes()
{
	for (int i = 0; i < GridNodes.Num(); ++i)
	{
		GridNodes[i].Index = i;
		GridNodes[i].NodeParentIndex = -1;
		GridNodes[i].GCost = MAX_int32;
		GridNodes[i].HCost = 0;
		GridNodes[i].IsInClosedSet = false;
	}
}

void AGC3DPathFinder::TryProcessNextPath()
{
	if (!IsProcessingPath && PathRequestsCount > 0)
	{
		PathRequests.Dequeue(CurrentPathRequest);
		--PathRequestsCount;
		
		if (CurrentPathRequest->IsInvalid) TryProcessNextPath();
		IsProcessingPath = true;
		FindPath(CurrentPathRequest->PathStart, CurrentPathRequest->PathEnd);
	}
}

void AGC3DPathFinder::FinishProcessingPath(const FPath& Path)
{
	CurrentPathRequest->PathRequestResolved.ExecuteIfBound(Path);
	IsProcessingPath = false;
	TryProcessNextPath();
}




