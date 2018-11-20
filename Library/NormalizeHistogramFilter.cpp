#include "NormalizeHistogramFilter.h"


void NormalizeHistogramFilter::bucle(uchar*& _input, uchar*& _output, uint _x, uint _y)
{
	_input = base->image.ptr<uchar>(_y);

	float b = _input[_x * 3];
	float g = _input[_x * 3 + 1];
	float r = _input[_x * 3 + 2];

	const int total_of_pixels = base->cols * base->rows;
	
	b /= total_of_pixels;
	g /= total_of_pixels;
	r /= total_of_pixels;

	_output[_x * 3] = static_cast<int>(b);
	_output[_x * 3 + 1] = static_cast<int>(g);
	_output[_x * 3 + 2] = static_cast<int>(r);
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
	return TEXT("");
}
