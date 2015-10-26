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

	AcceptanceSquared = 400.f;

	SlowdownSquared = 10000.f;
	
	bAutoActivate = true;

	angle = 0.f;
	
	bUseAITargetLocation = false;

	newVel = FVector2D::ZeroVector;
	
	pathFollowSlowdown = true;

	bUsePathFinding = false;

	
}





void UAvoidanceComponent::Activate(bool bReset)
{
	Super::Activate();
	
	//UE_LOG(LogRVOTest, Warning, TEXT("uav: activate %p"), (void*)this); 


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


void UAvoidanceComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	//UE_LOG(LogRVOTest, Warning, TEXT("uav endplay "));

	manager->DeRegisterAvoidanceComponent(this);

}

// Called when the game starts
void UAvoidanceComponent::BeginPlay()
{
	Super::BeginPlay();
	
	//UE_LOG(LogRVOTest, Warning, TEXT("uav beginplay "));
	

	aiController = Cast<AAIController>(pawn->Controller);

	if (bUsePathFinding)
	{
		MoveToGlobalTarget();
	}
	
}

void UAvoidanceComponent::SetNewAvoidanceVelocity(FVector2D newVelocity, FVector2D inputDir, float inputScale)
{
	UE_LOG(LogRVOTest, VeryVerbose, TEXT("uav component:: set new vel, old:  %f %f, new : %f %f %f"), mc->Velocity.X, mc->Velocity.Y, mc->Velocity.Z, newVelocity.X, newVelocity.Y);

	newVel = newVelocity;	
	//UCharacterMovementComponent* ucharmc;
	
	mc->RequestDirectMove(FVector{ newVelocity, 0.f }, false);
	
	
	
}

FVector2D UAvoidanceComponent::GetPreferredVelocity()
{
	FVector2D ToTarget;
	if (bUseAITargetLocation && aiController)
	{
		ToTarget = FVector2D{ aiController->GetPathFollowingComponent()->GetCurrentTargetLocation() };
		//FVector2D actLoc {pawn->GetActorLocation() };
		//UE_LOG(LogRVOTest, Warning, TEXT("UAv::prefv GetCurrentTargetLocation, %f %f, actloc: %f %f"), CurrentTarget.X, CurrentTarget.Y, actLoc.X, actLoc.Y);
		//UE_LOG(LogRVOTest, Warning, TEXT("UAv::Totarget: %f %f"), ToTarget.X, ToTarget.Y);

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

	float k = (sqrDist < SlowdownSquared && pathFollowSlowdown) ? sqrDist * m + b : 1.f;
	if (k < SMALL_NUMBER)
	{
		return FVector2D::ZeroVector;
	}

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

	if (FMath::FRand() < 0.002f)
	{
		FVector2D ToTarget = FVector2D{ CurrentTarget - FVector2D{ pawn->GetActorLocation() } }.GetSafeNormal();
		UE_LOG(LogRVOTest, Warning, TEXT("ToTarget Dir: %.2f %.2f"), ToTarget.X, ToTarget.Y);
	}
	



	if (bUsePathFinding && pathIndex < path.Num() - 1)
	{
		if (FVector2D::DistSquared(CurrentTarget, FVector2D{ pawn->GetActorLocation() }) < AcceptanceSquared)
		{
			UE_LOG(LogRVOTest, Warning, TEXT("tick, next target"));

			pathIndex++;
			CurrentTarget = FVector2D{ path[pathIndex] };
			pathFollowSlowdown = (pathIndex == path.Num() - 1);
		}
	}
	
	//float angle = atan2f(newVel.Y, newVel.X);
	//pawn->SetActorRotation(FRotator{ 0.f, angle * 6.283f, 0.f });
	// ...
}

void UAvoidanceComponent::SetGlobalTarget(FVector target)
{
	navpath = UNavigationSystem::FindPathToLocationSynchronously(this, pawn->GetActorLocation(), target, pawn);

	if (GetWorld()->GetNavigationSystem() == nullptr)
	{
		UE_LOG(LogRVOTest, Warning, TEXT("navsys null"));
		return;
	}
	if (GetWorld()->GetNavigationSystem()->GetMainNavData() == nullptr)
	{
		UE_LOG(LogRVOTest, Warning, TEXT("mainnavdata null"));
		return;
	}
	path = navpath->PathPoints;
	
	for (FVector point : path)
	{
		FVector2D p{ point };
		UE_LOG(LogRVOTest, Warning, TEXT("uav::setglob p: %f %f"), p.X, p.Y);

	}

	CurrentTarget = path.Num() > 0 ? FVector2D{ path[0] } : FVector2D{ pawn->GetActorLocation() };
	
	pathFollowSlowdown = (path.Num() <= 1);

	pathIndex = 0;

	//auto points = path->GetPathPoints();

	//UNavigationSystem* NavSys = aiController ? UNavigationSystem::GetCurrent(aiController->GetWorld()) : nullptr;

	//const ANavigationData* NavData = NavSys->GetNavDataForProps(aiController->GetNavAgentPropertiesRef());
	//if (NavData)
	//{
	//	FPathFindingQuery Query(aiController, *NavData, aiController->GetNavAgentLocation(), target);
	//	FPathFindingResult Result = NavSys->FindPathSync(Query);
	//	auto path = Result.Path;
	//	auto points = path->GetPathPoints();
	//	for (auto point : points)
	//	{
	//		
	//	}
	//	auto point = points[0];
	//	points.
	//	//point.
	//	if (Result.IsSuccessful())
	//	{
	//		PFollowComp->RequestMove(Result.Path, NULL);
	//	}
	//	else if (PFollowComp->GetStatus() != EPathFollowingStatus::Idle)
	//	{
	//		PFollowComp->AbortMove(TEXT("Aborting move due to new move request failing to generate a path"), FAIRequestID::AnyRequest);
	//		PFollowComp->SetLastMoveAtGoal(false);
	//	}
	//}
}


void UAvoidanceComponent::MoveToGlobalTarget()
{
	SetGlobalTarget(GlobalTarget);
}