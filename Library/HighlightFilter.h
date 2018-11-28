#pragma once
#include "Filter.h"
class HighlightFilter :
	public Filter
{
private:
	int highlight;
	void bucle(uchar*&, uchar*&, uint, uint) override;
	void computed_results() override;
public:
	HighlightFilter();
	~HighlightFilter();
	const wchar_t* get_name() override;
	void reset();
	void modify(int);
};

