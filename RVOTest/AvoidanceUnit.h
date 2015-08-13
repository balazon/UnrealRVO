// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "AvoidanceUnit.generated.h"


class UAvoidanceComponent;

/**
 * 
 */
UINTERFACE(Blueprintable, MinimalAPI)
class UAvoidanceUnit : public UInterface
{
	GENERATED_UINTERFACE_BODY()

	
};



class RVOTEST_API IAvoidanceUnit
{

	

	GENERATED_IINTERFACE_BODY()

public:

	
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent,  Category = Avoidance, meta = (DisplayName = "Avoidance Component"))
	UAvoidanceComponent* GetAvoidanceComponent();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = Avoidance, meta = (DisplayName = "Get Nearby Units"))
	TArray<APawn*> GetNearbyUnits();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = Avoidance, meta = (DisplayName = "Set Nearby Units"))
	void SetNearbyUnits(const TArray<APawn*>& Units);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = Avoidance, meta = (DisplayName = "Set New Avoidance Velocity"))
	void SetNewAvoidanceVelocity(FVector2D newVelocity);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = Avoidance, meta = (DisplayName = "Preferred Velocity"))
	FVector2D PreferredVelocity();

	/*UFUNCTION(BlueprintImplementableEvent, Category = DumbShit, meta = (DisplayName = "On Do Dumb Shit"))
		void DoDumbShit();*/


	
	
	//virtual UAvoidanceComponent* AvoidanceComponent_Implementation();
	/*virtual TArray<TScriptInterface<IAvoidanceUnit>> NearbyUnits_Implementation();
	virtual FVector2D PreferredVelocity_Implementation();
	virtual void SetNewVelocity_Implementation(FVector2D newVelocity);*/
};
