// Fill out your copyright notice in the Description page of Project Settings.


#include "MainPlayerState.h"

float AMainPlayerState::GetEXP() const
{
	return m_exp;
}

float AMainPlayerState::GetMaxEXP() const
{
	return m_maxExp;
}

int AMainPlayerState::GetPlayerLevel() const
{
	return m_level;
}


