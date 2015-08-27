// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "GameFramework/Actor.h"
#include "ORCASolver.h"
#include "ORCAManager.generated.h"

class UAvoidanceComponent;

#define NE_XCOUNT (200)
#define NE_YCOUNT (200)
#define NE_XMIN (-8000.f)
#define NE_YMIN (-8000.f)
#define NE_SIDE (168.f)
#define NE_MAXUNITS_IN_CELL (16)
#define NE_INVALID_AGENT_ID (65535)

struct CloserAgentComparator
{
	ORCASolver* solver;
	uint16 agentId;
	bool operator()(uint16 leftId, uint16 rightId);

};

struct Grid
{
	//fill everything with the invalid agent id:  NE_INVALID_AGENT_ID
	Grid();
	
	uint16 AgentIds[NE_XCOUNT * NE_YCOUNT * NE_MAXUNITS_IN_CELL];

	void clear();

	void setAgent(float x, float y, uint16 id);

	void setAgentNeighbours(uint16 id, ORCASolver* solver);

	size_t GetIndex(int xcoord, int ycoord);
	size_t GetIndexf(float x, float y);

	size_t Indexes[CA_MAXAGENTS];

	int num;
	
	
	CloserAgentComparator comp;
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

	
	TArray<UAvoidanceComponent*> units;

	Grid grid;
	
	
};


