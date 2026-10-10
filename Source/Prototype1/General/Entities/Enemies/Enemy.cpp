#include "Enemy.h"

#include "EnemyDataAsset.h"
#include "GCDefaultMacros.h"
#include "GCMovementComponent.h"
#include "HealthComponent.h"
#include "StatComponent.h"

AEnemy::AEnemy()
{

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("Health Component"));
	StatComponent = CreateDefaultSubobject<UStatComponent>(TEXT("Stat Component"));
	
		
	auto* mesh = GetMesh();
	if (!mesh) return;
	
	mesh->bDisableMorphTarget = true;
	mesh->bPerBoneMotionBlur = false;
	mesh->bSkipKinematicUpdateWhenInterpolating = true;
	mesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickMontagesAndRefreshBonesWhenPlayingMontages;
	mesh->bComponentUseFixedSkelBounds = true;
	mesh->bEnableUpdateRateOptimizations = true;
	
	MinimapIcon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Minimap Icon"));
	MinimapIcon->SetupAttachment(mesh);


	
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	InitializeGCComponents();
}

int32 AEnemy::GetCost()
{
	return Cost;
}

int32 AEnemy::GetScoreOnDeath()
{
	return ScoreOnDeath;
}

float AEnemy::ApplySlow(float DesiredSlowEffectiveness)
{
	if (!EnemyConfig || CurrentSlowPercentage >= EnemyConfig->MaxSlowEffectiveness) return 0.0f;
	
	float slowToApply = DesiredSlowEffectiveness;
	if (EnemyConfig->SlowEffectivenessCurve) 
	{
		float alpha = UGCUtilities::GetInterpolationParameter(CurrentSlowPercentage, 0.0f, EnemyConfig->MaxSlowEffectiveness);
		float slowEffectivenessWeight = EnemyConfig->SlowEffectivenessCurve->GetFloatValue(alpha);
		if (slowEffectivenessWeight >= 0.0f)
		{
			slowToApply *= slowEffectivenessWeight;
		} 
	}
	

	
	UGameplayTagsManager& tagMgr = UGameplayTagsManager::Get();
	FGameplayTag MoveSpeedTag = tagMgr.RequestGameplayTag(FName(GC_TAGS_STATS_ENTITY_COMMON_MOVESPEED), true);
	
	bool success{};
	const FRuntimeStats& runtimeMoveSpeedSet = StatComponent->GetRuntimeStatSet(MoveSpeedTag, success);
	if (!success) return 0.0f;
	
	float currentAppliedSlow_Percent = CurrentSlowPercentage/100.0f;
	if (currentAppliedSlow_Percent >= 1.0f) return 0.0f;
	
	// Base move speed with no slow applied onto it
	float baseMoveSpeed = runtimeMoveSpeedSet.CurrentValue / (1.0f - (currentAppliedSlow_Percent));
	
	if (CurrentSlowPercentage + slowToApply >= EnemyConfig->MaxSlowEffectiveness)
	{
		// Desired slow 
		slowToApply = EnemyConfig->MaxSlowEffectiveness - CurrentSlowPercentage;
	}
	
	CurrentSlowPercentage += slowToApply;
	// Should already be pretty much clamped but avoid float creep
	CurrentSlowPercentage = FMath::Clamp(CurrentSlowPercentage, 0.0f, EnemyConfig->MaxSlowEffectiveness);
	
	// Reapply current slow percentage
	float newCurrentMoveSpeed = baseMoveSpeed * (1.0f - (CurrentSlowPercentage / 100.0f));
	
	StatComponent->SetRuntimeStatValue(MoveSpeedTag, newCurrentMoveSpeed, success);
	
	return slowToApply;
	
}

void AEnemy::RemoveSlow(float DesiredSlowRemoval)
{
	if (!EnemyConfig || CurrentSlowPercentage <= 0.0f) return;
	
	float slowToRemove = DesiredSlowRemoval;
	
	UGameplayTagsManager& tagMgr = UGameplayTagsManager::Get();
	FGameplayTag MoveSpeedTag = tagMgr.RequestGameplayTag(FName(GC_TAGS_STATS_ENTITY_COMMON_MOVESPEED), true);
	
	bool success{};
	const FRuntimeStats& runtimeMoveSpeedSet = StatComponent->GetRuntimeStatSet(MoveSpeedTag, success);
	if (!success) return;
	
		
	// Base move speed with no slow applied onto it
	float baseMoveSpeed;
	float currentAppliedSlow_Percent = CurrentSlowPercentage/100.0f;
	if (currentAppliedSlow_Percent >= 1.0f)
	{
		const FStat& baseStat = StatComponent->GetDefaultStatSet(MoveSpeedTag, success);
		if (!success) return;
		baseMoveSpeed = baseStat.StartAmount;
	}
	else
	{
		baseMoveSpeed = runtimeMoveSpeedSet.CurrentValue / (1.0f - (currentAppliedSlow_Percent));
	}

	
	if (CurrentSlowPercentage - slowToRemove <= 0.0f)
	{
		CurrentSlowPercentage = 0.0f; 
	}
	else
	{
		CurrentSlowPercentage -= slowToRemove;
	}
	
	// Should already be pretty much clamped but avoid float creep
	CurrentSlowPercentage = FMath::Clamp(CurrentSlowPercentage, 0.0f, EnemyConfig->MaxSlowEffectiveness);
	
	// Reapply current slow percentage
	float newCurrentMoveSpeed = baseMoveSpeed * (1.0f - (CurrentSlowPercentage / 100.0f));
	
	StatComponent->SetRuntimeStatValue(MoveSpeedTag, newCurrentMoveSpeed, success);

}

