// Fill out your copyright notice in the Description page of Project Settings.


#include "RVOTest.h"


#include "ORCASolver.h"

#include "MathUtils.h"
#include <cmath>
#include <utility>


#include "CPLPSolver.h"

#include "SVGExporter.h"

#include <fstream>


//#define EPS (0.001)

CPLPSolver solver;

Agent::Agent() : Agent{ 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f }
{
}

Agent::Agent(float x, float y, float vx, float vy, float r, float vx_pref, float vy_pref, float maxVelocityMagnitude, float maxAccMagnitude)
	: x{ x }, y{ y }, vx{ vx }, vy{ vy }, r{ r }, vx_pref{ vx_pref }, vy_pref{ vy_pref }, maxVelocityMagnitude{ maxVelocityMagnitude }, maxAccMagnitude{ maxAccMagnitude }, nearbyCount{ 0 }
{

}

Agent::~Agent()
{
}

void ORCASolver::ClearNeighbours(int i)
{
	Agent& a = agents[i];
	a.nearbyCount = 0;
}
void ORCASolver::SetAgentsNearby(int i, int j)
{

	Agent& a = agents[i];
	if (a.nearbyCount >= CA_MAXNEARBY)
	{
		return;
	}
	a.nearbyAgents[a.nearbyCount] = j;
	a.nearbyCount++;
}

bool ORCASolver::AreAgentsNeighbours(int i, int j)
{
	UE_LOG(LogRVOTest, VeryVerbose, TEXT("agentsneighbours %d %d"), i, j);

	Agent& a = agents[i];
	Agent& b = agents[j];
	int aNearby = a.nearbyCount;
	int bNearby = b.nearbyCount;
	int maxN = aNearby < bNearby ? bNearby : aNearby;
	for (int k = 0; k < maxN; k++)
	{
		if (k < aNearby && a.nearbyAgents[k] == j || k < bNearby && b.nearbyAgents[k] == i)
		{
			UE_LOG(LogRVOTest, VeryVerbose, TEXT("agentsneighbours true"));
			return true;
		}
	}
	UE_LOG(LogRVOTest, VeryVerbose, TEXT("agentsneighbours false"));
	return false;
}



void ORCASolver::SetORCAConstraint(Agent& a, int j, float A, float B, float C)
{
	if (fabs(A) < EPS && fabs(B) < EPS || BMU::isnanf(A) || BMU::isnanf(B) || BMU::isnanf(C))
	{
		UE_LOG(LogRVOTest, Warning, TEXT("setORCAConstraint BBBAMMM %f %f %f"), A, B, C);
	}
	for (int i = 0; i < a.nearbyCount; i++)
	{
		if (a.nearbyAgents[i] == j)
		{
			a.ORCAA[i] = A;
			a.ORCAB[i] = B;
			a.ORCAC[i] = C;
			break;
		}
	}
}

ORCASolver::ORCASolver() : T{ CA_TAU }, num{ 0 }
{
}

ORCASolver::~ORCASolver()
{
}


Agent& ORCASolver::GetAgent(int id)
{
	return agents[id];
}

//returns id of agent
int ORCASolver::AddAgent()
{
	if (num == CA_MAXAGENTS)
		return -1;

	agents[num].nearbyCount = 0;
	return num++;
}

int ORCASolver::RemoveAgent(int id)
{
	if (id >= num || id < 0)
	{
		return -1;
	}

	num--;
	if (id == num)
	{
		return -1;
	}
	return num;
}

void ORCASolver::ClearAgents()
{
	num = 0;
}

void ORCASolver::SetParameters(float T)
{
	this->T = T;
}



