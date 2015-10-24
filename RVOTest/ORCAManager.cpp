// Fill out your copyright notice in the Description page of Project Settings.

#include "RVOTest.h"
#include "ORCAManager.h"
#include "EngineUtils.h"
#include "AvoidanceComponent.h"
#include <algorithm>

#include "MathUtils.h"

#include "BalaLib.h"

#include <chrono>
//#include "TimerManager.h"


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
//mylog{ (std::string{ "" } + TCHAR_TO_ANSI(*FPaths::ConvertRelativePathToFull(FPaths::GameLogDir())) + "/log.txt" ).c_str()}
//mylog{ BMU_GET_FILEPATH_AS_C_STRING("log.txt")
AORCAManager::AORCAManager()
{
	
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	FBalaRVOModule::Solver()->ClearAgents();

	comp.solver = FBalaRVOModule::Solver();
	UpdateInterval = 0.1f;
	
	bDynamicRadiusMultiplying = false;
}

// Called when the game starts or when spawned
void AORCAManager::BeginPlay()
{
	Super::BeginPlay();
	
	solver = FBalaRVOModule::Solver();

	solver->dynamicRadiusMultiplying = bDynamicRadiusMultiplying;
	//GetWorld()->gamemode
	//FTimerDelegate::CreateUObject
	//GetWorldTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateUObject(this, &AORCAManager::UpdatingDelegate), UpdateInterval, true);

	
}

void AORCAManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	//GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
}

void AORCAManager::UpdatingDelegate()
{
	SimulateORCA(UpdateInterval);
}

// Called every frame
void AORCAManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	auto start = std::chrono::high_resolution_clock::now();
	
		
	
	SimulateORCA(DeltaTime);
	
	auto elapsed = std::chrono::high_resolution_clock::now() - start;
	//auto time_span = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now() - start);
	long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
	//long long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
	//UE_LOG(LogRVOTest, Warning, TEXT("t %ld"), microseconds);

	
	UBalaLib::LogMessageToFile(FString::Printf(TEXT("%d : %ld\n"), units.Num(), microseconds));
	//mylog << units.Num() << " : " << microseconds << "\n";

}


void AORCAManager::SimulateORCA(float DeltaTime)
{
	
	solver->ClearAgents();

	for (UAvoidanceComponent* av : units)
	{
		
		if (!av || av->IsPendingKill() || av->IsBeingDestroyed() || !av->pawn || av->pawn->IsPendingKill() || av->pawn->IsActorBeingDestroyed())
		{
			continue;
		}
		
		FVector2D pos{ av->pawn->GetActorLocation() };

		FVector2D vel{ av->pawn->GetVelocity() };
		if (vel.SizeSquared() > av->MaxVelocity * av->MaxVelocity)
		{
			vel.Normalize();
			vel *= av->MaxVelocity;
		}
		
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
		if (!av || av->IsPendingKill())
		{
			continue;
		}
		id++;

		Agent& agent = solver->GetAgent(id);

		FVector input{ agent.vx_new - agent.vx, agent.vy_new - agent.vy, 0.f };
		FVector inputDir;
		float inputLength;
		input.ToDirectionAndLength(inputDir, inputLength);

		av->SetNewAvoidanceVelocity(FVector2D{ agent.vx_new, agent.vy_new }, FVector2D{ inputDir }, inputLength / agent.maxAccMagnitude);

		UE_LOG(LogRVOTest, VeryVerbose, TEXT("orcaman:: new vel: %f %f"), agent.vx_new, agent.vy_new);


		if (FVector2D{ agent.vx_new, agent.vy_new }.ContainsNaN())
		{
			UE_LOG(LogRVOTest, VeryVerbose, TEXT("BAM ORCAMan szivas"));

		}


	}


}


void AORCAManager::RegisterAvoidanceComponent(UAvoidanceComponent* ac)
{
	ac->pawn->AddTickPrerequisiteActor(this);
	units.AddUnique(ac);

	/*closest.reserve(units.Num());
	closest.push_back(units.Num() - 1);*/
	
}

void AORCAManager::DeRegisterAvoidanceComponent(UAvoidanceComponent* ac)
{
	UE_LOG(LogRVOTest, Warning, TEXT("orcaman: delete %p"), (void*)ac);

	/*if (!units.Contains(ac))
	{
		return;
	}*/

	ac->pawn->RemoveTickPrerequisiteActor(this);
	units.Remove(ac);

	

	
	
	/*for (int i = 0; i < closest.size(); i++)
	{
		if (closest[i] == units.Num())
		{
			closest.erase(closest.begin() + i);
			break;
		}
	}*/
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