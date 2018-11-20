#include "GaussianFilter.h"
#include <iostream>


void GaussianFilter::bucle(uchar*& _input, uchar*& _output, uint _x, uint _y)
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

			const float b = _input[px * 3];;
			const float g = _input[px * 3 + 1];
			const float r = _input[px * 3 + 2];

			sum_b = sum_b + (mask[i][j] * b);
			sum_g = sum_g + (mask[i][j] * g);
			sum_r = sum_r + (mask[i][j] * r);

			/*if (px > result->cols - substractor || py > result->rows - substractor) {
				sum_b = sum_b + 0;
				sum_g = sum_g + 0;
				sum_r = sum_r + 0;
			}
			else
			{
			
			}*/
		}
	}

	sum_b = sum_b / sum_mask;
	sum_g = sum_g / sum_mask;
	sum_r = sum_r / sum_mask;

	/*if (sum_b < 0)
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
		sum_r = 255;*/

	_output[_x * 3] = sum_b;
	_output[_x * 3 + 1] = sum_g;
	_output[_x * 3 + 2] = sum_r;
}

void GaussianFilter::make_mask()
{
	float **pre = new float*[msize];
	mask = new int*[msize];
	substractor = msize / 2;
	for (int i = 0; i < msize; ++i)
	{
		mask[i] = new int[msize];
		pre[i] = new float[msize];
	}
	for (int i = 0; i < msize; i++)
		for (int j = 0; j < msize; j++)
		{
			const int px = j - substractor;
			const int py = i - substractor;

			float product = (px * px) + (py * py);
			product = product / (2 * pow(sigma, 2));
			product = exp(-product);
			product = product / ((2 * 3.1416) * pow(sigma, 2));
			pre[i][j] = product;
			//cout << pre[i][j] << endl;
		}
	for (int i = 0; i < msize; i++)
		for (int j = 0; j < msize; j++)
		{
			mask[i][j] = static_cast<int>(pre[i][j] / pre[0][0]);
			sum_mask += mask[i][j];
			//cout << mask[i][j] << endl;
		}
	for (int i = 0; i < msize; ++i)
		delete[] pre[i];
	delete pre;
	//system("pause");
}

GaussianFilter::GaussianFilter(): Filter(), msize(3), sum_mask(0), sigma(1.0)
{
	substractor = msize / 2;
	make_mask();
}


GaussianFilter::~GaussianFilter()
{
	for (int i = 0; i < msize; ++i)
		delete[] mask[i];
	delete mask;
}

const wchar_t* GaussianFilter::get_name()
{
	return TEXT("Filtro gaussiano");
}

void GaussianFilter::reset()
{
}

void GaussianFilter::modify(int _msize, float _sigma)
{
	for (int i = 0; i < msize; ++i)
		delete[] mask[i];
	delete result, mask;
	if (_msize % 2 == 0)
		_msize += 1;
	msize = _msize;
	sigma = _sigma;
	sum_mask = 0;
	make_mask();
	result = new Picture(base->image);
}
