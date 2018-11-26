#include "UniformEqualizationHistogramFilter.h"



void UniformEqualizationHistogramFilter::bucle(uchar*& _input, uchar*& _output, uint _x, uint _y)
{
	_input = base->image.ptr<uchar>(_y);

	float b = _input[_x * 3];
	float g = _input[_x * 3 + 1];
	float r = _input[_x * 3 + 2];

	const HistogramElement he_b = base->hist_b.get_histogram_element_by_value(b);
	const HistogramElement he_g = base->hist_g.get_histogram_element_by_value(g);
	const HistogramElement he_r = base->hist_r.get_histogram_element_by_value(r);

	const float fb = (float(base->hist_b.data.back().value) - float(base->hist_b.data[0].value)) * (float(he_b.cdf) / float(base->hist_b.data.back().cdf)) + float(base->hist_b.data[0].value);
	const float fg = (float(base->hist_g.data.back().value) - float(base->hist_g.data[0].value)) * (float(he_g.cdf) / float(base->hist_g.data.back().cdf)) + float(base->hist_g.data[0].value);
	const float fr = (float(base->hist_r.data.back().value) - float(base->hist_r.data[0].value)) * (float(he_r.cdf) / float(base->hist_r.data.back().cdf)) + float(base->hist_r.data[0].value);

	_output[_x * 3] = static_cast<int>(fb);
	_output[_x * 3 + 1] = static_cast<int>(fg);
	_output[_x * 3 + 2] = static_cast<int>(fr);
}

UniformEqualizationHistogramFilter::UniformEqualizationHistogramFilter()
{
	substractor = 0;
}


UniformEqualizationHistogramFilter::~UniformEqualizationHistogramFilter()
{
}

const wchar_t * UniformEqualizationHistogramFilter::get_name()
{
	return TEXT("Filtro del histograma con ecualizado uniforme");
}
