// Fill out your copyright notice in the Description page of Project Settings.

#include "RVOTest.h"


//#include "ORCASolver.h"


DEFINE_LOG_CATEGORY(LogRVOTest)



void FBalaRVOModule::StartupModule()
{
	//PointerToBobsMagic = new BobsMagic::MagicalBob();

	//PointerToBobsMagic->ThankEveryoneForReading();
	//solver = new ORCASolver();
}

/**
* Called before the module is unloaded, right before the module object is destroyed.
*/
void FBalaRVOModule::ShutdownModule()
{
	//PointerToBobsMagic->Dispose();
	//PointerToBobsMagic = NULL;
	//delete solver;
}

ORCASolver mysolver;

ORCASolver* FBalaRVOModule::Solver()
{
	return &mysolver;
}






float FBalaRVOModule::getAFloat()
{
	/*Agent agent;
	agent.x = 1.f;
	return agent.x;*/
	//solver.ComputeNewVelocities();

	return 1.f;
}

//Agent* FBalaRVOModule::getAgent()
//{
//	return new Agent();
//}

float FBalaRVOModule::getSum(float a, float b)
{
	return a + b;

}

IMPLEMENT_PRIMARY_GAME_MODULE(FBalaRVOModule, RVOTest, "RVOTest");