#include <iostream>

#include "Smoothing.h"




Smoothing::Smoothing(std::string mode, int size) {

	setMode(mode);

	list_size = size;
	for (int i = 0; i < list_size; i++)
		weights.push_back(i+1);

	//oriCoords = std::vector<int *>();
	aveCoords = std::vector<double *>();
	weiCoords = std::vector<double *>();
}

Smoothing::~Smoothing() {

}

int Smoothing::addNewElement(int elem_x, int elem_y) {
	if (list_x.size() == list_size) { 
		list_x.pop_front();
		list_y.pop_front();
	}
	list_x.push_back(elem_x);
	list_y.push_back(elem_y);
	return 1;
}


double * Smoothing::getNewCoordinates(int  elem_x, int elem_y) {
	addNewElement(elem_x, elem_y);

	double * cave = getAverageCoordinates();
	double * cwei = getWeightedCoordinates();
	oriCoords.push_back(elem_x);
	aveCoords.push_back(cave);
	weiCoords.push_back(cwei);

	switch (smoothing_mode) {
	case AVERAGED:
		return getAverageCoordinates();
	case WEIGHTED:
		return getWeightedCoordinates();
	case BSPLINES:
		return getBsplineCoordinates();
	}
	return 0;
}



double * Smoothing::getAverageCoordinates() {
	double * average = new double[2];

	average[0] = 0;
	average[1] = 0;

	for (std::list<int>::iterator elem = list_x.begin(); elem != list_x.end(); elem++) {
		average[0] += *elem;
	}
	for (std::list<int>::iterator elem = list_y.begin(); elem != list_y.end(); elem++) {
		average[1] += *elem;
	}

	average[0] = average[0] / list_size;
	average[1] = average[1] / list_size;

	return average;
}


double * Smoothing::getWeightedCoordinates() {
	double * weighted_average = new double[2];
	int S = (list_size * (weights.front() + weights.back())) / 2;
	weighted_average[0] = 0;
	weighted_average[1] = 0;

	double tmp = 0;
	for (std::list<int>::iterator elem = list_x.begin(), w = weights.begin(); elem != list_x.end(); elem++, w++) {
		tmp += *elem * ((float) (*w) / (float) S);
	}
	weighted_average[0] = tmp;

	tmp = 0;
	for (std::list<int>::iterator elem = list_y.begin(), w = weights.begin(); elem != list_y.end(); elem++, w++) {
		tmp += *elem * ((float)(*w) / (float) S);
	}
	weighted_average[1] = tmp;
	//weighted_average[0] = weighted_average[0] / list_size;
	//weighted_average[1] = weighted_average[1] / list_size;

	return weighted_average;
}


double * Smoothing::getBsplineCoordinates() {
	return 0;
}


int Smoothing::setMode(std::string mode) {

	if (mode.compare("averaged") == 0)
		smoothing_mode = AVERAGED;
	if (mode.compare("weighted") == 0)
		smoothing_mode = WEIGHTED;
	if (mode.compare("bsplines") == 0)
		smoothing_mode = BSPLINES;

	std::cout << "Mode changed to " << mode << std::endl;
	return 0;
}