#include "LaplacianFilter.h"



void LaplacianFilter::bucle(uchar*& _input, uchar*& _output, uint _x, uint _y)
{
	int sum = 0;

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			_input = base->image.ptr<uchar>((_y - 1) + i);

			const int px = (_x - substractor) + j;

			const float b = _input[px * 3];
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

void LaplacianFilter::make_mask()
{
	mask[0][0] = 0;			mask[0][1] = weight;		mask[0][2] = 0;
	mask[1][0] = weight;	mask[1][1] = -(weight * 4);	mask[1][2] = weight;
	mask[2][0] = 0;			mask[2][1] = weight;		mask[2][2] = 0;
}

LaplacianFilter::LaplacianFilter(): Filter(), weight(1)
{
	make_mask();
}

LaplacianFilter::~LaplacianFilter()
{
}

void LaplacianFilter::reset()
{
	modify(1);
}

void LaplacianFilter::modify(int _weight)
{
	delete result;
	weight = _weight;
	make_mask();
	result = new Picture(base->image);
}
