#include "DirectionalNorthFilter.h"



void DirectionalNorthFilter::bucle(uchar*& _input, uchar*& _output, uint _x, uint _y)
{
	int sum = 0;

	for (int i = 0; i < msize; i++)
	{
		for (int j = 0; j < msize; j++)
		{
			_input = base->image.ptr<uchar>((_y - substractor) + i);

			const int px = (_x - substractor) + j;

			const float b = _input[px * 3];;
			const float g = _input[px * 3 + 1];
			const float r = _input[px * 3 + 2];

			const float grey = (b + g + r) / 3;

			sum = sum + mask[j][i] * grey;

		}
	}

	if (sum < 0)
		sum = 0;
	if (sum > 255)
		sum = 255;

	_output[_x * 3] = sum;
	_output[_x * 3 + 1] = sum;
	_output[_x * 3 + 2] = sum;
}

void DirectionalNorthFilter::make_mask()
{
	mask = new int*[msize];
	substractor = msize / 2;
	for (int i = 0; i < msize; ++i)
		mask[i] = new int[msize];

	for (int i = 0; i < msize; i++)
		for (int j = 0; j < msize; j++)
		{
			mask[i][j] = (j <= substractor) ? weight:-weight;
		}
	mask[substractor][substractor] = -weight*2;
}

DirectionalNorthFilter::DirectionalNorthFilter(): Filter(), msize(3), weight(1)
{
	substractor = msize / 2;
	make_mask();
}

DirectionalNorthFilter::~DirectionalNorthFilter()
{
	for (int i = 0; i < msize; ++i)
		delete[] mask[i];
	delete result, mask;
}

const wchar_t* DirectionalNorthFilter::get_name()
{
	return TEXT("Filtro del direccional norte");
}

void DirectionalNorthFilter::reset()
{
	modify(3, 1);
}

void DirectionalNorthFilter::modify(int _msize, int _weight)
{
	for (int i = 0; i < msize; ++i)
		delete[] mask[i];
	delete result, mask;
	if (_msize % 2 == 0)
		_msize += 1;
	msize = _msize;
	weight = _weight;
	make_mask();
	result = new Picture(base->image);
}
