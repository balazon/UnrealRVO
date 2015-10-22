

#pragma once





#include <vector>
#include <unordered_set>

//CPLP for Closest Point Linear Programming
/**
*
*/
class RVOTEST_API CPLPSolver
{
public:
	CPLPSolver();
	~CPLPSolver();

	void Reset();

	//Ax + By < C
	//fixed for not being shuffled in Solve(), and stay in the beginning of the ordered constraints
	void AddConstraintLinear(float A, float B, float C, bool fixed = false);

	//fixed for not being shuffled in Solve(), and stay in the beginning of the ordered constraints
	void AddConstraintCircle(float U, float V, float R, bool fixed = false);

	//Destination coordinates
	void SetDestination(float u, float v);

	bool HasSolution();
	void Solve(float& resX, float& resY);

	void SolveSafest(int failIndex, float& resX, float& resY);

	bool debug;

	bool usedSafest;

	float usedDInSafest;

	void LogData();

	//for debug purposes
	void SetOrder(std::vector<int> order);


private:
	bool useCustomOrder;
	//destination coordinates
	float u, v;

	enum ConstraintType { CT_LINEAR, CT_CIRCLE };

	//3 elements define a constraint
	std::vector<float> constraints;

	std::vector<ConstraintType> constraintTypes;

	bool feasible;

	std::vector<int> order;

	std::unordered_set<int> filter;

	//nth constraint in array (not in the random order)
	bool pointSatisfiesConstraint(float tx, float ty, int n, float d = 0.f);

	//with regard to random order, from 0 up to and including n (first n+1)
	bool pointSatisfiesConstraints(float tx, float ty, int n, float d = 0.f, bool onlyCircles = false);

	void createRandomOrder();

	void normalizeLinearConstraints();

	//the max d value for checked linear constraints (up to and including index n)
	float getPointsMaxDistance(float x, float y, int n);

	
	//calculate new d
	void findNewRelaxationDistance(float& d, int failIndex, float& resX, float& resY);

	

	//first n+1 (from 0 up to and including n) constraints are regarded (random), (x,y),
	// point is checked if it's inside circles and 
	// if the max d distance from lines is smaller than current best solution's
	void updatePointIfBetter(float x, float y, int n, float& resX, float& resY, float& d);


	int fixedElementsNum;


	//TODO for min search pass as argument to 1d LP
	//std::vector<float> temp;

	//uint8 linearProgram1D(const std::vector<float>&  )

};


