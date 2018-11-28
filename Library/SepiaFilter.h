#pragma once
#include "Filter.h"
class SepiaFilter :
	public Filter
{
private:
	void bucle(uchar*&, uchar*&, uint, uint) override;
	void computed_results() override;
public:
	SepiaFilter();
	~SepiaFilter();
	const wchar_t* get_name() override;
};

