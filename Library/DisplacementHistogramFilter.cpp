#include "DisplacementHistogramFilter.h"



void DisplacementHistogramFilter::bucle(uchar*& _input, uchar*& _output, uint _x, uint _y)
{
	_input = base->image.ptr<uchar>(_y);

	float b = _input[_x * 3];
	float g = _input[_x * 3 + 1];
	float r = _input[_x * 3 + 2];

	b += displacement;
	g += displacement;
	r += displacement;

	if (b < 0)
		b = 0;
	if (b > 255)
		b = 255;
	if (g < 0)
		g = 0;
	if (g > 255)
		g = 255;
	if (r < 0)
		r = 0;
	if (r > 255)
		r = 255;

	_output[_x * 3] = static_cast<int>(b);
	_output[_x * 3 + 1] = static_cast<int>(g);
	_output[_x * 3 + 2] = static_cast<int>(r);
}

void DisplacementHistogramFilter::computed_results()
{
}

DisplacementHistogramFilter::DisplacementHistogramFilter() : Filter(), displacement(50)
{
	substractor = 0;
}


DisplacementHistogramFilter::~DisplacementHistogramFilter() = default;

const wchar_t * DisplacementHistogramFilter::get_name()
{
	return TEXT("Filtro del histograma desplazado");
}

void DisplacementHistogramFilter::reset()
{
	modify(50);
}

void DisplacementHistogramFilter::modify(int _displacement)
{
	delete result;
	displacement = _displacement;
	result = new Picture(base->image);
}