void AEnemy::SetTarget_Implementation(AActor* TargetActor)
{
	// Different types handle target differently
	
}


void AEnemy::ChangeStatsForFirstMode()
{
	if (StatComponent)
	{
		float slowPercentage = CurrentSlowPercentage;
		RemoveSlow(slowPercentage);
		StatComponent->ChangeStatsForMode(false);
		ScoreOnDeath /= EnemyConfig->ScoreMultiplierInSecondMode;
		ApplySlow(slowPercentage);
	}
	
	
}

void AEnemy::ChangeStatsForSecondMode()
{
	if (StatComponent)
	{
		float slowPercentage = CurrentSlowPercentage;
		RemoveSlow(slowPercentage);
		StatComponent->ChangeStatsForMode(true);
		ScoreOnDeath *= EnemyConfig->ScoreMultiplierInSecondMode;
		ApplySlow(slowPercentage);;
	}
}

void AEnemy::InitializeGCComponents_Implementation()
{
	
	if (EnemyConfig) {
		StatComponent->StatDataAsset = EnemyConfig;
	}
	
	StatComponent->OnStatChanged.AddUniqueDynamic(this, &AEnemy::OnStatChanged);
	
	HealthComponent->OnDeath_Actor.AddUniqueDynamic(this, &AEnemy::OnDeath);
	HealthComponent->OnDamaged.AddUniqueDynamic(this, &AEnemy::OnDamaged);
	HealthComponent->OnHealthManipulated.AddUniqueDynamic(this, &AEnemy::OnHealthManipulated);
	
	StatComponent->Initialize();
	
	Cost = EnemyConfig->CostToSpawn;
	ScoreOnDeath = EnemyConfig->ScoreOnDeath;
	
}

void AEnemy::PostInitializeComponents()
{
	Super::PostInitializeComponents();


}


void AEnemy::OnStatChanged_Implementation(FGameplayTag Tag, FRuntimeStats Stats)
{
	float Current = Stats.CurrentValue;
	float Max = Stats.MaxValue;
	
	/** 
	 * Very error prone with C strings
	 * Could use macros/blueprints but only reason this is in cpp is for the flying enemy.
	 */
	UGameplayTagsManager& tagMgr = UGameplayTagsManager::Get();
	if (Tag.MatchesTagExact(tagMgr.RequestGameplayTag(GC_TAGS_STATS_ENTITY_COMMON_HEALTH, true)))
	{
		HealthStatChanged(Current, Max);
	}
	else if (Tag.MatchesTagExact(tagMgr.RequestGameplayTag(GC_TAGS_STATS_ENTITY_COMMON_MOVESPEED, true)))
	{
		MoveSpeedStatChanged(Current);
	}
	else if (Tag.MatchesTagExact(tagMgr.RequestGameplayTag(GC_TAGS_STATS_ENTITY_COMMON_DAMAGE, true)))
	{
		DamageStatChanged(Current);
	}
	else if (Tag.MatchesTagExact(tagMgr.RequestGameplayTag(GC_TAGS_STATS_ENTITY_COMMON_ATTACKCOOLDOWN, true)))
	{
		AttackCooldownStatChanged(Current);
	}
	else if (Tag.MatchesTagExact(tagMgr.RequestGameplayTag(GC_TAGS_STATS_ENTITY_COMMON_ATTACKSPEED, true)))
	{
		AttackSpeedStatChanged(Current);
	}
}

void AEnemy::HealthStatChanged_Implementation(float Current, float Max)
{
	if (HealthComponent)
	{
		HealthComponent->SetCurrentAndMaxHealth(Current, Max);
	}
}

void AEnemy::MoveSpeedStatChanged_Implementation(float Current)
{
	if (auto* charComp = GetGCCharacterMovement())
	{
		charComp->MaxSpeed = Current;
	}
}

void AEnemy::DamageStatChanged_Implementation(float Current)
{
	Damage = Current;
}

void AEnemy::AttackCooldownStatChanged_Implementation(float Current)
{
	AttackCooldown = Current;
}

void AEnemy::AttackSpeedStatChanged_Implementation(float Current)
{
	
}

void AEnemy::OnDeath_Implementation(AActor* KillingActor)
{
	LastKillingActor = KillingActor;
}

void AEnemy::FinishDeath_Implementation()
{
	OnEnemyKilled.Broadcast(this, LastKillingActor);
}

void AEnemy::OnDamaged_Implementation()
{
}


void AEnemy::OnHealthManipulated_Implementation()
{
	
}
