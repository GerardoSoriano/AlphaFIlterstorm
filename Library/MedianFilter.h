#pragma once
#include "Filter.h"
class MedianFilter :
	public Filter
{
private:
	int msize;
	int mask[3][3];
	void bucle(uchar*&, uchar*&, uint, uint) override;
	void make_mask();
public:
	MedianFilter();
	~MedianFilter();
	void reset() const override;
	void modify(int);
};