void ORCASolver::computeORCAConstraints(int i, int j)
{
	Agent& a = agents[i];
	Agent& b = agents[j];
	float ABx = b.x - a.x;
	float ABy = b.y - a.y;
	float R = a.r + b.r;
	float r = R / T;
	float Ox = ABx / T;
	float Oy = ABy / T;
	float Px, Py;
	float Qx, Qy;

	//v^{opt}_A - v^{opt}_B which is vrel when v^{opt}_X = v_X for all X
	float vrelx = a.vx - b.vx;
	float vrely = a.vy - b.vy;


	float distSq = ABx * ABx + ABy* ABy;
	if (distSq > (a.maxVelocityMagnitude + b.maxVelocityMagnitude) * (a.maxVelocityMagnitude + b.maxVelocityMagnitude) * T * T)
	{
		SetORCAConstraint(a, j, 0.f, 0.f, 0.f);

		SetORCAConstraint(b, i, 0.f, 0.f, 0.f);

		return;
	}


	//v_opt is outside the VO
	bool outside = false;



	//bool pqcircle = BMU::IntersectCircleCircle(ABx, ABy, R, ABx * .5f, ABy * .5f, .5f * sqrtf(ABx * ABx + ABy * ABy), Px, Py, Qx, Qy);

	bool pqcircle = BMU::IntersectCircleCircle(ABx, ABy, R, ABx * .5f, ABy * .5f, .5f * sqrtf(ABx * ABx + ABy * ABy), Px, Py, Qx, Qy);
	while (!pqcircle || fabs(Px) < EPS && fabs(Py) < EPS || fabs(Qx) < EPS && fabs(Qy) < EPS)
	{
		R *= .9f;
		r = R / T;
		pqcircle = BMU::IntersectCircleCircle(ABx, ABy, R, ABx * .5f, ABy * .5f, .5f * sqrtf(ABx * ABx + ABy * ABy), Px, Py, Qx, Qy);
	}

	if (BMU::isnanf(Qx) || BMU::isnanf(Qy) || BMU::isnanf(Px) || BMU::isnanf(Py))
	{
		UE_LOG(LogRVOTest, Warning, TEXT("P: %.2f %.2f, Q: %.2f %.2f"), Px, Py, Qx, Qy);
		UE_LOG(LogRVOTest, Warning, TEXT("AB: %.2f %.2f, R: %.2f %d"), ABx, ABy, R, pqcircle);
		BMU::debug = true;
		pqcircle = BMU::IntersectCircleCircle(ABx, ABy, R, ABx * .5f, ABy * .5f, .5f * sqrtf(ABx * ABx + ABy * ABy), Px, Py, Qx, Qy);
		BMU::debug = false;
	}

	if (fabs(Px) < EPS && fabs(Py) < EPS || fabs(Qx) < EPS && fabs(Qy) < EPS)
	{
		UE_LOG(LogRVOTest, Warning, TEXT("P: %.2f %.2f, Q: %.2f %.2f"), Px, Py, Qx, Qy);
		UE_LOG(LogRVOTest, Warning, TEXT("AB: %.2f %.2f, R: %.2f %d"), ABx, ABy, R, pqcircle);
		BMU::debug = true;
		pqcircle = BMU::IntersectCircleCircle(ABx, ABy, R, ABx * .5f, ABy * .5f, .5f * sqrtf(ABx * ABx + ABy * ABy), Px, Py, Qx, Qy);
		BMU::debug = false;

	}

	UE_LOG(LogRVOTest, VeryVerbose, TEXT("computeSmallest, P( %f , %f )  Q( %f,  %f )"), Px, Py, Qx, Qy);



	if (-ABy * Px + ABx * Py > 0.f)
	{
		std::swap(Px, Qx);
		std::swap(Py, Qy);
	}

	float Npx = Py;
	float Npy = -Px;

	float Nqx = -Qy;
	float Nqy = Qx;

	float Nplrec = 1.f / sqrtf(Npx * Npx + Npy * Npy);
	float Nqlrec = 1.f / sqrtf(Nqx * Nqx + Nqy * Nqy);

	Npx *= Nplrec;
	Npy *= Nplrec;
	Nqx *= Nqlrec;
	Nqy *= Nqlrec;

	if (BMU::isnanf(Npx) || BMU::isnanf(Npy) || BMU::isnanf(Nqx) || BMU::isnanf(Nqy) || fabs(Npx) < EPS && fabs(Npy) < EPS || fabs(Nqx) < EPS && fabs(Nqy) < EPS)
	{
		UE_LOG(LogRVOTest, Warning, TEXT("BAM"));

	}


	//G, H points are not needed, but they are O's orthogonal projections to AP, and AQ (or the little circle's touching point)
	//they are the 'g' and 'h' in variable names below: Aog, Aoh, ..

	


	//outside QAP angle area

	if (Npx * vrelx + Npy * vrely > 0 || Nqx * vrelx + Nqy * vrely > 0)
	{
		/*SetUVector(i, j, 0.f, 0.f);
		return;*/
		/*if (Npx * vrelx + Npy * vrely > a.maxAccMagnitude + b.maxAccMagnitude || Nqx * vrelx + Nqy * vrely > a.maxAccMagnitude + b.maxAccMagnitude )
		{
		SetUVector(i, j, 0.f, 0.f);
		return;
		}
		reversed = true;*/
		outside = true;

	}


	//Inside OG, or OH constraint is the area that contains B
	float Aog = -Px;
	float Bog = -Py;
	float Cog = Aog * Ox + Bog * Oy;

	float Aoh = -Qx;
	float Boh = -Qy;
	float Coh = Aoh * Ox + Boh * Oy;


	float Sx, Sy, Nx, Ny;
	if (Aog * vrelx + Bog * vrely < Cog || Aoh * vrelx + Boh * vrely < Coh)
	{
		if (-ABy * vrelx + ABx * vrely > 0.f)
		{
			BMU::OrthogonalProjectionOfPointOnLine(Nqx, Nqy, 0.f, vrelx, vrely, Sx, Sy);
			Nx = Nqx;
			Ny = Nqy;
		}
		else
		{
			BMU::OrthogonalProjectionOfPointOnLine(Npx, Npy, 0.f, vrelx, vrely, Sx, Sy);
			Nx = Npx;
			Ny = Npy;
		}
	}
	else
	{
		bool orth = BMU::OrthogonalProjectionOfPointOnCircle(Ox, Oy, r, vrelx, vrely, Sx, Sy);
		if (!orth)
		{
			orth = BMU::OrthogonalProjectionOfPointOnCircle(Ox, Oy, r, Ox + 2.f * Npx * EPS, Oy + 2.f * Npy * EPS, Sx, Sy);
			if (!orth)
			{
				UE_LOG(LogRVOTest, Warning, TEXT("POINTCIRCLE"));
				orth = BMU::OrthogonalProjectionOfPointOnCircle(Ox, Oy, r, Ox + 2.f * Npx * EPS, Oy + 2.f * Npy * EPS, Sx, Sy);
			}
		}
		Nx = Sx - Ox;
		Ny = Sy - Oy;
		outside = outside || ((vrelx - Ox) * (vrelx - Ox) + (vrely - Oy) * (vrely - Oy) > r * r);
	}
	float Nlrec = 1.f / sqrtf(Nx * Nx + Ny * Ny);
	Nx *= Nlrec;
	Ny *= Nlrec;

	float ux = Sx - vrelx;
	float uy = Sy - vrely;

	if (!outside)
	{
		ux *= .5f;
		uy *= .5f;
	}

	float A1, B1, C1, A2, B2, C2;
	A1 = -Nx; B1 = -Ny; C1 = -Nx * (a.vx + ux) - Ny * (a.vy + uy);
	A2 = Nx; B2 = Ny; C2 = Nx * (b.vx - ux) + Ny * (b.vy - uy);

	float oLrec1 = 1.f / sqrtf(A1 * A1 + B1 * B1);
	float oLrec2 = 1.f / sqrtf(A2 * A2 + B2 * B2);

	A1 *= oLrec1;
	B1 *= oLrec1;
	C1 *= oLrec1;

	A2 *= oLrec2;
	B2 *= oLrec2;
	C2 *= oLrec2;
	if (fabs(A1) < EPS && fabs(B1) < EPS || fabs(A2) < EPS && fabs(B2) < EPS)
	{
		UE_LOG(LogRVOTest, Warning, TEXT("ORCALINN: %f %f %f %f %f %f"), A1, B1, C1, A2, B2, C2);
	}
	

	UE_LOG(LogRVOTest, VeryVerbose, TEXT("ij: %d %d, \n  %.2f, %.2f, %.2f \n  %.2f, %.2f, %.2f\n"), i, j, A1, B1, C1, A2, B2, C2);
	/*UE_LOG(LogRVOTest, Warning, TEXT("u: %.2f %.2f\n"), ux, uy);
	UE_LOG(LogRVOTest, Warning, TEXT("a.v: %.2f %.2f\n"), a.vx, a.vy);
	UE_LOG(LogRVOTest, Warning, TEXT("b.v: %.2f %.2f\n"), b.vx, b.vy);
	UE_LOG(LogRVOTest, Warning, TEXT("a.v + u %.2f %.2f\n"), a.vx + ux, a.vy + uy);
	UE_LOG(LogRVOTest, Warning, TEXT("b.v - u %.2f %.2f\n\n"), b.vx - ux, b.vy - uy);
	*/

	


	if (BMU::isnanf(Nx) || BMU::isnanf(Ny) || fabs(Nx) < EPS && fabs(Ny) < EPS)
	{
		UE_LOG(LogRVOTest, Warning, TEXT("ORCA: n < EPS"));
	}

	if (BMU::isnanf(ux) || BMU::isnanf(uy))
	{
		UE_LOG(LogRVOTest, Warning, TEXT("BAM"));
	}

	
	if (BMU::isnanf(A1) || BMU::isnanf(B1) || BMU::isnanf(C1) || BMU::isnanf(A2) || BMU::isnanf(B2) || BMU::isnanf(C2))
	{
		UE_LOG(LogRVOTest, Warning, TEXT("ORCALINN: %f %f %f %f %f %f"), A1, B1, C1, A2, B2, C2);
	}

	SetORCAConstraint(a, j, A1, B1, C1);

	SetORCAConstraint(b, i, A2, B2, C2);
}

