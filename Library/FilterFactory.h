#pragma once
#include "Filter.h"

const enum FilterList{
	_Median,
	_WeightedMedian,
	_MinusMedian,
	_Average,
	_Laplacian,
	_MinusLaplacian,
	_DirectionalNorth,
	_DirectionalEast,
	_GrayscaleAverage,
	_GrayscaleLuminosity,
	_GrayscaleLuminance,
	_Sepia,
	_Sobel,
	_Gaussian
};

class FilterFactory
{
public:
	FilterList choise;
	FilterFactory(FilterList _choise);
	~FilterFactory();
	Filter* createFilter() const;
};

