#pragma once
#include <stdio.h>
#include <string>
#include<list>


#define AVERAGED 1
#define WEIGHTED 2
#define BSPLINES 3

class Smoothing {

private:
	int smoothing_mode;
	int list_size;

	std::list<int> list_x;
	std::list<int> list_y;
	std::list<int> weights;

public:
	Smoothing(std::string mode, int size);
	~Smoothing();

	int addNewElement(int elem_x, int elem_y);

	int * getNewCoordinates(int elem_x, int elem_y);

	int * getAverageCoordinates();

	int * getWeightedCoordinates();

	int * getBsplineCoordinates();
};