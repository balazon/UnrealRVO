// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"

#include "AvoidanceComponent.generated.h"

class AORCAManager;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RVOTEST_API UAvoidanceComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAvoidanceComponent();

	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	virtual void Activate(bool bReset = false) override;

	virtual void Deactivate() override;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Avoidance)
	float radius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Avoidance)
	float MaxVelocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Avoidance)
	float MaxAcceleration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Avoidance)
	FVector2D CurrentTarget;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Avoidance)
		float AcceptanceSquared;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Avoidance)
		float SlowdownSquared;
	

	//UFUNCTION(BlueprintCallable, Category = Avoidance, meta = (DisplayName = "Get Current Avoidance Id"))
	//void 

	//TODO move all of AvoidanceUnit, and BalaCharacter RVO here!

	void SetNewAvoidanceVelocity(FVector2D newVelocity, FVector2D inputDir, float inputScale);

	FVector2D GetPreferredVelocity();

	APawn* pawn;

protected:
	bool bDetour;
	bool bStuck;
	float stuckTimer;
	FVector2D TempTarget;
	
	UPawnMovementComponent* mc;

	AORCAManager* manager;

	FVector2D newVel;

	float angle;

	
};
