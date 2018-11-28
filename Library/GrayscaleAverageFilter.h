#pragma once
#include "Filter.h"
class GrayscaleAverageFilter :
	public Filter
{
private:
	void bucle(uchar*&, uchar*&, uint, uint) override;
	void computed_results() override;
public:
	GrayscaleAverageFilter();
	~GrayscaleAverageFilter();
	const wchar_t* get_name() override;
};

