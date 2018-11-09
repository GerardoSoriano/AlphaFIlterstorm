#include "FilterFactory.h"
#include "MedianFilter.h"
#include "SobelFilter.h"
#include "WeightedMedianFilter.h"
#include "MinusMedianFilter.h"
#include "AverageFilter.h"


FilterFactory::FilterFactory(FilterList _choise) : choise(_choise) {};


FilterFactory::~FilterFactory()
{
}

Filter* FilterFactory::createFilter() const
{
	switch (choise)
	{
	case _Median:
		return new MedianFilter();
	case _WeightedMedian:
		return new WeightedMedianFilter();
	case _MinusMedian:
		return new MinusMedianFilter();
	case _Average:
		return new AverageFilter();
	case _Sobel:
		return new SobelFilter();
	default:
		return nullptr;
	}
}
