#pragma once
#include "Filter.h"
class SobelFilter :
	public Filter
{
private:
	int msize;
	int **maskX, **maskY;
	void bucle(uchar*&, uchar*&, uint, uint) override;
	void computed_results() override;
	void make_mask();
public:
	SobelFilter();
	~SobelFilter();
	const wchar_t* get_name() override;
	void reset();
	void modify(int);
};
