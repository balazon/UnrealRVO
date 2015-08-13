// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#ifndef __RVOTEST_H__
#define __RVOTEST_H__

//#include "Engine.h"

#include "EngineMinimal.h"
#include "ORCASolver.h"

DECLARE_LOG_CATEGORY_EXTERN(LogRVOTest, Log, All);

class FBalaRVOModule : public FDefaultGameModuleImpl
{
	//BobsMagic::MagicalBob *PointerToBobsMagic;

	/**
	* Called right after the module DLL has been loaded and the module object has been created
	*/
	virtual void StartupModule() override;

	/**
	* Called before the module is unloaded, right before the module object is destroyed.
	*/
	virtual void ShutdownModule();


public:

	static float getAFloat();



	static ORCASolver* Solver();

private:
	//static ORCASolver mysolver;

	//static Agent* getAgent();


	static float getSum(float a, float b);
	//static void SetAgentParams(int id, float x, float y, float vx, float vy, float r, float vx_pref, float vy_pref, float maxVelocityMagnitude, float maxAccMagnitude);

};

#endif