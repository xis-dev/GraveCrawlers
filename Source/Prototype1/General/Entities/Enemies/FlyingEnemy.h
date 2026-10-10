// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "FlyingEnemy.generated.h"

class AFlyingEnemyProjectile;
class UGC3DPathFindingAgent;

UENUM(BlueprintType)
enum EAIState
{
	Idle,
	Flying,
	Attacking,
	Strafing,
	Stateless
};
UCLASS()
class PROTOTYPE1_API AFlyingEnemy : public AEnemy
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AFlyingEnemy();
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	virtual void OnConstruction(const FTransform& Transform) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UGC3DPathFindingAgent* PathAgentComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attacking")
	class UArrowComponent* AttackSpawnPoint;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Strafing")
	float StrafeDistance{1000.0f};
	
	UPROPERTY(BlueprintReadOnly, Category = "Strafing")
	float StrafeDistanceSquared{StrafeDistance * StrafeDistance};
	
	// Exit tolerance to leave strafing back to flying
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Strafing")
	float StrafeExitTolerance{75.0f};
	
	UPROPERTY(BlueprintReadOnly, Category = "Strafing")
	float StrafeExitDistanceSquared{(StrafeDistance + StrafeExitTolerance) * (StrafeDistance + StrafeExitTolerance)};
	
	// How long the enemy chooses to bias a particular strafe direction 
	UPROPERTY(BlueprintReadOnly, Category = "Strafing")
	FVector2D StrafeTimePerDirection_Ranged{2.0f, 5.0f};
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Attacking")
	bool IsAttacking{false};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attacking")
	float AttackRegisterRange{650.0f};
	
	UPROPERTY(BlueprintReadOnly, Category = "Attacking")
	float AttackRegisterRangeSquared{AttackRegisterRange * AttackRegisterRange};
	
	// How long to wait to perform first attack once the enemy reaches AttackRegisterRange
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attacking")
	FVector2D DelayAfterRegisteringAttack_Ranged{1.0f, 2.5f};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MinDistanceFromPlayer{400.0f};
	
	// To make attacking more balanced, attack is aimed in a random direction in this radius from player
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attacking")
	float AttackErrorRadius{100.0f};
	
		
	// In degrees, cone size angle from which a random adjustment vector will be chosen
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Adjustment")
	float AdjustmentConeAngle{45.0f};
	
	// TODO: Change to MainPlayer ptr if need be
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* TargetPlayer;
	
	
	UFUNCTION(BlueprintCallable, Category = "State")
	void SetCurrentState(TEnumAsByte<EAIState> NewState);
	
	UFUNCTION(BlueprintCallable, Category = "State")
	TEnumAsByte<EAIState> GetCurrentState();
	
	UFUNCTION(BlueprintCallable, Category = "Attacking")
	void SetToAttackState();
	
	UFUNCTION(BlueprintNativeEvent, Category = "Attacking")
	void StartAttack();
	
	UFUNCTION(BlueprintCallable, Category = "Attacking")
	void InvalidateEnemy();
	
	UFUNCTION(BlueprintNativeEvent, Category = "Attacking")
	void EndAttack();
	
	virtual void SetTarget_Implementation(AActor* TargetActor) override	;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TEnumAsByte<EAIState> CurrentState;
	
	FTimerHandle AttackStateHandle;
	

	
	float MinDistanceFromPlayerSquared{MinDistanceFromPlayer * MinDistanceFromPlayer};
	
	float RandomStrafeTime;
	
	UPROPERTY(VisibleAnywhere)
	bool CanStrafe = true;
	
	UPROPERTY(VisibleAnywhere)
	bool CanSwapToAttackState{false};
	
	bool IsNewStrafeRequest{true};
	
	// Flag on whether to use the traditional correct cross product direction in left handed coord system or negate it for opposite strafe dir
	bool UseLeftHandedStrafeDir;
	
	FTimerHandle StrafeDirectionSwitchTimerHandle;
	
	void SwitchStrafeDirection();
	
	void SwitchStrafeDirection_Manual();
	
};
