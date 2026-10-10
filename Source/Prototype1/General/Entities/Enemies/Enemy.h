#pragma once

#include "CoreMinimal.h"
#include "GCCharacter.h"

#include "Enemy.generated.h"

class UGCMovementComponent;
class UFloatingPawnMovement;
struct FGameplayTag;
struct FRuntimeStats;
class UStatComponent;
class UHealthComponent;
class UEnemyDataAsset;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEnemyKilled, AEnemy*, Enemy, AActor*, KillingActor);

UCLASS(Blueprintable, BlueprintType)
class AEnemy:public AGCCharacter
{
	GENERATED_BODY()

public:
	
	AEnemy();
	
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UHealthComponent* HealthComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStatComponent* StatComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* MinimapIcon;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UEnemyDataAsset> EnemyConfig;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Damage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackCooldown;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool CanDamage{true};
	
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "Events")
	FOnEnemyKilled OnEnemyKilled;
	

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Cost;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ScoreOnDeath;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float CurrentSlowPercentage{};

	UPROPERTY(BlueprintReadOnly)
	AActor* LastKillingActor{nullptr};
	
public:
	
	UFUNCTION(BlueprintCallable)
	int32 GetCost();
	
	UFUNCTION(BlueprintCallable)
	int32 GetScoreOnDeath();

		
	/** 
	* @return Returns the true slow effectiveness that was applied, since slow may not directly stack
	*/
	UFUNCTION(BlueprintCallable, Category = "Status Effects")
	float ApplySlow(float DesiredSlowEffectiveness);
	
	UFUNCTION(BlueprintCallable, Category = "Status Effects")
	void RemoveSlow(float DesiredSlowRemoval);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetTarget(AActor* TargetActor);
	virtual void SetTarget_Implementation(AActor* TargetActor);
	
	UFUNCTION(BlueprintCallable)
	void ChangeStatsForFirstMode();
	
	UFUNCTION(BlueprintCallable)
	void ChangeStatsForSecondMode();
	
	// GC Components - GraveCrawlers, so it doesnt hide InitializeComponents on Base Class
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable, Category = "Components")
	void InitializeGCComponents();
	virtual void InitializeGCComponents_Implementation();
	
	virtual void PostInitializeComponents() override;
	
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable, Category = "Stats")
	void OnStatChanged(FGameplayTag Tag, FRuntimeStats Stats);
	virtual void OnStatChanged_Implementation(FGameplayTag Tag, FRuntimeStats Stats);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Stat Changes")
	void HealthStatChanged(float Current, float Max);
	virtual void HealthStatChanged_Implementation(float Current, float Max);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Stat Changes")
	void MoveSpeedStatChanged(float Current);
	virtual void MoveSpeedStatChanged_Implementation(float Current);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Stat Changes")
	void DamageStatChanged(float Current);
	virtual void DamageStatChanged_Implementation(float Current);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Stat Changes")
	void AttackCooldownStatChanged(float Current);
	virtual void AttackCooldownStatChanged_Implementation(float Current);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Stat Changes")
    void AttackSpeedStatChanged(float Current);
    virtual void AttackSpeedStatChanged_Implementation(float Current);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Health")
	void OnDeath(AActor* KillingActor);
	virtual void OnDeath_Implementation(AActor* KillingActor);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Health")
	void FinishDeath();
	virtual void FinishDeath_Implementation();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Health")
	void OnDamaged();
	virtual void OnDamaged_Implementation();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Health")
	void OnHealthManipulated();
	virtual void OnHealthManipulated_Implementation();
	
};
