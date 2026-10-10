#pragma once

#include "GCHeap.h"

// Grid node struct to define the state of a certain grid node
struct FGridNode: public GCHeapItem<FGridNode>
{
	
public:
	
	virtual int Compare(const FGridNode& rhs) override;
	
	bool operator==(const FGridNode& rhs) const
	{
		return Index == rhs.Index;
	}
	
	void Reset()
	{
		Index = -1;
		NodeParentIndex = -1;
		IsAvailable = true;
		IsInClosedSet = false;
		GCost = MAX_int32;
		HCost = 0;
	}
	
	// Reset node parameters without changing state, so essentially reset everything but IsAvailable & Index;
	void ResetParametersToPathfind()
	{
		NodeParentIndex = -1;
		IsInClosedSet = false;
		GCost = MAX_int32;
		HCost = 0;
	}
	
	// Type Hash func for set/map
	friend uint32 GetTypeHash(const FGridNode& node)
	{
		return GetTypeHash(node.Index);
	}
	
	
	bool IsAvailable{true};
	
	bool IsInClosedSet{};

	int32 Index{-1};
	
	int32 NodeParentIndex{-1};
	
	TArray<int32> NeighbourNodes{};
	
	// Cost from starting point to this node
	int32 GCost{MAX_int32};
	
	// Cost from this node to target node
	int32 HCost{0};
	
	int32 FCost() const {return GCost + HCost;}
};
