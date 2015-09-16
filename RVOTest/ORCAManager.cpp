// Fill out your copyright notice in the Description page of Project Settings.

#include "RVOTest.h"
#include "ORCAManager.h"
#include "EngineUtils.h"
#include "AvoidanceComponent.h"
#include <algorithm>



bool CloserAgentComparator::operator()(uint16 leftId, uint16 rightId)
{
	Agent& a = solver->GetAgent(agentId);
	Agent& left = solver->GetAgent(leftId);
	Agent& right = solver->GetAgent(rightId);
	float sqrL1 = (a.x - left.x) * (a.x - left.x) + (a.y - left.y) * (a.y - left.y);
	float sqrL2 = (a.x - right.x) * (a.x - right.x) + (a.y - right.y) * (a.y - right.y);
	return sqrL1 < sqrL2;
}




// Sets default values
AORCAManager::AORCAManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	FBalaRVOModule::Solver()->ClearAgents();

	comp.solver = FBalaRVOModule::Solver();
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


	solver = FBalaRVOModule::Solver();

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
			UE_LOG(LogRVOTest, VeryVerbose, TEXT("AvoidanceComponent:: TICK , preferredvelocity: %f %f"), PreferredVelocity.X, PreferredVelocity.Y);
			PreferredVelocity = av->GetPreferredVelocity();

		}
		Agent agent{ pos.X, pos.Y, vel.X, vel.Y, av->radius, PreferredVelocity.X, PreferredVelocity.Y, av->MaxVelocity, av->MaxAcceleration * DeltaTime };
		
		solver->GetAgent(solver->AddAgent()) = agent;

	}

	CalculateNearest();

		
	

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

		UE_LOG(LogRVOTest, VeryVerbose, TEXT("orcaman:: new vel: %f %f"), agent.vx_new, agent.vy_new);


		if (FVector2D{ agent.vx_new, agent.vy_new }.ContainsNaN())
		{
			UE_LOG(LogRVOTest, VeryVerbose, TEXT("BAM ORCAMan szivas"));
			
		}

		
	}
}


void AORCAManager::RegisterAvoidanceComponent(UAvoidanceComponent* ac)
{
	units.AddUnique(ac);
	closest.reserve(units.Num());
	closest.push_back(units.Num() - 1);
	
}

void AORCAManager::DeRegisterAvoidanceComponent(UAvoidanceComponent* ac)
{
	units.Remove(ac);
	for (int i = 0; i < closest.size(); i++)
	{
		if (closest[i] == units.Num())
		{
			closest.erase(closest.begin() + i);
			break;
		}
	}
}


void AORCAManager::SimulateORCA()
{
	

	
}

void AORCAManager::bruteCalculateNearest()
{


	for (int i = 0; i < units.Num(); i++)
	{

		comp.agentId = i;

		std::sort(closest.begin(), closest.end(), comp);
		for (int j = 0; j < CA_MAXNEARBY + 1 && j < closest.size(); j++)
		{
			if (closest[j] == i)
			{
				continue;
			}
			solver->SetAgentsNearby(i, closest[j]);
		}


	}
}

void AORCAManager::CalculateNearest()
{
	points.reserve(units.Num());
	points.clear();
	
	for (int i = 0; i < units.Num(); i++)
	{
		//points.push_back(Point{solver->})
		Agent& a = solver->GetAgent(i);
		Point p{ a.x, a.y, i };
		points.push_back(p);
	}
	kdTree.build(points);
	kdTree.updateNeighbours(solver);
}


void AORCAManager::SetDebugging(bool on)
{
	solver->SetDebugging(on);
}