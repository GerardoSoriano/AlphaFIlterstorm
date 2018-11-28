#pragma once
#include "Filter.h"

class SimpleEqualizationHistogramFilter :
	public Filter
{
private:
	void bucle(uchar*&, uchar*&, uint, uint) override;
	void computed_results() override;
public:
	SimpleEqualizationHistogramFilter();
	~SimpleEqualizationHistogramFilter();
	const wchar_t* get_name() override;
};

