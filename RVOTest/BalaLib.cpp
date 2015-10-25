// Fill out your copyright notice in the Description page of Project Settings.

#include "RVOTest.h"
#include "BalaLib.h"

#include "MathUtils.h"

//
//
//ORCASolver& UBalaLib::Solver()
//{
//	return solver;
//}
//
//void BalaLib::ComputeORCA()
//{
//	ORCASolver* solver = FBalaRVOModule::Solver();
//	solver->ComputeNewVelocities();
//}

BFLog UBalaLib::mylog = BFLog{ BMU_GET_FILEPATH_AS_C_STRING("log.txt") };

BFLog UBalaLib::mylog2 = BFLog{ BMU_GET_FILEPATH_AS_C_STRING("logdeltaandfps.txt") };


void UBalaLib::LogMessageToFile(FString string)
{
	mylog << BMU_FSTRING_TO_C_STRING(string);
}

void UBalaLib::LogMessageToOtherFile(FString string)
{
	mylog2 << BMU_FSTRING_TO_C_STRING(string);
	
}
