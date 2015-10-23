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

	AcceptanceSquared = 1600.f;

	SlowdownSquared = 10000.f;
	
	bAutoActivate = true;

	angle = 0.f;
	
	bStuck = false;

	bDetour = false;

	bUseAITargetLocation = true;
	
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
	

	aiController = Cast<AAIController>(pawn->Controller);
	

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
	FVector2D ToTarget;
	if (bUseAITargetLocation && aiController)
	{
		ToTarget = FVector2D{ aiController->GetPathFollowingComponent()->GetCurrentTargetLocation() };
		//FVector2D actLoc {pawn->GetActorLocation() };
		//UE_LOG(LogRVOTest, Warning, TEXT("UAv::prefv GetCurrentTargetLocation, %f %f, actloc: %f %f"), CurrentTarget.X, CurrentTarget.Y, actLoc.X, actLoc.Y);

	}
	else
	{
		ToTarget = FVector2D{ CurrentTarget - FVector2D{ pawn->GetActorLocation() } };
	}
	
	

	float sqrDist = ToTarget.SizeSquared();

	
	if (sqrDist < AcceptanceSquared)
		return FVector2D::ZeroVector;

	float m = 1.f / (SlowdownSquared - AcceptanceSquared);

	float b = -m * AcceptanceSquared;

	float k = sqrDist < SlowdownSquared ? sqrDist * m + b : 1.f;
	if (k < SMALL_NUMBER)
	{
		return FVector2D::ZeroVector;
	}

	FVector2D res = ToTarget.GetSafeNormal() * MaxVelocity * k;
	
	if (res.ContainsNaN())
	{
		UE_LOG(LogRVOTest, VeryVerbose, TEXT("UAvoidanceComp:: GetPreferredV , %f %f"), res.X, res.Y);
	}

	//pawn->s
	//pawn->Controller->
	//ACharacter* c;
	//c->AIControllerClass->
	

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


