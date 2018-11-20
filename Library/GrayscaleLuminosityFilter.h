#pragma once
#include "Filter.h"
class GrayscaleLuminosityFilter :
	public Filter
{
private:
	void bucle(uchar*&, uchar*&, uint, uint) override;
public:
	GrayscaleLuminosityFilter();
	~GrayscaleLuminosityFilter();
	const wchar_t* get_name() override;
};