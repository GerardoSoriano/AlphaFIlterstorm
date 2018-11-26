#include "ExponentialEqualizationHistogramFilter.h"



void ExponentialEqualizationHistogramFilter::bucle(uchar*& _input, uchar*& _output, uint _x, uint _y)
{
	_input = base->image.ptr<uchar>(_y);

	float b = _input[_x * 3];
	float g = _input[_x * 3 + 1];
	float r = _input[_x * 3 + 2];

	const HistogramElement he_b = base->hist_b.get_histogram_element_by_value(b);
	const HistogramElement he_g = base->hist_g.get_histogram_element_by_value(g);
	const HistogramElement he_r = base->hist_r.get_histogram_element_by_value(r);

	const float fb = float(base->hist_b.data[0].value) - (1 / alpha) * log(1 - (float(he_b.cdf) / float(base->hist_b.data.back().cdf)));
	const float fg = float(base->hist_g.data[0].value) - (1 / alpha) * log(1 - (float(he_g.cdf) / float(base->hist_g.data.back().cdf)));
	const float fr = float(base->hist_r.data[0].value) - (1 / alpha) * log(1 - (float(he_r.cdf) / float(base->hist_r.data.back().cdf)));

	_output[_x * 3] = static_cast<int>(fb);
	_output[_x * 3 + 1] = static_cast<int>(fg);
	_output[_x * 3 + 2] = static_cast<int>(fr);
}

ExponentialEqualizationHistogramFilter::ExponentialEqualizationHistogramFilter(): Filter(), alpha(0.01)
{
	substractor = 0;
}


ExponentialEqualizationHistogramFilter::~ExponentialEqualizationHistogramFilter()
{
}

const wchar_t * ExponentialEqualizationHistogramFilter::get_name()
{
	return TEXT("Filtro del histograma con ecualizado exponencial");
}

void ExponentialEqualizationHistogramFilter::reset()
{
	modify(0.01);
}

void ExponentialEqualizationHistogramFilter::modify(int _alpha)
{
	delete result;
	alpha = _alpha;
	result = new Picture(base->image);
}
