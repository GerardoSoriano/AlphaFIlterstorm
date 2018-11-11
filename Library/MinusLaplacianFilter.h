#pragma once
#include "Filter.h"
class MinusLaplacianFilter :
	public Filter
{
private:
	int weight;
	int mask[3][3];
	void bucle(uchar*&, uchar*&, uint, uint) override;
	void make_mask();
public:
	MinusLaplacianFilter();
	~MinusLaplacianFilter();
	void reset();
	void modify(int);
};

