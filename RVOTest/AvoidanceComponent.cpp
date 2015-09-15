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

	angle = 0.f;
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

void UAvoidanceComponent::SetNewAvoidanceVelocity(FVector2D newVelocity)
{
	UE_LOG(LogRVOTest, VeryVerbose, TEXT("uav component:: set new vel, old:  %f %f, new : %f %f %f"), mc->Velocity.X, mc->Velocity.Y, mc->Velocity.Z, newVelocity.X, newVelocity.Y);

	mc->Velocity = FVector{ newVelocity, 0.f };
	mc->UpdateComponentVelocity();
	
	FVector v_old{ mc->Velocity };
	FVector v_new{ newVelocity, 0.f };
	FVector fromOldToNew = v_new - v_old;
	FVector dir;
	float length;
	v_new.ToDirectionAndLength(dir, length);

	

	//UE_LOG(LogRVOTest, Warning, TEXT("dir: %f %f"), dir.X, dir.Y);


	//UCharacterMovementComponent* cmc = Cast<UCharacterMovementComponent>(mc);
	
	//ACharacter* mychar = Cast<ACharacter>(pawn);
	//mychar->laun
	//mc->AddInputVector(dir);

	//pawn->AddMovementInput(dir, length);
	
	float v_angle = atan2f(dir.Y, dir.X);
	if (length > 45.f)
	{
		angle = v_angle;
	}
	if (length < 5.f)
	{
		v_angle = angle;
	}
	
	//UE_LOG(LogRVOTest, Warning, TEXT("angles: %f %f"), v_angle, angle2);
	
	pawn->SetActorRotation(FRotator{ 0.f, v_angle * 57.296f, 0.f });

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
		UE_LOG(LogRVOTest, VeryVerbose, TEXT("UAvoidanceComp:: GetPreferredV , %f %f"), res.X, res.Y);
	}

	return ToTarget / sqrtf(sqrDist) * MaxVelocity * k;
}


// Called every frame
void UAvoidanceComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );
	
	SetNewAvoidanceVelocity(newVel);

	//float angle = atan2f(newVel.Y, newVel.X);
	//pawn->SetActorRotation(FRotator{ 0.f, angle * 6.283f, 0.f });
	// ...
}


