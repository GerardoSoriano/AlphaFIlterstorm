#pragma once
#include "Filter.h"
class MinusLaplacianFilter :
	public Filter
{
private:
	int weight;
	int mask[3][3];
	void bucle(uchar*&, uchar*&, uint, uint) override;
	void computed_results() override;
	void make_mask();
public:
	MinusLaplacianFilter();
	~MinusLaplacianFilter();
	const wchar_t* get_name() override;
	void reset();
	void modify(int);
};

