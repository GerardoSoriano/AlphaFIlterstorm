#pragma once
#include "Filter.h"
class DirectionalNorthFilter :
	public Filter
{
private:
	int msize, weight;
	int **mask;
	void bucle(uchar*&, uchar*&, uint, uint) override;
	void make_mask();
public:
	DirectionalNorthFilter();
	~DirectionalNorthFilter();
	void reset();
	void modify(int, int);
};

