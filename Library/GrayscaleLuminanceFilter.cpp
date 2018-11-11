#include "GrayscaleLuminanceFilter.h"



void GrayscaleLuminanceFilter::bucle(uchar*& _input, uchar*& _output, uint _x, uint _y)
{
	for (int i = 0; i < msize; i++)
	{
		for (int j = 0; j < msize; j++)
		{
			_input = base->image.ptr<uchar>((_y - substractor) + i);

			const int px = (_x - substractor) + j;

			float b = _input[px * 3];
			float g = _input[px * 3 + 1];
			float r = _input[px * 3 + 2];

			mask[j][i] = (b + g + r) / 3;
		}
	}

	sort(mask[0], pow(msize,2));

	_output[_x * 3] = (mask[0][0] + mask[2][2]) / 2;
	_output[_x * 3 + 1] = (mask[0][0] + mask[2][2]) / 2;
	_output[_x * 3 + 2] = (mask[0][0] + mask[2][2]) / 2;
}

void GrayscaleLuminanceFilter::make_mask()
{
	mask = new int*[msize];
	substractor = msize / 2;
	for (int i = 0; i < msize; ++i)
		mask[i] = new int[msize];
}

int GrayscaleLuminanceFilter::min_element(int a[], int i)
{
	if (i == 0) return 0;
	int j = min_element(a, i - 1);
	if (a[i - 1] < a[j]) return i - 1;
	return j;
}

void GrayscaleLuminanceFilter::swap(int& x, int& y) noexcept
{
	const int temp = x;
	x = y;
	y = temp;
}

void GrayscaleLuminanceFilter::sort(int* a, int sz)
{
	if (sz == 1) return;
	int j = min_element(a, sz);
	if (j != 0) swap(*a, a[j]);
	sort(++a, sz - 1);
}

GrayscaleLuminanceFilter::GrayscaleLuminanceFilter(): Filter(), msize(3)
{
	substractor = msize / 2;
	make_mask();
}

GrayscaleLuminanceFilter::~GrayscaleLuminanceFilter()
{
	delete mask;
}

void GrayscaleLuminanceFilter::reset()
{
	modify(3);
}

void GrayscaleLuminanceFilter::modify(int _msize)
{
	delete result, mask;
	if (_msize % 2 == 0)
		_msize += 1;
	msize = _msize;
	make_mask();
	result = new Picture(base->image);
}
