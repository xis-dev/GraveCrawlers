#include "GridNode.h"

#include "GCUtilities.h"

int FGridNode::Compare(const FGridNode& rhs)
{
	int compare = UGCUtilities::CompareInt(FCost(), rhs.FCost());
	if (compare == 0)
	{
		compare = UGCUtilities::CompareInt(HCost, rhs.HCost);
	}
	
	return -compare; // We want to return higher priority if the value is lower 
}
