// Fill out your copyright notice in the Description page of Project Settings.


#include "GCCharacter.h"

#include "GCMovementComponent.h"
#include "Components/CapsuleComponent.h"


// Sets default values
AGCCharacter::AGCCharacter()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionCylinder"));
	CapsuleComponent->InitCapsuleSize(34.0f, 88.0f);
	CapsuleComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
	
	CapsuleComponent->CanCharacterStepUpOn = ECB_No;
	CapsuleComponent->SetShouldUpdatePhysicsVolume(false);
	CapsuleComponent->SetCanEverAffectNavigation(false);
	CapsuleComponent->bDynamicObstacle = true;
	
	RootComponent = CapsuleComponent;
	
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BodyMesh"));
	Mesh->SetupAttachment(RootComponent);
	
	MovementComponent = CreateDefaultSubobject<UGCMovementComponent>(TEXT("MovementComponent"));
}

// Called when the game starts or when spawned
void AGCCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGCCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

// Called to bind functionality to input
void AGCCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

