#include "MedianFilter.h"


void MedianFilter::bucle(uchar*& _input, uchar*& _output, uint _x, uint _y)
{
	int sum_b = 0;
	int sum_g = 0;
	int sum_r = 0;

	for (int i = 0; i < msize; i++)
	{
		for (int j = 0; j < msize; j++)
		{
			_input = base->image.ptr<uchar>((_y - substractor) + i);

			const int px = (_x - substractor) + j;
			const int py = (_y - substractor) + i;

			if (px > result->cols - substractor || py > result->rows - substractor) {
				sum_b = sum_b + 0;
				sum_g = sum_g + 0;
				sum_r = sum_r + 0;
			}
			else
			{
				const float b = _input[px * 3];;
				const float g = _input[px * 3 + 1];
				const float r = _input[px * 3 + 2];

				sum_b = sum_b + (mask[j][i] * b);
				sum_g = sum_g + (mask[j][i] * g);
				sum_r = sum_r + (mask[j][i] * r);
			}
		}
	}
	sum_b = sum_b / pow(msize, 2);
	sum_g = sum_g / pow(msize, 2);
	sum_r = sum_r / pow(msize, 2);

	if (sum_b < 0)
		sum_b = 0;
	if (sum_g < 0)
		sum_g = 0;
	if (sum_r < 0)
		sum_r = 0;

	if (sum_b > 255)
		sum_b = 255;
	if (sum_g > 255)
		sum_g = 255;
	if (sum_r > 255)
		sum_r = 255;

	_output[_x * 3] = sum_b;
	_output[_x * 3 + 1] = sum_g;
	_output[_x * 3 + 2] = sum_r;

}

void MedianFilter::make_mask()
{
	mask = new int*[msize];
	substractor = msize / 2;
	for (int i = 0; i < msize; ++i)
		mask[i] = new int[msize];

	for (int i = 0; i < msize; i++)
		for (int j = 0; j < msize; j++)
		{
			mask[i][j] = 1;
		}
}

MedianFilter::MedianFilter(): Filter(), msize(3)
{
	substractor = msize / 2;
	make_mask();
}

MedianFilter::~MedianFilter()
{
	for (int i = 0; i < msize; ++i)
		delete[] mask[i];
	delete mask;
}

const wchar_t* MedianFilter::get_name()
{
	return TEXT("Filtro de la media");
}

void MedianFilter::reset()
{
	modify(3);
}

void MedianFilter::modify(int _msize)
{
	for (int i = 0; i < msize; ++i)
		delete[] mask[i];
	delete result, mask;
	if (_msize % 2 == 0)
		_msize += 1;
	msize = _msize;
	make_mask();
	result = new Picture(base->image);
}