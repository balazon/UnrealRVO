// Fill out your copyright notice in the Description page of Project Settings.

#include "RVOTest.h"
#include "KdTree.h"



std::ostream& operator << (std::ostream& os, const Point& p)
{
	os << "(" << p.x << "," << p.y << "," << p.pointId << ")";
	return os;
}

std::ostream& operator << (std::ostream& os, const PointNode& pn)
{

	os << "{ ";
	os << *pn.p;

	if (pn.left)
	{
		os << ", " << pn.left->p->pointId;
	}
	if (pn.right)
	{
		os << ", " << pn.right->p->pointId;
	}
	os << " }\n";

	return os;
}

//std::default_random_engine generator;
int randRange(int min, int max)
{
	int range = max - min + 1;
	return rand() % range + min;

	//std::uniform_int_distribution<int> distribution(min, max);
	//return distribution(generator);
}


void KdTree::build(std::vector<Point>& points)
{


	pointNodes.clear();
	pointNodes.reserve(points.size());

	if (points.size() == 0)
	{
		return;
	}

	
	root = buildRecursive(points, 0, points.size() - 1, 0);

	//printList(pointNodes);
}

PointNode* KdTree::buildRecursive(std::vector<Point>& list, int left, int right, int axis)
{

	pointNodes.push_back(PointNode{});
	PointNode& pn = pointNodes.back();
	if (left == right)
	{
		pn.p = &list[left];
		pn.left = nullptr;
		pn.right = nullptr;
	}
	else
	{
		if (axis == 0)
		{
			pn.p = &median(list, left, right, ByX);
		}
		else
		{
			pn.p = &median(list, left, right, ByY);
		}

		//printList(list);
		//2 elements left : first is median, second is the right leaf
		if (right - left == 1)
		{
			pn.left = nullptr;
		}
		else
		{
			pn.left = buildRecursive(list, left, left + (right - left) / 2 - 1, (axis + 1) % 2);
		}

		pn.right = buildRecursive(list, left + (right - left) / 2 + 1, right, (axis + 1) % 2);
	}


	return &pn;
}

void KdTree::updateNeighbours(ORCASolver* solver)
{
	for (int i = 0; i < pointNodes.size(); i++)
	{
		//int currentBest[MAXNEARBY];
		float currentBestDistancesSq[KD_MAXNEARBY];
		
		PointNode* pn = &pointNodes[i];
		
		
		Agent& a = solver->GetAgent(pn->p->pointId);
		findNearby(pn->p, root, a.nearbyAgents, currentBestDistancesSq, a.nearbyCount, 0);
	}
}

void KdTree::findNearby(Point* p, PointNode* node, int currentBest[], float currentBestDistancesSq[], int& nearbyCount, int axis)
{

	insertPoint(p, currentBest, currentBestDistancesSq, nearbyCount, node->p);

	float splitCoordDiff = axis == 0 ? p->x - node->p->x : p->y - node->p->y;
	PointNode* primary = splitCoordDiff >= 0 ? node->right : node->left;
	PointNode* secondary = splitCoordDiff >= 0 ? node->left : node->right;
	if (primary)
	{
		findNearby(p, primary, currentBest, currentBestDistancesSq, nearbyCount, (axis + 1) % 2);
	}


	if (nearbyCount == 0 || splitCoordDiff * splitCoordDiff < currentBestDistancesSq[nearbyCount - 1])
	{
		if (secondary)
		{
			findNearby(p, secondary, currentBest, currentBestDistancesSq, nearbyCount, (axis + 1) % 2);
		}
	}

}


void KdTree::insertPoint(Point* searchingPoint, int currentBest[], float currentBestDistancesSq[], int& nearbyCount, Point* nearbyPoint)
{
	if (nearbyPoint->pointId == searchingPoint->pointId)
	{
		return;
	}

	float sqDist = (nearbyPoint->x - searchingPoint->x) * (nearbyPoint->x - searchingPoint->x) + (nearbyPoint->y - searchingPoint->y) * (nearbyPoint->y - searchingPoint->y);
	if (nearbyCount == KD_MAXNEARBY && currentBestDistancesSq[KD_MAXNEARBY - 1] <= sqDist)
	{
		return;
	}
	int i;
	for (i = 0; i < nearbyCount; i++)
	{
		if (sqDist < currentBestDistancesSq[i])
		{
			break;
		}
	}

	int temp = nearbyPoint->pointId;
	float tempSq = sqDist;


	int upper = nearbyCount + 1 > KD_MAXNEARBY ? KD_MAXNEARBY : nearbyCount + 1;

	for (int j = i; j < upper; j++)
	{
		std::swap(currentBest[j], temp);
		std::swap(currentBestDistancesSq[j], tempSq);
	}
	if (nearbyCount < KD_MAXNEARBY)
	{
		nearbyCount++;
	}

}


void KdTree::setNeighbours(int owner, int neighbour)
{
	std::cout << neighbour << " ";

}