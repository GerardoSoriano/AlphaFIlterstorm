#pragma once
#include "Filter.h"
class WeightedMedianFilter :
	public Filter
{
private:
	int mask[3][3];
	float weight;
	void bucle(uchar*&, uchar*&, uint, uint) override;
public:
	WeightedMedianFilter();
	~WeightedMedianFilter();
	void reset() const override;
	void modify() const;
};

