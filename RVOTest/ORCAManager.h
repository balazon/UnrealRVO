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

	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//UFUNCTION(BlueprintCallable, Category = Avoidance, meta = (DisplayName = "Simulate ORCA"))
	void SimulateORCA(float DeltaTime);

	void RegisterAvoidanceComponent(UAvoidanceComponent* ac);

	void DeRegisterAvoidanceComponent(UAvoidanceComponent* ac);

	UFUNCTION(BlueprintCallable, Category = Avoidance, meta = (DisplayName = "Set debug mode"))
	void SetDebugging(bool on);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Avoidance, meta = (DisplayName = "Update Interval in seconds"))
	float UpdateInterval;

protected:
	FTimerHandle TimerHandle;
	void UpdatingDelegate();

	ORCASolver* solver;
	
	TArray<UAvoidanceComponent*> units;



	void bruteCalculateNearest();
	
	CloserAgentComparator comp;
	std::vector<int> closest;

	
	KdTree kdTree;
	std::vector<Point> points;
	void CalculateNearest();
	
};


