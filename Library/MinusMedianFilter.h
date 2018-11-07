#pragma once
#include "Filter.h"
class MinusMedianFilter :
	public Filter
{
private:
	int mask[3][3];
	void bucle(uchar*&, uchar*&, uint, uint) override;
public:
	MinusMedianFilter();
	~MinusMedianFilter();
	void reset() const override;
	void modify() const;
};

