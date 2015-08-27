// Fill out your copyright notice in the Description page of Project Settings.

#include "RVOTest.h"
#include "ORCAManager.h"
#include "EngineUtils.h"
#include "AvoidanceComponent.h"
#include <algorithm>



Grid::Grid()
{
	std::fill_n(AgentIds, NE_XCOUNT * NE_YCOUNT * NE_MAXUNITS_IN_CELL, NE_INVALID_AGENT_ID);

	comp.solver = FBalaRVOModule::Solver();
}


void Grid::clear()
{
	for (int i = 0; i < num; i++)
	{
		AgentIds[Indexes[i]] = NE_INVALID_AGENT_ID;
	}
	num = 0;
}

size_t Grid::GetIndex(int xcoord, int ycoord)
{
	UE_LOG(LogRVOTest, Warning, TEXT("getindex res %d"), (xcoord * NE_YCOUNT + ycoord) * NE_MAXUNITS_IN_CELL);

	return (xcoord * NE_YCOUNT + ycoord) * NE_MAXUNITS_IN_CELL;
}

size_t Grid::GetIndexf(float x, float y)
{
	UE_LOG(LogRVOTest, Warning, TEXT("getindexf x, y : %f %f"), x, y);

	int xcoord = (x - NE_XMIN) / NE_SIDE;
	int ycoord = (y - NE_YMIN) / NE_SIDE;

	UE_LOG(LogRVOTest, Warning, TEXT("getindexf xc yc : %d  %d"), xcoord, ycoord);

	return GetIndex(xcoord, ycoord);
}


void Grid::setAgent(float x, float y, uint16 id)
{
	size_t index = GetIndexf(x, y);
	
	for (int i = 0; i < NE_MAXUNITS_IN_CELL; i++)
	{
		if (AgentIds[index + i] == NE_INVALID_AGENT_ID)
		{
			UE_LOG(LogRVOTest, Warning, TEXT("setAgent x %f , y %f, id %d,  index + i %d + %d"), x, y, id, index, i);

			AgentIds[index + i] = id;
			Indexes[num++] = index + i;
			break;
		}
	}
}

bool CloserAgentComparator::operator()(uint16 leftId, uint16 rightId)
{
	Agent& a = solver->GetAgent(agentId);
	Agent& left = solver->GetAgent(leftId);
	Agent& right = solver->GetAgent(rightId);
	float sqrL1 = (a.x - left.x) * (a.x - left.x) + (a.y - left.y) * (a.y - left.y);
	float sqrL2 = (a.x - right.x) * (a.x - right.x) + (a.y - right.y) * (a.y - right.y);
	return sqrL1 < sqrL2;
}



void Grid::setAgentNeighbours(uint16 id, ORCASolver* solver)
{
	Agent& a = solver->GetAgent(id);
	size_t startIndex = GetIndexf(a.x, a.y) - (NE_YCOUNT + 1) * NE_MAXUNITS_IN_CELL;
	UE_LOG(LogRVOTest, Warning, TEXT("setAgentn id, startindex: %d %d"), id, startIndex);

	std::vector<uint16> closest;
	closest.reserve(10);

	comp.agentId = id;
	/*auto comp = [&](uint8 i, uint8 j) -> bool
	{
		Agent& left = solver->GetAgent(i);
		Agent& right = solver->GetAgent(j);
		float sqrL1 = (a.x - left.x) * (a.x - left.x) + (a.y - left.y) * (a.y - left.y);
		float sqrL2 = (a.x - right.x) * (a.x - right.x) + (a.y - right.y) * (a.y - right.y);
		return sqrL1 < sqrL2;
	};*/

	for (int i = 0; i < 9; i++)
	{
		size_t index = startIndex + ((i % 3) + i / 3 * NE_YCOUNT) * NE_MAXUNITS_IN_CELL;
		for (int j = 0; j < NE_MAXUNITS_IN_CELL; j++)
		{
			if (AgentIds[index + j] == NE_INVALID_AGENT_ID)
			{
				break;
			}
			if (AgentIds[index + j] != id)
			{
				UE_LOG(LogRVOTest, Warning, TEXT("agentids[%d + %d] = %d"), index, j, AgentIds[index + j]);
				//TODODO!!!
				closest.push_back(AgentIds[index + j]);
			}
			
		}
	}

	std::sort(closest.begin(), closest.end(), comp);
	
	for (int16 i = 0; i < CA_MAXNEARBY && i < closest.size(); i++)
	{
		solver->SetAgentsNearby(id, closest[i]);
		UE_LOG(LogRVOTest, Warning, TEXT("closest [%d] = %d"), i, closest[i]);
	}
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

	grid.clear();

	for (int i = 0; i < units.Num(); i++)
	{
		Agent& a = solver->GetAgent(i);
		grid.setAgent(a.x, a.y, i);
	}

	for (int i = 0; i < units.Num(); i++)
	{
		Agent& a = solver->GetAgent(i);
		grid.setAgentNeighbours(i, solver);
	}
/*
	for (int i = 0; i < units.Num(); i++)
	{
		for (int j = i + 1; j < units.Num(); j++)
		{
			solver->SetAgentsNearby(i, j);
			solver->SetAgentsNearby(j, i);
		}
	}*/

		
	

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