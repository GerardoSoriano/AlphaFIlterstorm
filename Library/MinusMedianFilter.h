#pragma once
#include "Filter.h"
class MinusMedianFilter :
	public Filter
{
private:
	int msize;
	int** mask;
	void bucle(uchar*&, uchar*&, uint, uint) override;
	void make_mask();
public:
	MinusMedianFilter();
	~MinusMedianFilter();
	void reset();
	void modify(int);
};