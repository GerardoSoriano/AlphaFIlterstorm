#include "AverageFilter.h"


void AverageFilter::bucle(uchar*& _input, uchar*& _output, uint _x, uint _y)
{
	for (int i = 0; i < msize; i++)
	{
		for (int j = 0; j < msize; j++)
		{
			_input = base->image.ptr<uchar>((_y - substractor) + i);

			const int px = (_x - substractor) + j;

			arr_b[j][i] = _input[px * 3];
			arr_g[j][i] = _input[px * 3 + 1];
			arr_r[j][i] = _input[px * 3 + 2];
		}
	}

	sort(arr_b[0], pow(msize, 2));
	sort(arr_g[0], pow(msize, 2));
	sort(arr_r[0], pow(msize, 2));

	_output[_x * 3] = arr_b[1][1];
	_output[_x * 3 + 1] = arr_g[1][1];
	_output[_x * 3 + 2] = arr_r[1][1];
}

void AverageFilter::computed_results()
{
}

void AverageFilter::make_mask()
{
	arr_b = new int*[msize];
	arr_g = new int*[msize];
	arr_r = new int*[msize];
	substractor = msize / 2;
	for (int i = 0; i < msize; ++i)
	{
		arr_b[i] = new int[msize];
		arr_g[i] = new int[msize];
		arr_r[i] = new int[msize];
	}
}

int AverageFilter::min_element(int a[], int i)
{
	if (i == 0) return 0;
	int j = min_element(a, i - 1);
	if (a[i - 1] < a[j]) return i - 1;
	return j;
}

void AverageFilter::swap(int& x, int& y) noexcept
{
	const int temp = x;
	x = y;
	y = temp;
}

void AverageFilter::sort(int* a, int sz)
{
	if (sz == 1) return;
	int j = min_element(a, sz);
	if (j != 0) swap(*a, a[j]);
	sort(++a, sz - 1);
}

AverageFilter::AverageFilter(): Filter(), msize(3)
{
	substractor = msize / 2;
	make_mask();
}

AverageFilter::~AverageFilter()
{
	delete arr_b, arr_g, arr_r;
}

const wchar_t* AverageFilter::get_name()
{
	return TEXT("Filtro por promedio");
}

void AverageFilter::reset()
{
	modify(3);
}

void AverageFilter::modify(int _msize)
{
	delete result, arr_b, arr_g, arr_r;
	if (_msize % 2 == 0)
		_msize += 1;
	msize = _msize;
	make_mask();
	result = new Picture(base->image);
}
