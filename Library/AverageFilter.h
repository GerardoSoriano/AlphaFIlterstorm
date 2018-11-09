#pragma once
#include "Filter.h"
class AverageFilter :
	public Filter
{
private:
	int msize, substractor;
	int **arr_b, **arr_g, **arr_r;
	void bucle(uchar*&, uchar*&, uint, uint) override;
	void make_mask();
	int min_element(int[], int);
	void swap(int&, int&);
	void sort(int*, int);
public:
	AverageFilter();
	~AverageFilter();
	void apply() override;
	void reset();
	void modify(int);
};

