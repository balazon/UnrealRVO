// Fill out your copyright notice in the Description page of Project Settings.

#include "RVOTest.h"
#include "BalaCharacter.h"


// Sets default values
ABalaCharacter::ABalaCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	AvoidanceComponent = ObjectInitializer.CreateDefaultSubobject<UAvoidanceComponent>(this, "AvoidanceComponent1");
	
}

// Called when the game starts or when spawned
void ABalaCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABalaCharacter::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

// Called to bind functionality to input
void ABalaCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

	
}

//
//UAvoidanceComponent* ABalaCharacter::GetAvoidanceComponent()
//{
//	return AvoidanceComponent;
//}
//
//TArray<APawn*> ABalaCharacter::GetNearbyUnits()
//{
//	return TArray < APawn* > {};
//}
//
//void ABalaCharacter::SetNearbyUnits(const TArray<APawn*>& Units)
//{
//
//}
//
//void ABalaCharacter::SetNewAvoidanceVelocity(FVector2D newVelocity)
//{
//	
//}
//
//FVector2D ABalaCharacter::PreferredVelocity()
//{
//	return FVector2D{};
//}


//_Implementation


UAvoidanceComponent* ABalaCharacter::GetAvoidanceComponent_Implementation()
{
	return AvoidanceComponent;
}

TArray<APawn*> ABalaCharacter::GetNearbyUnits_Implementation()
{
	return TArray < APawn* > {};
}

void ABalaCharacter::SetNearbyUnits_Implementation(const TArray<APawn*>& Units)
{

}

void ABalaCharacter::SetNewAvoidanceVelocity_Implementation(FVector2D newVelocity)
{
	mc->Velocity = FVector{ newVelocity, 0.f };
	AvoidanceComponent->UpdateID();
}

FVector2D ABalaCharacter::PreferredVelocity_Implementation()
{
	
	return FVector2D{};
}