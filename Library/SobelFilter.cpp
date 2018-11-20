#include "SobelFilter.h"
#include <iostream>


void SobelFilter::bucle(uchar*& _input, uchar*& _output, uint _x, uint _y)
{
	int sum_x = 0;
	int sum_y = 0;

	for (int i = 0; i < msize; i++)
	{
		for (int j = 0; j < msize; j++)
		{
			_input = base->image.ptr<uchar>((_y - substractor) + i);

			const int px = (_x - substractor) + j;

			const float b = _input[px * 3];;
			const float g = _input[px * 3 + 1];
			const float r = _input[px * 3 + 2];

			const float gray = (b + g + r) / 3;

			sum_x = sum_x + maskX[j][i] * gray;
			sum_y = sum_y + maskY[j][i] * gray;
		}
	}
	int sum = sqrt(pow(sum_x, 2) + pow(sum_y, 2));

	if (sum < 0)
		sum = 0;
	if (sum > 255)
		sum = 255;

	_output[_x * 3] = sum;
	_output[_x * 3 + 1] = sum;
	_output[_x * 3 + 2] = sum;
}

void SobelFilter::make_mask()
{
	maskX = new int*[msize];
	maskY = new int*[msize];
	substractor = msize / 2;
	for (int i = 0; i < msize; ++i)
	{
		maskX[i] = new int[msize];
		maskY[i] = new int[msize];
	}
	for (int i = 0; i < msize; i++)
	{
		int k = (i <= substractor) ? (substractor + i) : (msize + substractor - i - 1);
		for (int j = 0; j < msize; j++)
		{
			if (j < substractor)
				maskX[i][j] = maskY[j][i] = j - k;
			else if (j > substractor)
				maskX[i][j] = maskY[j][i] = k - (msize - j - 1);
			else
				maskX[i][j] = maskY[j][i] = 0;
		}
	}
}

SobelFilter::SobelFilter(): Filter(), msize(3)
{
	substractor = msize / 2;
	make_mask();
}

SobelFilter::~SobelFilter()
{
	for (int i = 0; i < msize; ++i)
		delete[] maskX[i], maskY[i];
	delete maskX, maskY;
}

const wchar_t* SobelFilter::get_name()
{
	return TEXT("Filtro sobel");
}

void SobelFilter::reset()
{
	modify(3);
}

void SobelFilter::modify(int _msize)
{
	for (int i = 0; i < msize; ++i)
		delete[] maskX[i], maskY[i];
	delete maskX, maskY;
	if (_msize % 2 == 0)
		_msize += 1;
	msize = _msize;
	make_mask();
	result = new Picture(base->image);

}
