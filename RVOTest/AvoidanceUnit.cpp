// Fill out your copyright notice in the Description page of Project Settings.

#include "RVOTest.h"
#include "AvoidanceUnit.h"
#include "AvoidanceComponent.h"


UAvoidanceUnit::UAvoidanceUnit(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

//
//UAvoidanceComponent* IAvoidanceUnit::AvoidanceComponent_Implementation()
//{
//	return nullptr;
//}
//
//TArray<TScriptInterface<IAvoidanceUnit>> IAvoidanceUnit::NearbyUnits_Implementation()
//{
//	return TArray<TScriptInterface<IAvoidanceUnit>> {};
//}
//
//FVector2D IAvoidanceUnit::PreferredVelocity_Implementation()
//{
//	return FVector2D{};
//}
//
//void IAvoidanceUnit::SetNewVelocity_Implementation(FVector2D newVelocity)
//{
//	AvoidanceComponent()->UpdateID();
//}

//void IAvoidanceUnit::DoDumbShit_Implementation()
//{
//	UE_LOG(LogTemp, Warning, TEXT("dumb shit _impl"));
//}

UAvoidanceComponent* GetAvoidanceComponent()
{
	unimplemented();
	return nullptr;
}

TArray<APawn*> GetNearbyUnits()
{
	unimplemented();
	return TArray < APawn* > {};
}

void SetNearbyUnits(const TArray<APawn*>& Units)
{
	unimplemented();
	return;
}

void SetNewAvoidanceVelocity(FVector2D newVelocity)
{
	unimplemented();
	return;
}

FVector2D PreferredVelocity()
{
	unimplemented();
	return FVector2D{};
}