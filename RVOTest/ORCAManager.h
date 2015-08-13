// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "AvoidanceUnit.h"
#include "GameFramework/Actor.h"
#include "ORCAManager.generated.h"

/**
 * 
 */
UCLASS()
class RVOTEST_API AORCAManager : public AActor
{
	GENERATED_BODY()
public:
	AORCAManager();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable, Category = Avoidance, meta = (DisplayName = "Simulate ORCA"))
	void SimulateORCA();

protected:

	TArray<APawn*> units;
};


