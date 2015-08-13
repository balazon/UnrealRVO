// Fill out your copyright notice in the Description page of Project Settings.

#include "RVOTest.h"
#include "BalaCharacter.h"


// Sets default values
ABalaCharacter::ABalaCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->AttachTo(RootComponent);
	CameraBoom->bAbsoluteRotation = true; // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->RelativeRotation = FRotator(-60.f, 0.f, 0.f);
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->AttachTo(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm


	AvoidanceComponent = CreateDefaultSubobject<UAvoidanceComponent>(TEXT("AvoidanceComponent1"));
	
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
	return NearbyUnits;
}

void ABalaCharacter::SetNearbyUnits_Implementation(const TArray<APawn*>& Units)
{
	NearbyUnits = Units;
}

void ABalaCharacter::SetNewAvoidanceVelocity_Implementation(FVector2D newVelocity)
{
	mc->Velocity = FVector{ newVelocity, 0.f };
	AvoidanceComponent->UpdateID();
}

FVector2D ABalaCharacter::PreferredVelocity_Implementation()
{
	FVector2D ToTarget{ CurrentTarget - FVector2D{ GetActorLocation() } };
	
	float sqrDist = ToTarget.SizeSquared();
	if (sqrDist < 100.f)
		return FVector2D{};

	return ToTarget / sqrtf(sqrDist) * AvoidanceComponent->MaxVelocity;
}