// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <string>
#include <fstream>

/**
 * 
 */
class BFLog
{
public:
	
	BFLog();

	BFLog(const char* fileName);

	/*template <typename T>
	BFLog& operator << (T&& t);*/

	
	BFLog& operator << (const char* s);

	BFLog& operator << (int number);
private:
	

	std::ofstream out;


};





