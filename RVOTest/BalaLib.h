// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "BFLog.h"

//#include "ORCASolver.h"
#include "BalaLib.generated.h"



/**
 * 
 */
UCLASS()
class RVOTEST_API UBalaLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

		/*static void ComputeORCA();*/

		/*static ORCASolver& Solver();

	private:
	static ORCASolver solver;*/

	TODOO HERE -

	static BalaCustomLog(FString string);

private:

	static BFLog mylog;

	
};
