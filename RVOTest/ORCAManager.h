// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "GameFramework/Actor.h"
#include "ORCASolver.h"
#include "KdTree.h"
#include "ORCAManager.generated.h"

class UAvoidanceComponent;


struct CloserAgentComparator
{
	ORCASolver* solver;
	uint16 agentId;
	bool operator()(uint16 leftId, uint16 rightId);

};


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

	void RegisterAvoidanceComponent(UAvoidanceComponent* ac);

	void DeRegisterAvoidanceComponent(UAvoidanceComponent* ac);

	

protected:
	ORCASolver* solver;
	
	TArray<UAvoidanceComponent*> units;



	void bruteCalculateNearest();
	
	CloserAgentComparator comp;
	std::vector<int> closest;

	
	KdTree kdTree;
	std::vector<Point> points;
	void CalculateNearest();
	
};


