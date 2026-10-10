#include "HealthComponent.h"


UHealthComponent::UHealthComponent()
{
	IsAlive = true;
	LethalDamageThreshold = 1.0f;
}

void UHealthComponent::InitializeHealth(float health)
{
	CurrentHealth = MaxHealth = health;
}

void UHealthComponent::Invalidate()
{
	IsAlive = false;
}

float UHealthComponent::DealDamage(float Damage, AActor* DamagingActor)
{
	if (!IsAlive || Damage <= 0.0f) return 0.0f;
	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.0f, MaxHealth);
	
	OnDamaged.Broadcast();
	OnDamaged_Actor.Broadcast(DamagingActor); // Calling without verifying if the actor exists so listeners dont need to bind to both
	if (CurrentHealth <= 0.00001f)
	{
		OnDeath.Broadcast();
		OnDeath_Actor.Broadcast(DamagingActor);
		IsAlive = false;
	}
	
	return CurrentHealth;
}

float UHealthComponent::DealNonLethalDamage(float Damage, AActor* DamagingActor)
{
	if (!IsAlive || Damage <= 0.0f) return 0.0f;
	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, LethalDamageThreshold, MaxHealth);
	
	OnDamaged.Broadcast();
	OnDamaged_Actor.Broadcast(DamagingActor); // Calling without verifying if the actor exists so listeners dont need to bind to both
	
	return CurrentHealth;
}

bool UHealthComponent::IsAtLethalThreshold()
{
	return CurrentHealth <= LethalDamageThreshold;
}

float UHealthComponent::Heal(float HealAmount)
{
	if (!IsAlive) return 0.0;
	
	CurrentHealth = FMath::Clamp(CurrentHealth + HealAmount, 0.0, MaxHealth);
	
	OnHealed.Broadcast();
	
	return CurrentHealth;
}

void UHealthComponent::SetCurrentAndMaxHealth(float Current, float Max)
{
	if (!IsAlive) return;
	
	MaxHealth = Max;
	CurrentHealth = FMath::Clamp(Current, 0.0, MaxHealth);
	OnHealthManipulated.Broadcast();
}

void UHealthComponent::GetCurrentAndMaxHealth(float& Current, float& Max)
{
	if (!IsAlive) return;
	
	Current = CurrentHealth;
	Max = MaxHealth;
}
