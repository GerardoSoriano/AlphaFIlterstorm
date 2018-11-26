#pragma once
#include <vector>
#include <opencv2/core/mat.hpp>
#include "HistogramElement.h"
using namespace std;
using namespace cv;

class Histogram {
public:
	int total_pixels;
	int lower_frecuency, higher_frecuency;
	vector <HistogramElement> data;
	vector <HistogramElement> data_aux;
	Histogram();
	~Histogram();
	void create_histogram(Mat, int);
	float histogram_porcentage(Histogram);
	HistogramElement get_histogram_element_by_value(int);
};
