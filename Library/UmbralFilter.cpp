#include "UmbralFilter.h"


void UmbralFilter::bucle(uchar*& _input, uchar*& _output, uint _x, uint _y)
{
	_input = base->image.ptr<uchar>(_y);

	const float b = _input[_x * 3];
	const float g = _input[_x * 3 + 1];
	const float r = _input[_x * 3 + 2];

	float gray = (b + g + r) / 3;

	gray = (gray > first_umbral && gray < last_umbral) ? 255 : 0;

	_output[_x * 3] = static_cast<int>(gray);
	_output[_x * 3 + 1] = static_cast<int>(gray);
	_output[_x * 3 + 2] = static_cast<int>(gray);
}

UmbralFilter::UmbralFilter(): Filter(), first_umbral(40), last_umbral(190)
{
	substractor = 0;
}


UmbralFilter::~UmbralFilter()
{
}

const wchar_t* UmbralFilter::get_name()
{
	return TEXT("Filtro del umbral");
}
