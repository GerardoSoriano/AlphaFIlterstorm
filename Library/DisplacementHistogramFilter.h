#pragma once
#include "Filter.h"
class DisplacementHistogramFilter :
	public Filter
{
private:
	int displacement;
	void bucle(uchar*&, uchar*&, uint, uint) override;
	virtual void computed_results();
public:
	DisplacementHistogramFilter();
	~DisplacementHistogramFilter();
	const wchar_t* get_name() override;
	void reset();
	void modify(int);
};

