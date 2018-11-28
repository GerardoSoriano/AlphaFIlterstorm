#include "SepiaFilter.h"


void SepiaFilter::bucle(uchar*& _input, uchar*& _output, uint _x, uint _y)
{
	_input = base->image.ptr<uchar>(_y);

	const float b = _input[_x * 3];
	const float g = _input[_x * 3 + 1];
	const float r = _input[_x * 3 + 2];

	float fb = (r*0.272) + (g*0.534) + (b*0.131);
	float fg = (r*0.349) + (g*0.686) + (b*0.168);
	float fr = (r*0.393) + (g*0.769) + (b*0.189);

	if (fb < 0)
		fb = 0;
	if (fg < 0)
		fg = 0;
	if (fr < 0)
		fr = 0;

	if (fb > 255)
		fb = 255;
	if (fg > 255)
		fg = 255;
	if (fr > 255)
		fr = 255;

	_output[_x * 3] = static_cast<int>(fb);
	_output[_x * 3 + 1] = static_cast<int>(fg);
	_output[_x * 3 + 2] = static_cast<int>(fr);
}

void SepiaFilter::computed_results()
{
}

SepiaFilter::SepiaFilter()
{
	substractor = 0;
}


SepiaFilter::~SepiaFilter()
{
}

const wchar_t* SepiaFilter::get_name()
{
	return TEXT("Filtro sepia");
}
