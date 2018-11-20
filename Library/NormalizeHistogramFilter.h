#pragma once
#include "Filter.h"
class NormalizeHistogramFilter :
	public Filter
{
private:
	void bucle(uchar*&, uchar*&, uint, uint) override;
public:
	NormalizeHistogramFilter();
	~NormalizeHistogramFilter();
	const wchar_t* get_name() override;
};

