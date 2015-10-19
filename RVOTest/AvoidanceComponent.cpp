// Fill out your copyright notice in the Description page of Project Settings.

#include "RVOTest.h"
#include "AvoidanceComponent.h"

#include "EngineUtils.h"

#include "ORCAManager.h"

//#include "UnrealMathUtility.h"

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

	angle = 0.f;
	
	bStuck = false;

	bDetour = false;

	
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
		UE_LOG(LogRVOTest, VeryVerbose, TEXT("You have to place an ORCAManager in the scene for the Aavoidance Component to work"));
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

void UAvoidanceComponent::SetNewAvoidanceVelocity(FVector2D newVelocity, FVector2D inputDir, float inputScale)
{
	UE_LOG(LogRVOTest, VeryVerbose, TEXT("uav component:: set new vel, old:  %f %f, new : %f %f %f"), mc->Velocity.X, mc->Velocity.Y, mc->Velocity.Z, newVelocity.X, newVelocity.Y);

	//mc->Velocity = FVector{ newVelocity, 0.f };
	newVel = newVelocity;
	mc->RequestDirectMove(FVector{ newVelocity, 0.f }, false);
	
	//PostProcessAvoidanceVelocity
	//UCharacterMovementComponent* asd;
	
	
	//UFloatingPawnMovement* floating;

	
	
	//mc->UpdateComponentVelocity();
	
	
	//FVector2D Vpref = GetPreferredVelocity();

	/*
	if (!bDetour && Vpref.SizeSquared() > MaxVelocity  * MaxVelocity / 4.f && newVelocity.SizeSquared() < 1.f)
	{
		bStuck = true;
	}
	else
	{
		bStuck = false;
		stuckTimer = 3.f;
	}*/

	
	
	//UE_LOG(LogRVOTest, Warning, TEXT("newVel, inputdir, inputScale: (%f %f), (%f %f), %f"), newVelocity.X, newVelocity.Y, inputDir.X, inputDir.Y, inputScale);
	//if (inputScale > 0.1)
	//pawn->AddMovementInput(FVector{ inputDir , 0.f}, inputScale);

	
	
	
}

FVector2D UAvoidanceComponent::GetPreferredVelocity()
{
	FVector2D ToTarget{ CurrentTarget - FVector2D{ pawn->GetActorLocation() } };

	float sqrDist = ToTarget.SizeSquared();
	if (sqrDist < 1600.f)
		return FVector2D{};

	
	float k = sqrDist < 10000.f ? sqrDist * 1.19047619e-4f - 0.19047619f : 1.f;

	FVector2D res = ToTarget.GetSafeNormal() * MaxVelocity * k;

	if (res.ContainsNaN())
	{
		UE_LOG(LogRVOTest, VeryVerbose, TEXT("UAvoidanceComp:: GetPreferredV , %f %f"), res.X, res.Y);
	}

	return res;
}


// Called every frame
void UAvoidanceComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );
	

	mc->RequestDirectMove(FVector{ newVel, 0.f }, false);

	return;

	if (bDetour)
	{
		stuckTimer -= DeltaTime;
		if (stuckTimer < 0.f)
		{
			FVector2D temp = CurrentTarget;
			CurrentTarget = TempTarget;
			TempTarget = CurrentTarget;
			bStuck = false;
			stuckTimer = 3.f;
			bDetour = false;
		}
	}

	if (bStuck)
	{
		stuckTimer -= DeltaTime;
		if (stuckTimer < 0.f)
		{
			//FVector::Unit
			
			FMath::VRandCone(-pawn->GetActorForwardVector(), 1.2f);
			
			float translate = FMath::FRandRange(0.8f, 1.2f) * 100.f;
			TempTarget = FVector2D{ pawn->GetActorLocation() } +FVector2D{ FMath::VRandCone(-pawn->GetActorForwardVector(), 1.2f) } *translate;
			
			FVector2D temp = CurrentTarget;
			CurrentTarget = TempTarget;
			TempTarget = CurrentTarget;
			bStuck = false;
			stuckTimer = 3.f;
			bDetour = true;
		}
	}

	
	//pawn->AddMovementInput(inputDir, inputL);
	//mc->AddInputVector
	

	//float angle = atan2f(newVel.Y, newVel.X);
	//pawn->SetActorRotation(FRotator{ 0.f, angle * 6.283f, 0.f });
	// ...
}


