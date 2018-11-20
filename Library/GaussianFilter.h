#pragma once
#include "Filter.h"
class GaussianFilter :
	public Filter
{
private:
	int msize, sum_mask;
	int **mask;
	float sigma;
	void bucle(uchar*&, uchar*&, uint, uint) override;
	void make_mask();
public:
	GaussianFilter();
	~GaussianFilter();
	const wchar_t* get_name() override;
	void reset();
	void modify(int, float);
};

