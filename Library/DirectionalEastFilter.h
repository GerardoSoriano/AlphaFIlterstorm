#pragma once
#include "Filter.h"
class DirectionalEastFilter :
	public Filter
{
private:
	int msize, weight;
	int **mask;
	void bucle(uchar*&, uchar*&, uint, uint) override;
	void make_mask();
public:
	DirectionalEastFilter();
	~DirectionalEastFilter();
	void reset();
	void modify(int, int);
};

