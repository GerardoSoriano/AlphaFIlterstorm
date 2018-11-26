#pragma once
#include "Filter.h"
class HighlightFilter :
	public Filter
{
private:
	int highlight;
	void bucle(uchar*&, uchar*&, uint, uint) override;
public:
	HighlightFilter();
	~HighlightFilter();
	const wchar_t* get_name() override;
};

