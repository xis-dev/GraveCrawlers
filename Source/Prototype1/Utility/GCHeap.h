#pragma once

#include <vector>
#include <concepts>

template <typename T>
class GCHeapItem
{
public:
	virtual ~GCHeapItem() = default;
	
	int HeapIndex = -1;
	
	// [-1,1] returning func(should be), based on C#'s IComparable CompareTo
	virtual int Compare(const T& rhs) = 0;
	
	bool operator==(const GCHeapItem& rhs) const
	{
		return HeapIndex == rhs.HeapIndex;
	}
};

template <typename T>
requires std::derived_from<T, GCHeapItem<T>>
class GCHeap
{
public:
	TArray<T*> items;
	int currentItemCount{};
	
	GCHeap(int size)
	{
		items.SetNum(size);
	}
	
	void Add(T* item)
	{
		if (currentItemCount >= items.Num()) // Resize heap if full
		{
			ResizeHeap();
		}
		item->HeapIndex = currentItemCount;
		items[currentItemCount] = item;
		UpdateItem(item);
		++currentItemCount;
	}
	
	void ResizeHeap(int32 sizePadding = 100)
	{
		items.SetNum(currentItemCount + 1 + sizePadding);
	}
	
	void Clear()
	{
		currentItemCount = 0;
	}
	
	// Pop off first item from heap
	T* PopFirstItem()
	{
		T* firstItem = items[0];
		--currentItemCount;
		items[0] = items[currentItemCount]; // Set first item to the current last item
		items[0]->HeapIndex = 0;
		SortDown(items[0]); // Sort down from current first(previously last) to place it correctly
		
		return firstItem;
	}
	
	void UpdateItem(T* item)
	{
		// Only sorting up since A* does not reduce costs, ideally sort down too
		SortUp(item);
	}
	
	int Num() {return currentItemCount;}
	
	bool Contains(T* item)
	{
		if (item->HeapIndex < currentItemCount && item->HeapIndex >= 0)
		{
			return items[item->HeapIndex] == item;
		}
		return false;
	}
	
	void SortUp(T* item)
	{
		int parentIdx = (item->HeapIndex - 1)/2;
		while (true)
		{
			T* parentItem = items[parentIdx];
			if (item->Compare(*parentItem) > 0) // Only swap when this item has a higher priority than parent
			{
				Swap(item, parentItem);
			}
			else {break;}
			
			parentIdx = (item->HeapIndex - 1) / 2;
		}
	}
	
	void SortDown(T* item)
	{
		while (true)
		{
			int childIdxLeft = item->HeapIndex * 2 + 1;
			int childIdxRight = item->HeapIndex * 2 + 2;
			int swapIdx = 0;
			
			if (childIdxLeft < currentItemCount)
			{
				swapIdx = childIdxLeft;
				
				if (childIdxRight < currentItemCount)
				{
					// We want to swap with highest prio child, only change swap index to right if prio is higher than left
					if (items[childIdxLeft]->Compare(*items[childIdxRight]) < 0) swapIdx = childIdxRight;
				}
				
				if (item->Compare(*items[swapIdx]) < 0)
				{
					Swap(item, items[swapIdx]);
				}
				else // Parent has higher priority than children, is in right position, exit
				{
					return;
				}
			}
			else return; // Parent has no children, is in right position, exit
			
			
			
		}
	}
	
	void Swap(T* itemA, T* itemB)
	{
		items[itemA->HeapIndex] = itemB;
		items[itemB->HeapIndex] = itemA;
		
		int tempAHeapIdx = itemA->HeapIndex;
		itemA->HeapIndex = itemB->HeapIndex;
		itemB->HeapIndex = tempAHeapIdx;
	}
};

