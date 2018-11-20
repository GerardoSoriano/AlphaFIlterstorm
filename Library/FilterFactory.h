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
	_Gaussian,
	_NormalizeHistogram
};

class FilterFactory
{
private:
	FilterList choise;
	FilterFactory();
public:
	~FilterFactory();
	static FilterFactory& get_instance();
	void change_choise(FilterList);
	Filter* createFilter() const;
};

