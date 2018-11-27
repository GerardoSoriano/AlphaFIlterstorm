#include "HighlightFilter.h"


void HighlightFilter::bucle(uchar*& _input, uchar*& _output, uint _x, uint _y)
{
	_input = base->image.ptr<uchar>(_y);

	const float b = _input[_x * 3];
	const float g = _input[_x * 3 + 1];
	const float r = _input[_x * 3 + 2];

	const float grey = highlight * log(((b + g + r) / 3) + 1);

	_output[_x * 3] = static_cast<int>(grey);
	_output[_x * 3 + 1] = static_cast<int>(grey);
	_output[_x * 3 + 2] = static_cast<int>(grey);
}

HighlightFilter::HighlightFilter(): Filter(), highlight(30)
{
	substractor = 0;
}


HighlightFilter::~HighlightFilter()
{
}

const wchar_t* HighlightFilter::get_name()
{
	return TEXT("Filtro de realce");
}

void HighlightFilter::reset()
{
	modify(30);
}

void HighlightFilter::modify(int _highlight)
{
	delete result;
	highlight = _highlight;
	result = new Picture(base->image);
}
