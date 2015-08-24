// Fill out your copyright notice in the Description page of Project Settings.

#include "RVOTest.h"
#include "AvoidanceComponent.h"




// Sets default values for this component's properties
UAvoidanceComponent::UAvoidanceComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;

	radius = 42.f;

	MaxVelocity = 600.f;

	MaxAcceleration = 2000.f;
	
	bAutoActivate = true;
	// ...
	
}


// Called when the game starts
void UAvoidanceComponent::BeginPlay()
{
	Super::BeginPlay();
	
	// ...
	pawn = Cast<APawn>(GetOwner());
	mc = pawn->GetMovementComponent();
	

	solver = FBalaRVOModule::Solver();

	currentID = solver->AddAgent();
	nextID = currentID;

	unit = Cast<IAvoidanceUnit>(pawn);
}


// Called every frame
void UAvoidanceComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );
	
	

	FVector loc = pawn->GetActorLocation();
	FVector2D pos{ loc.X, loc.Y };
	
	FVector velocity = pawn->GetVelocity();
	FVector2D vel{ velocity.X, velocity.Y };

	MaxVelocity = mc->GetMaxSpeed();
	
	FVector2D PreferredVelocity = unit->Execute_PreferredVelocity(pawn);

	/*PreferredVelocity = target - pos;
	float distSq = PreferredVelocity.SizeSquared();
	if (distSq > 100.f)
	{
		PreferredVelocity.Normalize();
		PreferredVelocity *= MaxVelocity;
	}
	else
	{
		PreferredVelocity = FVector2D{};
	}*/
	
	// mc->GetMaxSpeed()
	
	Agent agent{ pos.X, pos.Y, vel.X, vel.Y, radius, PreferredVelocity.X, PreferredVelocity.Y, MaxVelocity, MaxAcceleration * DeltaTime };

	for (APawn*  thepawn : unit->Execute_GetNearbyUnits(pawn))
	{
		IAvoidanceUnit* n = Cast<IAvoidanceUnit>(thepawn);
		if (n)
		{
			UAvoidanceComponent* ac = n->Execute_GetAvoidanceComponent(thepawn);
			solver->SetAgentsNearby(currentID, ac->GetCurrentID());
		}
		
	}
	
	solver->GetAgent(nextID) = agent;

	//owner->
	
		//RVOTest::Solver()
	
	
	//unit->Execute_DoDumbShit(pawn);
	
	// ...
}

//void UAvoidanceComponent::SetOwnerVelocity(float x, float y)
//{
//	mc->Velocity = FVector{ x, 0.f, y };
//	
//}

void UAvoidanceComponent::UpdateID()
{
	currentID = nextID;
}

int32 UAvoidanceComponent::GetCurrentID()
{
	return currentID;
}
