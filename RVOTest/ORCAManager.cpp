// Fill out your copyright notice in the Description page of Project Settings.

#include "RVOTest.h"
#include "ORCAManager.h"
#include "EngineUtils.h"
#include "AvoidanceComponent.h"




//fill everything with -1 : invalid agent id
void init();

void clear();

void setAgent(float x, float y, uint8 id);

void setAgentNeighbours(uint8 id);

uint32 Indexes[CA_MAXAGENTS];

int IndexCount;

void Grid::init()
{
	std::fill_n(AgentIds, NE_XCOUNT * NE_YCOUNT * NE_MAXUNITS_IN_CELL, -1);
}

void Grid::clear()
{
	for (int i = 0; i < num; i++)
	{
		AgentIds[Indexes[i]] = -1;
	}
	num = 0;
}

uint32 Grid::GetIndex(int xcoord, int ycoord)
{
	return (xcoord * NE_XCOUNT + ycoord) * NE_MAXUNITS_IN_CELL;
}


void Grid::setAgent(float x, float y, uint8 id)
{
	int xcoord = (x - NE_XMIN) / NE_SIDE;
	int ycoord = (y - NE_YMIN) / NE_SIDE;
	uint32 index = GetIndex(xcoord, ycoord);
	Indexes[num++] = index;
	for (int i = 0; i < NE_MAXUNITS_IN_CELL; i++)
	{
		if (AgentIds[index + i] == -1)
		{
			AgentIds[index + i] = id;
			break;
		}
	}
}

void Grid::setAgentNeighbours(uint8 id, ORCASolver* solver);
{
	solver->
}



// Sets default values
AORCAManager::AORCAManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	FBalaRVOModule::Solver()->ClearAgents();

	
	//unitList.
}

// Called when the game starts or when spawned
void AORCAManager::BeginPlay()
{
	Super::BeginPlay();
	
	
}

// Called every frame
void AORCAManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	ORCASolver* solver = FBalaRVOModule::Solver();

	solver->ClearAgents();

	for (UAvoidanceComponent* av : units)
	{
		if (!av)
		{
			continue;
		}
		FVector2D pos{ av->pawn->GetActorLocation() };

		FVector2D vel{ av->pawn->GetVelocity() };
		
		
		FVector2D PreferredVelocity = av->GetPreferredVelocity();
		
		if (PreferredVelocity.ContainsNaN())
		{
			UE_LOG(LogRVOTest, Warning, TEXT("AvoidanceComponent:: TICK , preferredvelocity: %f %f"), PreferredVelocity.X, PreferredVelocity.Y);
			PreferredVelocity = av->GetPreferredVelocity();

		}
		Agent agent{ pos.X, pos.Y, vel.X, vel.Y, av->radius, PreferredVelocity.X, PreferredVelocity.Y, av->MaxVelocity, av->MaxAcceleration * DeltaTime };
		
		solver->GetAgent(solver->AddAgent()) = agent;

	}


	for (int i = 0; i < units.Num(); i++)
	{
		for (int j = i + 1; j < units.Num(); j++)
		{
			solver->SetAgentsNearby(i, j);
			solver->SetAgentsNearby(j, i);
		}
	}

		
	

	solver->ComputeNewVelocities();

	
	int id = -1;
	for (UAvoidanceComponent* av : units)
	{
		if (!av)
		{
			continue;
		}
		id++;

		Agent& agent = solver->GetAgent(id);

		av->SetNewAvoidanceVelocity(FVector2D{ agent.vx_new, agent.vy_new });

		UE_LOG(LogRVOTest, Warning, TEXT("orcaman:: new vel: %f %f"), agent.vx_new, agent.vy_new);


		if (FVector2D{ agent.vx_new, agent.vy_new }.ContainsNaN())
		{
			UE_LOG(LogRVOTest, Warning, TEXT("BAM ORCAMan szivas"));
			
		}

		
	}
}


void AORCAManager::RegisterAvoidanceComponent(UAvoidanceComponent* ac)
{
	units.AddUnique(ac);
}

void AORCAManager::DeRegisterAvoidanceComponent(UAvoidanceComponent* ac)
{
	units.Remove(ac);
}


void AORCAManager::SimulateORCA()
{
	/*ORCASolver* solver = FBalaRVOModule::Solver();
	solver->ComputeNewVelocities();

	
	for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		AActor* actor = *ActorItr;
		IAvoidanceUnit* unit = Cast<IAvoidanceUnit>(actor);
		if (unit)
		{
			int id = unit->GetAvoidanceComponent()->GetCurrentID();
			
			Agent& agent = solver->GetAgent(id);

			
			unit->Execute_SetNewAvoidanceVelocity(actor, FVector2D{ agent.vx_new, agent.vy_new });
			
			unit->GetAvoidanceComponent()->UpdateID();
		}
	}*/

	
}