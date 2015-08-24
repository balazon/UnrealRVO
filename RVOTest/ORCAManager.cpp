// Fill out your copyright notice in the Description page of Project Settings.

#include "RVOTest.h"
#include "ORCAManager.h"
#include "EngineUtils.h"
#include "AvoidanceComponent.h"



// Sets default values
AORCAManager::AORCAManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	FBalaRVOModule::Solver()->ClearAgents();
}

// Called when the game starts or when spawned
void AORCAManager::BeginPlay()
{
	Super::BeginPlay();

	

	for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		
		APawn* pawn = Cast<APawn>(*ActorItr);
		IAvoidanceUnit* unit = Cast<IAvoidanceUnit>(pawn);

		
		if (unit)
		{
			AddTickPrerequisiteComponent(unit->Execute_GetAvoidanceComponent(pawn));
			units.Add(pawn);
		}
		
	}
	
}

// Called every frame
void AORCAManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	ORCASolver* solver = FBalaRVOModule::Solver();

	int ai = 0;
	int bi = 1;
	Agent a = solver->GetAgent(ai);
	Agent b = solver->GetAgent(bi);
	solver->ComputeNewVelocities();

	for (APawn* pawn : units)
	{
		IAvoidanceUnit* unit = Cast<IAvoidanceUnit>(pawn);
	
		UAvoidanceComponent* av = unit->Execute_GetAvoidanceComponent(pawn);
		int id = av->GetCurrentID();

		Agent& agent = solver->GetAgent(id);

		if (FVector2D{ agent.vx_new, agent.vy_new }.ContainsNaN())
		{
			/*float x, y;
			float vx, vy;
			float r;
			float vx_pref;
			float vy_pref;
			float vx_new;
			float vy_new;
			int nearbyAgents[CA_MAXNEARBY];
			float ux[CA_MAXNEARBY];
			float uy[CA_MAXNEARBY];
			float maxVelocityMagnitude;
			float maxAccMagnitude;
			*/

			
			UE_LOG(LogRVOTest, Warning, TEXT("Agent a: x %f ,y %f, vx %f , vy %f, r %f , vx_p %f, vy_p %f, %f maxV, %f maxA"), a.x, a.y, a.vx, a.vy, a.r, a.vx_pref, a.vy_pref, a.maxVelocityMagnitude, a.maxAccMagnitude);
			UE_LOG(LogRVOTest, Warning, TEXT("Agent b: x %f ,y %f, vx %f , vy %f, r %f , vx_p %f, vy_p %f, %f maxV, %f maxA"), b.x, b.y, b.vx, b.vy, b.r, b.vx_pref, b.vy_pref, b.maxVelocityMagnitude, b.maxAccMagnitude);
		}
		unit->Execute_SetNewAvoidanceVelocity(pawn, FVector2D{ agent.vx_new, agent.vy_new });

		av->UpdateID();
		
	}
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