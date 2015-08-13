// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "AvoidanceUnit.h"
#include "AvoidanceComponent.h"
#include "BalaCharacter.generated.h"

UCLASS()
class RVOTEST_API ABalaCharacter : public ACharacter, public IAvoidanceUnit
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABalaCharacter(const FObjectInitializer& ObjectInitializer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Avoidance, meta = (DisplayName = "Avoidance Component"))
	UAvoidanceComponent* GetAvoidanceComponent();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Avoidance, meta = (DisplayName = "Get Nearby Units"))
	TArray<APawn*> GetNearbyUnits();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Avoidance, meta = (DisplayName = "Set Nearby Units"))
	void SetNearbyUnits(const TArray<APawn*>& Units);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Avoidance, meta = (DisplayName = "Set New Avoidance Velocity"))
	void SetNewAvoidanceVelocity(FVector2D newVelocity);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Avoidance, meta = (DisplayName = "Preferred Velocity"))
	FVector2D PreferredVelocity();




	virtual UAvoidanceComponent* GetAvoidanceComponent_Implementation();
	
	virtual	TArray<APawn*> GetNearbyUnits_Implementation();
	
		virtual	void SetNearbyUnits_Implementation(const TArray<APawn*>& Units);
	
		virtual	void SetNewAvoidanceVelocity_Implementation(FVector2D newVelocity);
	
		virtual	FVector2D PreferredVelocity_Implementation();


protected:

	UPROPERTY(VisibleAnywhere, Category = Avoidance)
	UAvoidanceComponent* AvoidanceComponent;

	UMovementComponent* mc;
	

	UPROPERTY(VisibleAnywhere, Category = Avoidance)
	FVector2D CurrentTarget;
};
