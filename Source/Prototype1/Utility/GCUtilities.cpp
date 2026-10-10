// Fill out your copyright notice in the Description page of Project Settings.


#include "GCUtilities.h"

#include "PhysicsEngine/PhysicsSettings.h"


UClass* UGCUtilities::GetParentClass_Object(UObject* Obj) {
    return Obj->GetClass()->GetSuperClass();
}

UClass* UGCUtilities::GetParentClass_Class(UClass* Class)
{
    return Class->GetSuperClass();
}

UClass* UGCUtilities::GetClass(UObject* Obj)
{
    return Obj->GetClass();
}

float UGCUtilities::GetInterpolationParameter(float value, float a, float b)
{
    if (FMath::IsNearlyEqual(a, b)) return 0.0f;
    return FMath::Clamp((-value + a)/(a - b), 0.0f, 1.0f);
}

int UGCUtilities::GetIndexBasedOnWeights(const TArray<float>& Weights)
{
    float sum = 0.0f;
    for (float w : Weights)
    {
        if (w < 0.0) continue;;
        sum += w;
    }
   return GetIndexBasedOnWeightsWithSum(Weights, sum);
}

int UGCUtilities::GetIndexBasedOnWeightsWithSum(const TArray<float>& Weights, float Sum)
{
    if (Sum <= 0.0001f || Weights.Num() == 0) return -1;

    float rand = FMath::FRandRange(0.0f, Sum);
    float accumulation = 0.0f;
    for (int i = 0; i < Weights.Num(); ++i)
    {
        if (Weights[i] < 0.0) continue; 
        accumulation += Weights[i];
        if (rand <= accumulation) 
            return i;
    }
    return (Weights.Num() > 0) ? Weights.Num() - 1 : -1;
}

int UGCUtilities::CompareInt(int lhs, int rhs)
{
    return lhs == rhs ? 0 : lhs < rhs ? -1 : 1;
}

float UGCUtilities::CalculateInitialVelocityToReachPoint(const FVector& Start, const FVector& End, float AngleInDegrees)
{
    float gravity = FMath::Abs(UPhysicsSettings::Get()->DefaultGravityZ); 
    float dX = End.X - Start.X;
    float dY = End.Y - Start.Y;
    float horizontalDist = FMath::Sqrt(dX * dX + dY * dY);
    float verticalDist = End.Z - Start.Z;          
    float angleRad = FMath::DegreesToRadians(AngleInDegrees);
    float cosAngle = FMath::Cos(angleRad);
    float tanAngle = FMath::Tan(angleRad);

    float denom = horizontalDist * tanAngle - verticalDist;
    if (denom <= UE_KINDA_SMALL_NUMBER) return 0.0f;  

    float velocity = FMath::Sqrt((gravity * horizontalDist * horizontalDist) / (2.0f * cosAngle * cosAngle * denom));
	
    return velocity;
}


