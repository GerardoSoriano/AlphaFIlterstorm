#include "NormalizeHistogramFilter.h"


void NormalizeHistogramFilter::bucle(uchar*& _input, uchar*& _output, uint _x, uint _y)
{
	_input = base->image.ptr<uchar>(_y);

	float b = _input[_x * 3];
	float g = _input[_x * 3 + 1];
	float r = _input[_x * 3 + 2];

	const HistogramElement he_b = base->hist_b.get_histogram_element_by_value(b);
	const HistogramElement he_g = base->hist_g.get_histogram_element_by_value(g);
	const HistogramElement he_r = base->hist_r.get_histogram_element_by_value(r);

	const float fb = (float(he_b.frequency) / float(base->hist_b.higher_frecuency)) * 255;
	const float fg = (float(he_g.frequency) / float(base->hist_g.higher_frecuency)) * 255;
	const float fr = (float(he_r.frequency) / float(base->hist_r.higher_frecuency)) * 255;

	_output[_x * 3] = static_cast<int>(fb);
	_output[_x * 3 + 1] = static_cast<int>(fg);
	_output[_x * 3 + 2] = static_cast<int>(fr);
}

NormalizeHistogramFilter::NormalizeHistogramFilter()
{
	substractor = 0;
}


NormalizeHistogramFilter::~NormalizeHistogramFilter()
{
}

const wchar_t* NormalizeHistogramFilter::get_name()
{
	return TEXT("Filtro del histograma normalizado");
}
