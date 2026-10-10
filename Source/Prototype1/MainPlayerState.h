// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "MainPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class PROTOTYPE1_API AMainPlayerState : public APlayerState
{
	GENERATED_BODY()

private:
	UPROPERTY(Category = PlayerState, BlueprintGetter = GetEXP)
	float m_exp;

	UPROPERTY(Category = PlayerState, BlueprintGetter = GetMaxEXP)
	float m_maxExp{};

	UPROPERTY(Category = PlayerState, BlueprintGetter = GetPlayerLevel)
	int m_level;

public:
	UFUNCTION(BlueprintGetter)
	float GetEXP() const;

	UFUNCTION(BlueprintGetter)
	float GetMaxEXP() const;

	UFUNCTION(BlueprintGetter)
	int GetPlayerLevel() const;
	
};
