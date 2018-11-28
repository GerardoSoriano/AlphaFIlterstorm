#pragma once
#include "Picture.h"

const enum FilterList {
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
	_NormalizeHistogram,
	_EqualizationHistogram,
	_SimpleEqualizationHistogram,
	_UniformEqualizationHistogram,
	_ExponentialEqualizationHistogram,
	_DisplacementHistogram,
	_Umbral,
	_Highlight
};
struct Result
{
	int origin;
	int result;
};
class Filter
{
protected:
	vector<Result> results;
	bool need_hist;
	int substractor;
	Picture *base, *result;
	virtual void bucle(uchar*&, uchar*&, uint, uint) = 0;
	virtual void computed_results() = 0;
public:
	Filter();
	virtual ~Filter();
	virtual const wchar_t* get_name();
	void apply();
	void set_image(string);
	void set_image(Mat);
	Picture* get_base_image() const;
	Picture* get_result_image() const;
};