void ORCASolver::ComputeNewVelocities()
{
	//BMU::debug = true;
	for (int i = 0; i < num; i++)
	{
		Agent& a = agents[i];

		for (int k = 0; k < a.nearbyCount; k++)
		{
			int j = a.nearbyAgents[k];

			//if (i < j)
			if (true)
			{
				
				computeORCAConstraints(i, j);
			}
		}

		/*if (a.vx_pref * a.vx_pref + a.vy_pref * a.vy_pref < EPS)
		{
		a.vx_new = 0.f;
		a.vy_new = 0.f;
		continue;
		}*/

		solver.Reset();
		UE_LOG(LogRVOTest, VeryVerbose, TEXT("vxpref %f %f"), a.vx_pref, a.vy_pref);
		solver.SetDestination(a.vx_pref, a.vy_pref);
		for (int j = 0; j < a.nearbyCount; j++)
		{
			float A = a.ORCAA[j];
			float B = a.ORCAB[j];
			float C = a.ORCAC[j];
			
			//a.nearbyAgents[j] == i shouldn't happen but just to be safe
			if (fabs(A) < EPS && fabs(B) < EPS || a.nearbyAgents[j] == i )
			{
				continue;
			}

			if (fabs(A) < EPS && fabs(B) < EPS || BMU::isnanf(A) || BMU::isnanf(B))
			{
				UE_LOG(LogRVOTest, Warning, TEXT("A, B, C: %f %f %f"), A, B, C);

			}
			UE_LOG(LogRVOTest, VeryVerbose, TEXT("vxpref %f %f"), a.vx_pref, a.vy_pref);

			solver.AddConstraintLinear(A, B, C);

			float lrec = 1.f / sqrtf(A * A + B * B);
			A *= lrec;
			B *= lrec;
			C *= lrec;

			if (fabs(A) < EPS && fabs(B) < EPS)
			{
				UE_LOG(LogRVOTest, Warning, TEXT("A, B, C: %f %f %f"), A, B, C);
				BMU::debug = true;
				computeORCAConstraints(i, a.nearbyAgents[j]);
				BMU::debug = false;

				A = a.ORCAA[j];
				B = a.ORCAB[j];
				C = a.ORCAC[j];
				lrec = 1.f / sqrtf(A * A + B * B);
				A *= lrec;
				B *= lrec;
				C *= lrec;
				if (fabs(A) < EPS && fabs(B) < EPS)
				{
					UE_LOG(LogRVOTest, Warning, TEXT("A, B, C: %f %f %f"), A, B, C);

				}
			}

		}


		solver.AddConstraintCircle(a.vx, a.vy, a.maxAccMagnitude, true);
		solver.AddConstraintCircle(0.f, 0.f, a.maxVelocityMagnitude, true);



		solver.Solve(a.vx_new, a.vy_new);

		if (BMU::isnanf(0.f))
		{
			UE_LOG(LogRVOTest, Warning, TEXT("HMM"));

		}


		if (a.vx_new * a.vx_new + a.vy_new * a.vy_new > a.maxVelocityMagnitude * (a.maxVelocityMagnitude + 2 * EPS))
		{
			/*float vxnew, vynew;
			BMU::OrthogonalProjectionOfPointOnCircle(0.f, 0.f, a.maxVelocityMagnitude, a.vx_new, a.vy_new, vxnew, vynew);
			
			a.vx_new = vxnew;
			a.vy_new = vynew;

			if (BMU::isnanf(a.vx_new) || BMU::isnanf(a.vy_new))
			{
				UE_LOG(LogRVOTest, Warning, TEXT("BAM"));
			}

			if (BMU::isnanf(a.vx_new) || BMU::isnanf(a.vy_new) ||  (a.vx_new * a.vx_new + a.vy_new * a.vy_new > a.maxVelocityMagnitude * a.maxVelocityMagnitude))
			{
				float sqrDist1 = a.vx_new * a.vx_new + a.vy_new * a.vy_new;
				float sqrDist2 = a.maxVelocityMagnitude * a.maxVelocityMagnitude;
				UE_LOG(LogRVOTest, Warning, TEXT("sqr: %f %f maxvel"), sqrDist1, sqrDist2);
				BMU::debug = true;
				BMU::OrthogonalProjectionOfPointOnCircle(0.f, 0.f, a.maxVelocityMagnitude, a.vx_new, a.vy_new, vxnew, vynew);
				BMU::debug = false;
			}*/
			a.vx_new *= 0.99f;
			a.vy_new *= 0.99f;

			if (a.vx_new * a.vx_new + a.vy_new * a.vy_new > a.maxVelocityMagnitude * (a.maxVelocityMagnitude + 2 * EPS))
			{

				SVGExporter::writeUnitORCAs("D:/Bala/Unreal Projects/RVOTest/test.svg", this, num, i);
				UE_LOG(LogRVOTest, Warning, TEXT("a.v: %f %f, a.vnew : %f %f, a.vmax: %f"), a.vx, a.vy, a.vx_new, a.vy_new, a.maxVelocityMagnitude);
				BMU::debug = true;
				solver.debug = true;
				solver.Solve(a.vx_new, a.vy_new);
				BMU::debug = false;
				solver.debug = false;
			}

		}

		float axnew = a.vx_new - a.vx;
		float aynew = a.vy_new - a.vy;
		if (axnew * axnew + aynew * aynew > a.maxAccMagnitude)
		{
			/*float myaxnew, myaynew;
			BMU::OrthogonalProjectionOfPointOnCircle(0.f, 0.f, a.maxAccMagnitude, a.vx_new - a.vx, a.vy_new - a.vy, myaxnew, myaynew);
			
			a.vx_new = a.vx + myaxnew;
			a.vy_new = a.vy + myaynew;

			if (BMU::isnanf(a.vx_new) || BMU::isnanf(a.vy_new))
			{
				UE_LOG(LogRVOTest, Warning, TEXT("BAM"));
			}

			if (BMU::isnanf(a.vx_new) || BMU::isnanf(a.vy_new) || ((a.vx_new - a.vx) * (a.vx_new - a.vx) + (a.vy_new - a.vy) * (a.vy_new - a.vy) > a.maxAccMagnitude * a.maxAccMagnitude))
			{
				BMU::debug = true;
				float sqrDist1 = (a.vx_new - a.vx) * (a.vx_new - a.vx) + (a.vy_new - a.vy) * (a.vy_new - a.vy);
				float sqrDist2 = a.maxAccMagnitude * a.maxAccMagnitude;
				UE_LOG(LogRVOTest, Warning, TEXT("sqr: %f %f maxacc"), sqrDist1, sqrDist2);
				BMU::OrthogonalProjectionOfPointOnCircle(0.f, 0.f, a.maxAccMagnitude, a.vx_new - a.vx, a.vy_new - a.vy, myaxnew, myaynew);
				BMU::debug = false;
			}
*/

			axnew *= 0.99f;
			aynew *= 0.99f;
			a.vx_new = a.vx + axnew;
			a.vy_new = a.vy + aynew;

			if ((a.vx_new - a.vx) * (a.vx_new - a.vx) + (a.vy_new - a.vy) * (a.vy_new - a.vy) > a.maxAccMagnitude * (a.maxAccMagnitude + 2 * EPS))
			{
				//SVGExporter::writeUnitORCAs("D:/Bala/Unreal Projects/RVOTest/test.svg", this, num, i);
				UE_LOG(LogRVOTest, Warning, TEXT("a.v %f %f, a.vnew %f %f, a.maxacc : %f"), a.vx, a.vy, a.vx_new, a.vy_new, a.maxAccMagnitude);
				BMU::debug = true;
				solver.debug = true;
				solver.Solve(a.vx_new, a.vy_new);
				BMU::debug = false;
				solver.debug = false;
			}

		}

		if (BMU::isnanf(a.vx_new) || BMU::isnanf(a.vy_new))
		{
			//FString dir{ "D:/Bala/Unreal Projects/RVOTest/test.svg" };
			
			//SVGExporter::writeUnitORCAs("D:/Bala/Unreal Projects/RVOTest/test.svg", this, num, i);
			UE_LOG(LogRVOTest, Warning, TEXT("BAM\n"));
			UE_LOG(LogRVOTest, Warning, TEXT("a.v: %f %f, a.vnew : %f %f, a.vmax: %f"), a.vx, a.vy, a.vx_new, a.vy_new, a.maxVelocityMagnitude);
			UE_LOG(LogRVOTest, Warning, TEXT("a.v %f %f, a.vnew %f %f, a.maxacc : %f"), a.vx, a.vy, a.vx_new, a.vy_new, a.maxAccMagnitude);

			BMU::debug = true;
			solver.debug = true;
			solver.Solve(a.vx_new, a.vy_new);
			BMU::debug = false;
			solver.debug = false;
			if (BMU::isnanf(a.vx_new) || BMU::isnanf(a.vy_new))
			{
				UE_LOG(LogRVOTest, VeryVerbose, TEXT("BAM confirm\n"));
			}
		}

		if (BMU::isnanf(a.vx_new) || BMU::isnanf(a.vy_new))
		{
			a.vx_new = a.vx;
			a.vy_new = a.vy;
		}
	}
}

void ORCASolver::SetDebugging(bool on)
{
	solver.debug = on;
	BMU::debug = on;
}