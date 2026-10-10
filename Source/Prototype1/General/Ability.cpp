// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability.h"

#include "Components/AbilityUserComponent.h"
#include "Components/StatComponent.h"
#include "GameplayTagsManager.h"
#include "BlueprintGameplayTagLibrary.h"
#include "RuntimeStats.h"

// Sets default values
UAbility::UAbility()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryComponentTick.bCanEverTick = true;
	
}

void UAbility::ShowAbilityBlueprint_Implementation()
{
}

bool UAbility::StartCooldown()
{
	UGameplayTagsManager& tagMgr = UGameplayTagsManager::Get();
	FString cooldownStr{".Cooldown"};
	FGameplayTag cooldownTag = tagMgr.RequestGameplayTag(FName(AbilityDA->AbilityStatTag.GetTagName().ToString() + cooldownStr));
	
	bool success;
	float cooldown{};
	const FRuntimeStats& stats = OwnerStatComponent->GetRuntimeStatSet(cooldownTag, success);
	if (success)
	{
		cooldown = stats.CurrentValue;
		GetWorld()->GetTimerManager().SetTimer(CooldownHandle, this, &UAbility::CooldownFinished, cooldown);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to get cooldown tag from ability: %s, Owned by: %s "), *GetName(), *GetOwner()->GetActorNameOrLabel())
	}
	
	OnCooldown = success;
	
	OnAbilityCooldownStarted.Broadcast(AbilityDA->AbilityStatTag, cooldown);
	return success;
}

// Called when the game starts or when spawned
void UAbility::BeginPlay()
{
	Super::BeginPlay();
	
}

void UAbility::Initialize(UStatComponent* statComp)
{
	if (!statComp) {UE_LOG(LogTemp, Error, TEXT("Stat component not found")); return;}
	bool success;
	OwnerStatComponent = statComp;
	OwnerStatComponent->OnStatChanged.AddDynamic(this, &UAbility::OnAbilityStatChanged_Base);
	OwnerStatComponent->RegisterExternalStats(AbilityDA->StatDataAsset->StatMap, success);
	if (!success)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to register external stats"));
		OwnerStatComponent->OnStatChanged.RemoveDynamic(this, &UAbility::OnAbilityStatChanged_Base);
		return;
	}
	BP_OnInitialized(statComp);
}

bool UAbility::	isOnCooldown()
{
	return OnCooldown;
}

void UAbility::CooldownFinished_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Cooldown Over"));
	OnCooldown = false;
	if (IsTryingToUseAbility)
	{
  		UseAbility();
	} 
}

void UAbility::UseAbility_Implementation()
{
	if (OnCooldown)
	{
		UE_LOG(LogTemp, Warning, TEXT("Ability: %s is on cooldown."), *GetName());
		IsTryingToUseAbility = true;
		return;
	}
	//
	// if (!AbilityDA->AbilityNeedsConfirmation)
	// {
	// 	ConfirmAbility();
	// } 
	UE_LOG(LogTemp, Warning, TEXT("Ability used"));
	IsTryingToUseAbility = false;
	
		GetOwner()->GetComponentByClass<UAbilityUserComponent>()->SetActiveAbility(this);
			UE_LOG(LogTemp, Warning, TEXT("Current Active On: %s"), *GetName());
		

}


void UAbility::ConfirmAbility_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Ability confirmed"));
	IsTryingToUseAbility = false;
	if (auto* abilityComp = GetOwner()->FindComponentByClass<UAbilityUserComponent>())
	{
		if (abilityComp->GetCurrentActiveAbility() == this)
		{
			UE_LOG(LogTemp, Warning, TEXT("Current Active Off: %s"), *GetName());
			abilityComp->SetActiveAbility(nullptr);
		}
	}
}

void UAbility::CancelAbility_Implementation()
{
	AbilityWasCanceled = true;
	UE_LOG(LogTemp, Warning, TEXT("Ability Canceled"));
}

void UAbility::OnAbilityStatChanged_Base(FGameplayTag tag, FRuntimeStats stats)
{
	// Does tag match our ability tag, does not need to be exact, will be true if it a child
	if (tag.MatchesTag(AbilityDA->AbilityStatTag))
	{
		OnAbilityStatChanged(tag, stats);	
	}
}

void UAbility::OnAbilityStatChanged_Implementation(FGameplayTag tag, FRuntimeStats stats)
{
	// Default implementation
}	





