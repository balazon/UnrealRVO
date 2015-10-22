// Fill out your copyright notice in the Description page of Project Settings.

#include "RVOTest.h"
#include "BFLog.h"

#include "MathUtils.h"


BFLog::BFLog()
{

}

BFLog::BFLog(const char* fileName) : out{ fileName, std::ofstream::out }
{
	if (!out.is_open())
	{
		UE_LOG(LogRVOTest, Warning, TEXT("LOGERROR: file %s can't be opened!"), fileName);

	}
}

//template <typename T>
//BFLog& BFLog::operator << (T&& t)
//{
//	out << std::forward<T>(t);
//	return *this;
//}


BFLog& BFLog::operator << (const char* s)
{
	out << s;
	return *this;
}

BFLog& BFLog::operator << (int number)
{
	out << number;
	return *this;
}