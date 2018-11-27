#include "EqualizationHistogramFilter.h"


void EqualizationHistogramFilter::bucle(uchar*& _input, uchar*& _output, uint _x, uint _y)
{
	_input = base->image.ptr<uchar>(_y);

	const float b = _input[_x * 3];
	const float g = _input[_x * 3 + 1];
	const float r = _input[_x * 3 + 2];

	const auto he_b = base->hist_b.get_histogram_element_by_value(b);
	const auto he_g = base->hist_g.get_histogram_element_by_value(g);
	const auto he_r = base->hist_r.get_histogram_element_by_value(r);

	const auto fb = (float(he_b.cdf) - float(base->hist_b.data[0].cdf)) / (float(base->hist_b.total_pixels) - float(base->hist_b.data[0].cdf)) * 255;
	const auto fg = (float(he_g.cdf) - float(base->hist_g.data[0].cdf)) / (float(base->hist_g.total_pixels) - float(base->hist_g.data[0].cdf)) * 255;
	const auto fr = (float(he_r.cdf) - float(base->hist_r.data[0].cdf)) / (float(base->hist_r.total_pixels) - float(base->hist_r.data[0].cdf)) * 255;

	_output[_x * 3] = static_cast<int>(fb);
	_output[_x * 3 + 1] = static_cast<int>(fg);
	_output[_x * 3 + 2] = static_cast<int>(fr);
}

EqualizationHistogramFilter::EqualizationHistogramFilter(): Filter()
{
	need_hist = true;
	substractor = 0;
}

EqualizationHistogramFilter::~EqualizationHistogramFilter() = default;

const wchar_t* EqualizationHistogramFilter::get_name()
{
	return TEXT("Filtro del histograma ecualizado");
}
