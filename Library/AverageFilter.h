#pragma once
#include "Filter.h"
class AverageFilter :
	public Filter
{
private:
	int msize;
	int **arr_b, **arr_g, **arr_r;
	void bucle(uchar*&, uchar*&, uint, uint) override;
	void make_mask();
	static int min_element(int[], int);
	static void swap(int&, int&) noexcept;
	static void sort(int*, int);
public:
	AverageFilter();
	~AverageFilter();
	const wchar_t* get_name() override;
	void reset();
	void modify(int);
};

