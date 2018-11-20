#pragma once
#include "Filter.h"
class GrayscaleLuminanceFilter :
	public Filter
{
private:
	int msize;
	int** mask;
	void bucle(uchar*&, uchar*&, uint, uint) override;
	void make_mask();
	static int min_element(int[], int);
	static void swap(int&, int&) noexcept;
	static void sort(int*, int);
public:
	GrayscaleLuminanceFilter();
	~GrayscaleLuminanceFilter();
	const wchar_t* get_name() override;
	void reset();
	void modify(int);
};

