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

void UmbralFilter::computed_results()
{
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

void UmbralFilter::reset()
{
	modify(40, 190);
}

void UmbralFilter::modify(int _first_umbral, int _last_umbral)
{
	delete result;
	_last_umbral = (_last_umbral <= _first_umbral) ? _first_umbral + 1 : _last_umbral;
	_first_umbral = (_first_umbral > 255) ? 255 : _first_umbral;
	_first_umbral = (_first_umbral < 0) ? 0 : _first_umbral;
	_last_umbral = (_last_umbral > 255) ? 255 : _last_umbral;
	_last_umbral = (_last_umbral < 0) ? 0 : _last_umbral;
	_first_umbral = (_first_umbral >= _last_umbral) ? _last_umbral - 1 : _first_umbral;
	first_umbral = _first_umbral;
	last_umbral = _last_umbral;
	result = new Picture(base->image);
}
