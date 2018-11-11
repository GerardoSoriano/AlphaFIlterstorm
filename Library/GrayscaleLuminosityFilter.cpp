#include "GrayscaleLuminosityFilter.h"


void GrayscaleLuminosityFilter::bucle(uchar*& _input, uchar*& _output, uint _x, uint _y)
{
	_input = base->image.ptr<uchar>(_y);

	const float b = _input[_x * 3];
	const float g = _input[_x * 3 + 1];
	const float r = _input[_x * 3 + 2];

	const float gray = (r*0.3) + (g*0.59) + (b*0.11);

	_output[_x * 3] = static_cast<int>(gray);
	_output[_x * 3 + 1] = static_cast<int>(gray);
	_output[_x * 3 + 2] = static_cast<int>(gray);
}

GrayscaleLuminosityFilter::GrayscaleLuminosityFilter()
{
	substractor = 0;
}


GrayscaleLuminosityFilter::~GrayscaleLuminosityFilter()
{
}
