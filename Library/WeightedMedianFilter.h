#pragma once
#include "Filter.h"
class WeightedMedianFilter :
	public Filter
{
private:
	int msize, weight;
	int** mask;
	void bucle(uchar*&, uchar*&, uint, uint) override;
	void computed_results() override;
	void make_mask();
public:
	WeightedMedianFilter();
	~WeightedMedianFilter();
	const wchar_t* get_name() override;
	void reset();
	void modify(int, int);
};

