// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityUserComponent.h"

#include "EnhancedInputComponent.h"

// Sets default values for this component's properties
UAbilityUserComponent::UAbilityUserComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UAbilityUserComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UAbilityUserComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UAbilityUserComponent::InitializeAbilities(class UStatComponent* statComp)
{
	if (!AbilityDataAsset || !statComp) return;
	for (auto [action, abilityType] : AbilityDataAsset->Abilities)
	{
		if (auto* ability = Cast<UAbility>(GetOwner()->
											AddComponentByClass(abilityType, false, FTransform::Identity, false)))
		{
			ability->Initialize(statComp);
			LiveAbilities.Add(action, ability);
		}
	}
	
	SetupInputBindings(GetOwner()->InputComponent);
}

void UAbilityUserComponent::SetupInputBindings(class UInputComponent* inputComp)
{
	for (auto [action, ability] : LiveAbilities)
	{
		if (UEnhancedInputComponent* enhancedInputComp = Cast<UEnhancedInputComponent>(inputComp))
		{
			enhancedInputComp->BindAction(action, ETriggerEvent::Started, ability, &UAbility::UseAbility);
			if (ability->AbilityDA->AbilityNeedsConfirmation)
			{
				enhancedInputComp->BindAction(action, ETriggerEvent::Completed, ability, &UAbility::ConfirmAbility);
				enhancedInputComp->BindAction(action, ETriggerEvent::Canceled, ability, &UAbility::ConfirmAbility);
				
			}
		}
	}
}

void UAbilityUserComponent::SetActiveAbility(UAbility* Ability)
{
	CurrentActiveAbility = Ability;
}

UAbility* UAbilityUserComponent::GetCurrentActiveAbility()
{
	return CurrentActiveAbility;
}

