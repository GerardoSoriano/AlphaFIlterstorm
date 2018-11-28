#pragma once
#include "Filter.h"
class ExponentialEqualizationHistogramFilter :
	public Filter
{
private:
	float alpha;
	void bucle(uchar*&, uchar*&, uint, uint) override;
	void computed_results() override;
public:
	ExponentialEqualizationHistogramFilter();
	~ExponentialEqualizationHistogramFilter();
	const wchar_t* get_name() override;
	void reset();
	void modify(int);
};

