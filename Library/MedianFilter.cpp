#include "MedianFilter.h"


void MedianFilter::bucle(uchar*& _input, uchar*& _output, uint _x, uint _y)
{
	int sum_b = 0;
	int sum_g = 0;
	int sum_r = 0;

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			_input = base->image.ptr<uchar>((_y - 1) + i);

			const int px = (_x - 1) + j;
			const int py = (_y - 1) + i;

			if (px > result->cols - 1 || py > result->rows - 1) {
				sum_b = sum_b + 0;
				sum_g = sum_g + 0;
				sum_r = sum_r + 0;
			}
			else
			{
				const float b = _input[((_x - 1) + j) * 3];;
				const float g = _input[((_x - 1) + j) * 3 + 1];
				const float r = _input[((_x - 1) + j) * 3 + 2];

				sum_b = sum_b + (mask[j][i] * b);
				sum_g = sum_g + (mask[j][i] * g);
				sum_r = sum_r + (mask[j][i] * r);
			}
		}
	}
	sum_b = sum_b / 9;
	sum_g = sum_g / 9;
	sum_r = sum_r / 9;

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

//void MedianFilter::bucle(uchar*& _input, uchar*& _output, uint _x, uint _y)
//{
//	int sum_b = 0;
//	int sum_g = 0;
//	int sum_r = 0;
//
//	for (int i = 0; i < 3; i++)
//	{
//		for (int j = 0; j < 3; j++)
//		{
//			_input = base->image.ptr<uchar>((_y - 1) + i);
//
//			const int px = (_x - 1) + j;
//			const int py = (_y - 1) + i;
//
//			if (px > result->cols - 1 || py > result->rows - 1) {
//				sum_b = sum_b + 0;
//				sum_g = sum_g + 0;
//				sum_r = sum_r + 0;
//			}
//			else
//			{
//				const float b = _input[((_x - 1) + j) * 3];;
//				const float g = _input[((_x - 1) + j) * 3 + 1];
//				const float r = _input[((_x - 1) + j) * 3 + 2];
//
//				sum_b = sum_b + (mask[j][i] * b);
//				sum_g = sum_g + (mask[j][i] * g);
//				sum_r = sum_r + (mask[j][i] * r);
//			}
//		}
//	}
//	sum_b = sum_b / 9;
//	sum_g = sum_g / 9;
//	sum_r = sum_r / 9;
//
//	if (sum_b < 0)
//		sum_b = 0;
//	if (sum_g < 0)
//		sum_g = 0;
//	if (sum_r < 0)
//		sum_r = 0;
//
//	if (sum_b > 255)
//		sum_b = 255;
//	if (sum_g > 255)
//		sum_g = 255;
//	if (sum_r > 255)
//		sum_r = 255;
//
//	_output[_x * 3] = sum_b;
//	_output[_x * 3 + 1] = sum_g;
//	_output[_x * 3 + 2] = sum_r;
//
//}

void MedianFilter::make_mask()
{
	for (int i = 0; i < msize; i++)
		for (int j = 0; j < msize; j++)
		{
			mask[i][j] = 1;
		}
}

MedianFilter::MedianFilter(): Filter()
{
	make_mask();
}

MedianFilter::~MedianFilter()
{
}

void MedianFilter::reset() const
{
	
}

void MedianFilter::modify(int _msize)
{
	delete result;
	msize = _msize;
	make_mask();
	result = new Picture(base->image);
}