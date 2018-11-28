#pragma once
#include "Filter.h"
class MinusMedianFilter :
	public Filter
{
private:
	int msize;
	int** mask;
	void bucle(uchar*&, uchar*&, uint, uint) override;
	void computed_results() override;
	void make_mask();
public:
	MinusMedianFilter();
	~MinusMedianFilter();
	const wchar_t* get_name() override;
	void reset();
	void modify(int);
};