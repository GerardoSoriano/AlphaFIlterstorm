#pragma once
#include "Filter.h"
class UmbralFilter :
	public Filter
{
private:
	int first_umbral, last_umbral;
	void bucle(uchar*&, uchar*&, uint, uint) override;
	void computed_results() override;
public:
	UmbralFilter();
	~UmbralFilter();
	const wchar_t* get_name() override;
	void reset();
	void modify(int, int);
};

