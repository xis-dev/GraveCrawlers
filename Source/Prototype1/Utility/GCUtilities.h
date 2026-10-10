// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <functional>

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include <vector>
#include "GCUtilities.generated.h"

/**
 * Personal Gravecrawlers Utility Class
 */
UCLASS()
class PROTOTYPE1_API UGCUtilities : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Utility")
	static UClass* GetParentClass_Object(UObject* Object);

	UFUNCTION(BlueprintCallable, Category = "Utility")
	static UClass* GetParentClass_Class(UClass* Class);
	
	UFUNCTION(BlueprintCallable, Category = "Utility")
    static UClass* GetClass(UObject* Obj);
	
	/** 
	 * Get interpolation parameter(t), for @param value value between @param a and @param b
	 */
	UFUNCTION(BlueprintCallable, Category = "Math Utility")
	static float GetInterpolationParameter(float value, float a, float b);
	
	UFUNCTION(BlueprintCallable, Category = "Utility")
	static int GetIndexBasedOnWeights(const TArray<float>& Weights);
	
	// Index based on weights but you already know the sum, save from needing to calc
	UFUNCTION(BlueprintCallable, Category = "Utility")
	static int GetIndexBasedOnWeightsWithSum(const TArray<float>& Weights, float Sum);
	
	UFUNCTION(BlueprintCallable, Category = "Physics Utility")
	static float CalculateInitialVelocityToReachPoint(const FVector& Start, const FVector& End, float AngleInDegrees);
	
	// Unreal's UKismetArrayLibrary Reverse seems to not be proper and cant find good documentation on the usage of FArrayProperty for generic reverse so this will do
	template <typename T>
	UFUNCTION(BlueprintCallable, Category = "Utility")
	static void ReverseTArray(TArray<T>& array)
	{
		int left = 0;
		int right = array.Num() - 1;
	
		while (left < right)
		{
			Swap(array[left], array[right]);
		
			++left;
			--right;
		}
	};
	
	template <typename T>
	static void ReverseVector(std::vector<T>& vec)
	{
		int left = 0;
		int right = vec.size() - 1;
	
		while (left < right)
		{
			std::swap(vec[left], vec[right]);
		
			++left;
			--right;
		}
	}
	
	template <typename T>
	static bool VectorContains(std::vector<T>& vec, T item)
	{
		auto vecIterator = std::find_if(vec.begin(), vec.end(), [item](const T& element){return item == element;});
	
		return vecIterator != vec.end();
	};
	
	// Helper function translating C#'s CompareTo for integers, just to make FGridNode cleaner
	static int CompareInt(int lhs, int rhs);
	
	// Helper function so i can use pure function pointers from dynamic object instances
	template <typename ReturnType, class OwnerType, typename... Params>
	static std::function<ReturnType(Params...)> ConvertDynamicFunctionToStaticForm(OwnerType* owningObject, ReturnType(OwnerType::*func)(Params...))
	{
		return [owningObject, func](Params... p){(owningObject->*func)(p...);};
	}
};


