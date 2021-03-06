// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "AIController.h"
#include "AI/Navigation/NavigationSystem.h"
#include "AI/Navigation/NavigationPath.h"


#include "AvoidanceComponent.generated.h"

class AORCAManager;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RVOTEST_API UAvoidanceComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAvoidanceComponent();

	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	//virtual void InitializeComponent() override;

	virtual void Activate(bool bReset = false) override;

	//virtual void OnRegister() override;

	//virtual void Deactivate() override;

	//virtual void OnComponentDestroyed() override;

	//virtual void DestroyComponent(bool bPromoteChildren = false) override;

	//virtual void OnUnregister() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Avoidance)
	float radius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Avoidance)
	float MaxVelocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Avoidance)
	float MaxAcceleration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Avoidance)
	FVector2D CurrentTarget;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Avoidance)
		float AcceptanceSquared;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Avoidance)
		float SlowdownSquared;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Avoidance)
		bool bUseAITargetLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Avoidance)
		FVector GlobalTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Avoidance)
	bool bUsePathFinding;

	void SetNewAvoidanceVelocity(FVector2D newVelocity, FVector2D inputDir, float inputScale);

	FVector2D GetPreferredVelocity();

	APawn* pawn;
	AAIController* aiController;

	UFUNCTION(BlueprintCallable, Category = "Bala Library")
	void MoveToGlobalTarget();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Avoidance)
		UNavigationPath* navpath;
protected:


	bool pathFollowSlowdown;

	uint8 pathIndex;
	TArray<FVector> path;

	void SetGlobalTarget(FVector target);

	UPawnMovementComponent* mc;

	AORCAManager* manager;

	FVector2D newVel;

	float angle;

	
};
