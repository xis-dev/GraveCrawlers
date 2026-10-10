// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/RVOAvoidanceInterface.h"
#include "AI/Navigation/NavigationAvoidanceTypes.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GCMovementComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROTOTYPE1_API UGCMovementComponent : public UFloatingPawnMovement, public IRVOAvoidanceInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UGCMovementComponent();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UArrowComponent* FeetLocation;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class AGCCharacter* OwningCharacter;
	
	UPROPERTY(Category="Character Movement: Avoidance", EditAnywhere, BlueprintReadOnly, meta=(ForceUnits=cm))
	float AvoidanceConsiderationRadius;
	
	UPROPERTY(Category="Character Movement: Avoidance", EditAnywhere, BlueprintReadOnly)
	float AvoidanceWeight;
	
	UPROPERTY(Category = "Character Movement: Avoidance",  VisibleAnywhere, BlueprintReadOnly)
	int32 AvoidanceUID;
	
	UPROPERTY(Category="Character Movement: Avoidance", EditAnywhere, BlueprintReadOnly, AdvancedDisplay)
	FNavAvoidanceMask AvoidanceGroup;
	
	UPROPERTY(Category="Character Movement: Avoidance", EditAnywhere, BlueprintReadOnly, AdvancedDisplay)
	FNavAvoidanceMask GroupsToAvoid;
	
	UPROPERTY(Category="Character Movement: Avoidance", EditAnywhere, BlueprintReadOnly, AdvancedDisplay)
	FNavAvoidanceMask GroupsToIgnore;
	

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	
	virtual void PostInitProperties() override;
	
	AGCCharacter* GetCharacterOwner() const {return OwningCharacter;}
	
	// RVO Functions
	virtual void SetRVOAvoidanceUID(int32 UID) override;
	virtual int32 GetRVOAvoidanceUID() override;
	virtual void SetRVOAvoidanceWeight(float Weight) override;
	virtual float GetRVOAvoidanceWeight() override;
	virtual FVector GetRVOAvoidanceOrigin() override;
	virtual float GetRVOAvoidanceRadius() override;
	virtual float GetRVOAvoidanceHeight() override;
	virtual float GetRVOAvoidanceConsiderationRadius() override;
	virtual FVector GetVelocityForRVOConsideration() override;
	virtual void SetAvoidanceGroupMask(int32 GroupFlags) override;
	virtual int32 GetAvoidanceGroupMask() override;
	virtual void SetGroupsToAvoidMask(int32 GroupFlags) override;
	virtual int32 GetGroupsToAvoidMask() override;
	virtual void SetGroupsToIgnoreMask(int32 GroupFlags) override;
	virtual int32 GetGroupsToIgnoreMask() override;
	
	// TODO: Get capsule bounds and get base instead
	virtual FVector GetFeetLocation() const override;
};
