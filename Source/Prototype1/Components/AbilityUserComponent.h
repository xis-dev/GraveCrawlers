// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "../DataAssets/EntityAbilityDataAsset.h"
#include "AbilityUserComponent.generated.h"

class UStatComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROTOTYPE1_API UAbilityUserComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAbilityUserComponent();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UEntityAbilityDataAsset> AbilityDataAsset;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<UInputAction*, UAbility*> LiveAbilities;
	
	UPROPERTY(BlueprintReadOnly, Category="Stats")
	TObjectPtr<UAbility> CurrentActiveAbility;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	
	UFUNCTION(BlueprintCallable)
	void InitializeAbilities(class UStatComponent* statComp);
	
	UFUNCTION(BlueprintCallable)
	void SetupInputBindings(class UInputComponent* inputComp);
	
	UFUNCTION(BlueprintCallable)
	void SetActiveAbility(UAbility* Ability);
	
	UFUNCTION(BlueprintCallable)
	UAbility* GetCurrentActiveAbility();
};
