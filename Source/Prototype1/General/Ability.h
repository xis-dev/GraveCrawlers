// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "DataAssets/EntityDataAsset.h"
#include "DataAssets/AbilityDataAsset.h"
#include "Ability.generated.h"

struct FRuntimeStats;
class UStatComponent;
struct FGameplayTag;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAbilityCooldownStarted, FGameplayTag, AbiltyTag, float, Cooldown);

UCLASS(Blueprintable )
class PROTOTYPE1_API UAbility : public UActorComponent
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	UAbility();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Data")
	TObjectPtr<UAbilityDataAsset> AbilityDA;
	
	
	UPROPERTY(BlueprintReadOnly, Category="Stats")
	TObjectPtr<UStatComponent> OwnerStatComponent;

	

protected:
	
	UPROPERTY(BlueprintReadOnly)
	FTimerHandle CooldownHandle;
	
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Events")
	FAbilityCooldownStarted OnAbilityCooldownStarted;
	
	UPROPERTY(BlueprintReadOnly)
	bool OnCooldown{};
	
	UPROPERTY(BlueprintReadOnly)
	bool IsTryingToUseAbility;
	
	UPROPERTY(BlueprintReadOnly)
	bool AbilityWasCanceled;
	
	// TODO: Remove from data asset and move here?? 
	// Display the blueprint of the ability in the world, ie slow area radius prior to throwing
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void ShowAbilityBlueprint();
	
	UFUNCTION(BlueprintCallable)
	bool StartCooldown();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	

public:	
	
	UFUNCTION(BlueprintCallable)
	void Initialize(UStatComponent* statComp);
	
	UFUNCTION(BlueprintCallable)
	bool isOnCooldown();
	
	UFUNCTION(BlueprintNativeEvent)
	void CooldownFinished();
	virtual void CooldownFinished_Implementation();
	
	UFUNCTION(BlueprintNativeEvent)
	void UseAbility();
	virtual void UseAbility_Implementation();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ConfirmAbility();
	virtual void ConfirmAbility_Implementation();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void CancelAbility();
	virtual void CancelAbility_Implementation();
	
private:
	// Base function bound to owning stat component, confirms the tag is the right ability tag calls OnAbilityStatChanged, no need for ability to receive other stat changes, 
	UFUNCTION(BlueprintCallable, Category = "Stats")
	void OnAbilityStatChanged_Base(FGameplayTag tag, FRuntimeStats stats);
	
protected:
	// Ability stat changed, broadcasted from base only when the tag is a subset of ability tag
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Stats")
	void OnAbilityStatChanged(FGameplayTag tag, FRuntimeStats stats);
	
	// Ability stat changed, broadcasted from base only when the tag is a subset of ability tag
	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnInitialized(UStatComponent* statComp);
	
};
