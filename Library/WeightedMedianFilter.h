#pragma once
#include "Filter.h"
class WeightedMedianFilter :
	public Filter
{
private:
	int msize, weight, substractor;
	int** mask;
	void bucle(uchar*&, uchar*&, uint, uint) override;
	void make_mask();
public:
	WeightedMedianFilter();
	~WeightedMedianFilter();
	void apply() override;
	void reset();
	void modify(int _msize, int _weight);
};

