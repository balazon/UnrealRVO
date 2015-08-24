// Fill out your copyright notice in the Description page of Project Settings.

#include "RVOTest.h"
#include "MathUtils.h"



#include <iostream>


#include <cmath>
#include <utility>
#define EPS (0.001)


bool IntersectLines(float A, float B, float C, float D, float E, float F, float& resX, float& resY)
{
	UE_LOG(LogRVOTest, Warning, TEXT("il params: %f %f %f %f %f %f"), A, B, C, D, E, F);

	float denominator = D * B - E * A;
	if (fabs(denominator) < EPS) return false;

	resX = (F * B - E * C) / denominator;
	resY = (C * D - A * F) / denominator;

	if (isnan(resX) || isnan(resY))
	{
		UE_LOG(LogRVOTest, Warning, TEXT("params: %f %f %f %f %f %f"), A, B, C, D, E, F);
	}
	return true;
}



void OrthogonalProjectionOfPointOnLine(float A, float B, float C, float tx, float ty, float& resX, float& resY)
{
	UE_LOG(LogRVOTest, Warning, TEXT("oppl params: %f %f %f %f %f"), A, B, C, tx, ty);

	float denominator = A * A + B * B;
	resX = (B * B * tx - A * B * ty + A * C) / denominator;
	resY = (A * A * ty - A * B * tx + B * C) / denominator;
	if (isnan(resX) || isnan(resY))
	{
		UE_LOG(LogRVOTest, Warning, TEXT("params: %f %f %f %f %f"), A, B, C, tx, ty);
	}
}

bool QuadraticEquation(float a, float b, float c, float& x1, float& x2)
{
	UE_LOG(LogRVOTest, Warning, TEXT("qe params: %f %f %f"), a, b, c);

	std::cout << "quad: " << a << " , " << b << " , " << c << "\n";
	float D;
	if (fabs(a) < EPS)
	{
		if (fabs(b) < EPS)
		{
			return false;
		}
		x1 = x2 = -c / b;
		return true;
	}
	if ((D = b * b - 4 * a * c) < 0)
	{
		return false;
	}
	float denom_reciproc = .5f / a;
	float sqrtD = sqrtf(D);
	x1 = (-b + sqrtD) * denom_reciproc;
	x2 = (-b - sqrtD) * denom_reciproc;

	if (isnan(x1) || isnan(x2))
	{
		UE_LOG(LogRVOTest, Warning, TEXT("params: %f %f %f"), a, b, c);
	}

	return true;
}

bool IntersectLineCircle(float A, float B, float C, float u, float v, float r, float& x1, float& y1, float& x2, float& y2)
{
	UE_LOG(LogRVOTest, Warning, TEXT("ilc params: %f %f %f %f %f %f"), A, B, C, u, v, r);

	bool swapped = false;
	if (fabs(A) < EPS && fabs(B) < EPS)
	{
		UE_LOG(LogRVOTest, Warning, TEXT("params: %f %f %f %f %f %f"), A, B, C, u, v, r);
		return false;
	}
	if (fabs(B) < EPS)
	{	
		return IntersectLineCircle(B, A, C, v, u, r, y1, x1, y2, x2);
	}
	float a = A * A + B * B;
	float b = -2.f * C * A - 2.f * u * B * B + 2.f * v * A * B;
	float c = C * C - 2.f * v * C * B + (u * u + v * v - r * r) * B * B;
	bool solvable = QuadraticEquation(a, b, c, x1, x2);
	if (!solvable)
	{
		return false;
	}
	y1 = (C - A * x1) / B;
	y2 = (C - A * x2) / B;

	if (isnan(x1) || isnan(x2) || isnan(y1) || isnan(y2))
	{
		UE_LOG(LogRVOTest, Warning, TEXT("params: %f %f %f %f %f %f"), A, B, C, u, v, r);
	}
	
	return true;
}

bool IntersectCircleCircle(float u1, float v1, float r1, float u2, float v2, float r2, float& x1, float& y1, float& x2, float& y2)
{
	UE_LOG(LogRVOTest, Warning, TEXT("icc params: %f %f %f %f %f %f"), u1, v1, r1, u2, v2, r2)

	float A = u1 - u2;
	float B = v1 - v2;
	if (fabs(A) < EPS && fabs(B) < EPS)
	{
		return false;
	}
	return IntersectLineCircle(A, B, .5f * ((r2 + r1) * (r2 - r1) + (u1 + u2) * (u1 - u2) + (v1 + v2) * (v1 - v2)), u1, v1, r1, x1, y1, x2, y2);
}

bool OrthogonalProjectionOfPointOnCircle(float u, float v, float r, float tx, float ty, float& resX, float& resY)
{
	UE_LOG(LogRVOTest, Warning, TEXT("params: %f %f %f %f %f"), u, v, r, tx, ty);

	float OTx = tx - u;
	float OTy = ty - v;
	if (fabs(OTx) < EPS && fabs(OTy) < EPS)
	{
		return false;
	}
	float k = r / sqrtf(OTx * OTx + OTy * OTy);
	resX = u + OTx * k;
	resY = v + OTy * k;

	if (isnan(resX) || isnan(resY))
	{
		UE_LOG(LogRVOTest, Warning, TEXT("params: %f %f %f %f %f"), u, v, r, tx, ty);
	}

	return true;
}

// (A,B) and (D,E) must be normalized
void AngleBisector(float A, float B, float C, float D, float E, float F, float& G, float& H, float& I)
{
	G = A - D;
	H = B - E;
	I = C - F;

	if (fabs(G) < EPS && fabs(H) < EPS)
	{
		UE_LOG(LogRVOTest, Warning, TEXT("anglebisectorBAM"));

	}
}