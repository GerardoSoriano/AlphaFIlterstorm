#include "ExponentialEqualizationHistogramFilter.h"



void ExponentialEqualizationHistogramFilter::bucle(uchar*& _input, uchar*& _output, uint _x, uint _y)
{
	_input = base->image.ptr<uchar>(_y);

	const float b = _input[_x * 3];
	const float g = _input[_x * 3 + 1];
	const float r = _input[_x * 3 + 2];

	const auto he_b = base->hist_b.get_histogram_element_by_value(b);
	const auto he_g = base->hist_g.get_histogram_element_by_value(g);
	const auto he_r = base->hist_r.get_histogram_element_by_value(r);

	const auto fb = float(base->hist_b.data[0].value) - (1 / alpha) * log(1 - (float(he_b.cdf) / float(base->hist_b.data.back().cdf)));
	const auto fg = float(base->hist_g.data[0].value) - (1 / alpha) * log(1 - (float(he_g.cdf) / float(base->hist_g.data.back().cdf)));
	const auto fr = float(base->hist_r.data[0].value) - (1 / alpha) * log(1 - (float(he_r.cdf) / float(base->hist_r.data.back().cdf)));

	_output[_x * 3] = static_cast<int>(fb);
	_output[_x * 3 + 1] = static_cast<int>(fg);
	_output[_x * 3 + 2] = static_cast<int>(fr);
}

ExponentialEqualizationHistogramFilter::ExponentialEqualizationHistogramFilter(): Filter(), alpha(0.01)
{
	need_hist = true;
	substractor = 0;
}


ExponentialEqualizationHistogramFilter::~ExponentialEqualizationHistogramFilter() = default;

const wchar_t * ExponentialEqualizationHistogramFilter::get_name()
{
	return TEXT("Filtro del histograma con ecualización exponencial");
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
