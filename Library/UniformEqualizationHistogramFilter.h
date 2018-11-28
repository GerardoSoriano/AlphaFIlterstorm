#pragma once
#include "Filter.h"
class UniformEqualizationHistogramFilter :
	public Filter
{
private:
	void bucle(uchar*&, uchar*&, uint, uint) override;
	void computed_results() override;
public:
	UniformEqualizationHistogramFilter();
	~UniformEqualizationHistogramFilter();
	const wchar_t* get_name() override;
};

