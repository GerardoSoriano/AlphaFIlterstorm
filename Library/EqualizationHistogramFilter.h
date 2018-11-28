#pragma once
#include "Filter.h"
class EqualizationHistogramFilter :
	public Filter
{
private:
	void bucle(uchar*&, uchar*&, uint, uint) override;
public:
	EqualizationHistogramFilter();
	~EqualizationHistogramFilter();
	const wchar_t* get_name() override;
	void computed_results() override;
};

