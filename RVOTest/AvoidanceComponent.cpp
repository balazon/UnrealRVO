// Fill out your copyright notice in the Description page of Project Settings.

#include "RVOTest.h"
#include "AvoidanceComponent.h"

#include "EngineUtils.h"

#include "ORCAManager.h"

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

void UAvoidanceComponent::Activate(bool bReset)
{
	Super::Activate();
	
	pawn = Cast<APawn>(GetOwner());

	mc = pawn->GetMovementComponent();

	
	for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		AActor* actor = *ActorItr;
		AORCAManager* orcaTest = Cast<AORCAManager>(actor);
		if (orcaTest)
		{
			manager = orcaTest;
			break;
		}
	}
	if (!manager)
	{
		UE_LOG(LogRVOTest, Warning, TEXT("You have to place an ORCAManager in the scene for the Aavoidance Component to work"));
	}
	else
	{
		manager->RegisterAvoidanceComponent(this);
	}

	
}

void UAvoidanceComponent::Deactivate()
{
	Super::Deactivate();
	//unit is dead etc
	manager->DeRegisterAvoidanceComponent(this);
}


// Called when the game starts
void UAvoidanceComponent::BeginPlay()
{
	Super::BeginPlay();
	

	

	// ...
}

void UAvoidanceComponent::SetNewAvoidanceVelocity(FVector2D newVelocity)
{
	UE_LOG(LogRVOTest, Warning, TEXT("uav component:: set new vel, old:  %f %f, new : %f %f %f"), mc->Velocity.X, mc->Velocity.Y, mc->Velocity.Z, newVelocity.X, newVelocity.Y);

	mc->Velocity = FVector{ newVelocity, 0.f };
	mc->UpdateComponentVelocity();

	newVel = newVelocity;
}

FVector2D UAvoidanceComponent::GetPreferredVelocity()
{
	FVector2D ToTarget{ CurrentTarget - FVector2D{ pawn->GetActorLocation() } };

	float sqrDist = ToTarget.SizeSquared();
	if (sqrDist < 1600.f)
		return FVector2D{};

	float k = sqrDist < 10000.f ? sqrDist / 10000.f : 1.f;

	FVector2D res = ToTarget / sqrtf(sqrDist) * MaxVelocity;

	if (res.ContainsNaN())
	{
		UE_LOG(LogRVOTest, Warning, TEXT("UAvoidanceComp:: GetPreferredV , %f %f"), res.X, res.Y);
	}

	return ToTarget / sqrtf(sqrDist) * MaxVelocity * k;
}


// Called every frame
void UAvoidanceComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );
	
	SetNewAvoidanceVelocity(newVel);
	// ...
}


