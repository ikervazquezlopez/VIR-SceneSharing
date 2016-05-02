#pragma once
#include <stdio.h>
#include <string>
#include<list>
#include <vector>


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

	std::vector<int > oriCoords;
	std::vector<double *> aveCoords;
	std::vector<double *> weiCoords;

	Smoothing(std::string mode, int size);
	~Smoothing();

	int addNewElement(int elem_x, int elem_y);

	double * getNewCoordinates(int elem_x, int elem_y);

	double * getAverageCoordinates();

	double * getWeightedCoordinates();

	double * getBsplineCoordinates();

	int setMode(std::string mode);
};