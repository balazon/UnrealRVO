// Fill out your copyright notice in the Description page of Project Settings.


#pragma once


#include <cstdlib>

#include <random>

#include <iostream>


#include "ORCASolver.h"


#define KD_MAXNEARBY (5)


template <typename T>
void printList(const std::vector<T>& list)
{
	std::cout << "[";
	std::string prefix{ "" };
	for (auto& e : list)
	{
		std::cout << prefix << e;
		prefix = ", ";
	}
	std::cout << "]\n";
}

int randRange(int min, int max);


template <typename T, typename Comparator = std::less<T>>
int partition(std::vector<T>& list, int left, int right, int pivotIndex, Comparator comp = Comparator())
{

	auto pivotValue = list[pivotIndex];
	std::swap(list[pivotIndex], list[right]);
	int storeIndex = left;
	for (int i = left; i < right; i++)
	{
		if (comp(list[i], pivotValue))
		{
			std::swap(list[storeIndex], list[i]);
			storeIndex++;
		}

	}
	std::swap(list[right], list[storeIndex]);

	return storeIndex;
}

//n-th smallest element
template <typename T, typename Comparator = std::less<T>>
T& select(std::vector<T>& list, int left, int right, int n, Comparator comp = Comparator())
{
	if (left == right)
	{
		return list[left];
	}
	while (true)
	{
		int pivotIndex = randRange(left, right);
		pivotIndex = partition(list, left, right, pivotIndex, comp);
		if (n == pivotIndex)
		{
			return list[n];
		}
		else if (n < pivotIndex)
		{
			right = pivotIndex - 1;
		}
		else
		{
			left = pivotIndex + 1;
		}
	}
}



template <typename T, typename Comparator = std::less<T>>
T& median(std::vector<T>& container, int left, int right, Comparator comp = Comparator())
{
	return select(container, left, right, left + (right - left) / 2, comp);
}

template <typename T, typename Comparator = std::less<T>>
T& median(std::vector<T>& container, Comparator comp = Comparator())
{
	return select(container, 0, container.size() - 1, (container.size() - 1) / 2, comp);
}



struct Point
{
	float x, y;
	int pointId;
};



struct PointByX {
	bool operator()(const Point& a, const Point& b)
	{
		return a.x < b.x;
	}
};

struct PointByY {
	bool operator()(const Point& a, const Point& b)
	{
		return a.y < b.y;
	}
};

struct PointNode
{
	Point* p;

	PointNode* left;
	PointNode* right;

	PointNode() : p{ nullptr }, left{ nullptr }, right{ nullptr }
	{

	}
};




class KdTree
{
public:
	void build(std::vector<Point>& points);

	void updateNeighbours(ORCASolver* solver);

	//static constexpr int MAXNEARBY = 5;

	void setNeighbours(int owner, int neighbour);
private:
	PointNode* root;

	std::vector<PointNode> pointNodes;

	PointByX ByX;
	PointByY ByY;

	//axis % 2: 0 for x, 1 for y
	PointNode* buildRecursive(std::vector<Point>& list, int left, int right, int axis);

	void findNearby(Point* p, PointNode* node, int currentBest[], float currentBestDistancesSq[], int& nearbyCount, int axis);

	void insertPoint(Point* searchingPoint, int currentBest[], float currentBestDistancesSq[], int& nearbyCount, Point* nearbyPoint);
};



std::ostream& operator << (std::ostream& os, const Point& p);

std::ostream& operator << (std::ostream& os, const PointNode& pn);



