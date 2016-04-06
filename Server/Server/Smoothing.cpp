#include "Smoothing.h"




Smoothing::Smoothing(std::string mode, int size) {

	if (mode.compare("averaged") == 0)
		smoothing_mode = AVERAGED;
	if (mode.compare("weighted") == 0)
		smoothing_mode = WEIGHTED;
	if (mode.compare("bsplines") == 0)
		smoothing_mode = BSPLINES;

	list_size = size;
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


int * Smoothing::getNewCoordinates(int  elem_x, int elem_y) {
	addNewElement(elem_x, elem_y);

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



int * Smoothing::getAverageCoordinates() {
	int * average = new int[2];

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


int * Smoothing::getWeightedCoordinates() {
	int * weighted_average = new int[2];

	weighted_average[0] = 0;
	weighted_average[1] = 0;

	for (std::list<int>::iterator elem = list_x.begin(), w = weights.begin(); elem != list_x.end(); elem++, w++) {
		weighted_average[0] += *elem * (*w);
	}
	for (std::list<int>::iterator elem = list_y.begin(), w = weights.begin(); elem != list_y.end(); elem++, w++) {
		weighted_average[1] += *elem * (*w);
	}

	weighted_average[0] = weighted_average[0] / list_size;
	weighted_average[1] = weighted_average[1] / list_size;

	return weighted_average;
}


int * Smoothing::getBsplineCoordinates() {
	return 0;
}