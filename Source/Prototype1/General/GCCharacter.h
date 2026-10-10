// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GCCharacter.generated.h"

UCLASS()
class PROTOTYPE1_API AGCCharacter : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AGCCharacter();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UCapsuleComponent* CapsuleComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USkeletalMeshComponent* Mesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UGCMovementComponent* MovementComponent;
	

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	UFUNCTION(BlueprintCallable, Category = "Getters")
	UCapsuleComponent* GetCapsuleComponent(){return CapsuleComponent;}
	
	UFUNCTION(BlueprintCallable, Category = "Getters")
	USkeletalMeshComponent* GetMesh(){return Mesh;}
	
	UFUNCTION(BlueprintCallable, Category = "Getters")
	UGCMovementComponent* GetGCCharacterMovement(){return MovementComponent;}
};
