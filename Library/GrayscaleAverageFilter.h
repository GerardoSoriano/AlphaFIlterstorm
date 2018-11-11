#pragma once
#include "Filter.h"
class GrayscaleAverageFilter :
	public Filter
{
private:
	void bucle(uchar*&, uchar*&, uint, uint) override;
public:
	GrayscaleAverageFilter();
	~GrayscaleAverageFilter();
};

