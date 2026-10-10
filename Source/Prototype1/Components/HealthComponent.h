#pragma once

#include "CoreMinimal.h"
#include "HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeath);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeathWithKillingActor, AActor*, KillingActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDamaged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDamagedWithDamagingActor, AActor*, DamagingActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHealed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHealthManipulated);


UCLASS(BlueprintType, Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UHealthComponent: public UActorComponent
{
	GENERATED_BODY()
public:
	
	UHealthComponent();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float CurrentHealth;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool IsAlive;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float MaxHealth;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LethalDamageThreshold;
	
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "Events")
	FOnDeath OnDeath;
	
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "Events")
	FOnDamaged OnDamaged;
	
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "Events|Actor Callbacks")
    FOnDamagedWithDamagingActor OnDamaged_Actor;
    
    UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "Events|Actor Callbacks")
    FOnDeathWithKillingActor OnDeath_Actor;
	
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "Events")
	FOnHealed OnHealed;
	
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "Events")
	FOnHealthManipulated OnHealthManipulated;
	
	UFUNCTION(BlueprintCallable)
	void InitializeHealth(float health);
	
	UFUNCTION(BlueprintCallable)
	void Invalidate();
	
	UFUNCTION(BlueprintCallable)
	float DealDamage(float Damage, AActor* DamagingActor = nullptr);
	
	UFUNCTION(BlueprintCallable)
	float DealNonLethalDamage(float Damage, AActor* DamagingActor = nullptr);
	
	UFUNCTION(BlueprintCallable)
	bool IsAtLethalThreshold();
    	
	UFUNCTION(BlueprintCallable)
	float Heal(float HealAmount);
    	
	UFUNCTION(BlueprintCallable)
	void SetCurrentAndMaxHealth(float Current, float Max);
	
	UFUNCTION(BlueprintCallable)
	void GetCurrentAndMaxHealth(float& Current, float& Max);
	

	
};
