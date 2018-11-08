#pragma once
#include "Filter.h"
class MedianFilter :
	public Filter
{
private:
	int msize, substractor;
	int** mask;
	void bucle(uchar*&, uchar*&, uint, uint) override;
	void make_mask();
public:
	MedianFilter();
	~MedianFilter();
	void apply() override;
	void reset();
	void modify(int);
};

