#include "FilterFactory.h"
#include "MedianFilter.h"
#include "SobelFilter.h"
#include "WeightedMedianFilter.h"
#include "MinusMedianFilter.h"
#include "AverageFilter.h"
#include "LaplacianFilter.h"
#include "MinusLaplacianFilter.h"
#include "DirectionalNorthFilter.h"
#include "DirectionalEastFilter.h"
#include "GrayscaleAverageFilter.h"
#include "GrayscaleLuminosityFilter.h"
#include "GrayscaleLuminanceFilter.h"
#include "SepiaFilter.h"
#include "GaussianFilter.h"
#include "NormalizeHistogramFilter.h"
#include "UmbralFilter.h"
#include "HighlightFilter.h"


FilterFactory::FilterFactory()
{
}

FilterFactory::~FilterFactory()
{
}

FilterFactory& FilterFactory::get_instance()
{
	static FilterFactory ff;
	return ff;
}

void FilterFactory::change_choise(FilterList _choise)
{
	choise = _choise;
};

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
	case _Laplacian:
		return new LaplacianFilter();
	case _MinusLaplacian:
		return new MinusLaplacianFilter();
	case _DirectionalNorth:
		return new DirectionalNorthFilter();
	case _DirectionalEast:
		return new DirectionalEastFilter();
	case _GrayscaleAverage:
		return new GrayscaleAverageFilter();
	case _GrayscaleLuminosity:
		return new GrayscaleLuminosityFilter();
	case _GrayscaleLuminance:
		return new GrayscaleLuminanceFilter();
	case _Sepia:
		return new SepiaFilter();
	case _Sobel:
		return new SobelFilter();
	case _Gaussian:
		return new GaussianFilter();
	case _Umbral:
		return new UmbralFilter();
	case _Highlight:
		return new HighlightFilter();
	default:
		return nullptr;
	}
}
