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
#define NE_SIDE (84.f)
#define NE_MAXUNITS_IN_CELL (4)

struct Grid
{
	uint8 AgentIds[NE_XCOUNT * NE_YCOUNT * NE_MAXUNITS_IN_CELL];

	//fill everything with -1 : invalid agent id
	void init();

	void clear();

	void setAgent(float x, float y, uint8 id);

	void setAgentNeighbours(uint8 id, ORCASolver* solver);

	uint32 GetIndex(int xcoord, int ycoord);

	uint32 Indexes[CA_MAXAGENTS];

	int num;
	
	

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


