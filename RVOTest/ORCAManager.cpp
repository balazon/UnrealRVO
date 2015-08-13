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
			AddTickPrerequisiteComponent(unit->GetAvoidanceComponent());
			units.Add(pawn);
		}
		
	}
	
}

// Called every frame
void AORCAManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	ORCASolver* solver = FBalaRVOModule::Solver();
	solver->ComputeNewVelocities();

	for (APawn* pawn : units)
	{
		IAvoidanceUnit* unit = Cast<IAvoidanceUnit>(pawn);
	

		int id = unit->GetAvoidanceComponent()->GetCurrentID();

		Agent& agent = solver->GetAgent(id);

		unit->Execute_SetNewAvoidanceVelocity(pawn, FVector2D{ agent.vx_new, agent.vy_new });

		unit->GetAvoidanceComponent()->UpdateID();
		
	}
}




void AORCAManager::SimulateORCA()
{
	ORCASolver* solver = FBalaRVOModule::Solver();
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
	}

	
}