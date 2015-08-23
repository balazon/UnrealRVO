// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "AvoidanceUnit.h"
#include "AvoidanceComponent.generated.h"


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

		
	void UpdateID();

	UFUNCTION(BlueprintCallable, Category = Avoidance, meta = (DisplayName = "Get Current Avoidance Id"))
	int32 GetCurrentID();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Avoidance)
		float radius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Avoidance)
		float MaxVelocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Avoidance)
		float MaxAcceleration;

protected:

	IAvoidanceUnit* unit;
	APawn* pawn;
	UPawnMovementComponent* mc;

	ORCASolver* solver;

	int currentID;
	int nextID;

	
	
	
};
