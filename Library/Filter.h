#pragma once
#include "Picture.h"

class Filter
{
protected:
	int substractor;
	Picture *base, *result;
	virtual void bucle(uchar*&, uchar*&, uint, uint) = 0;
public:
	Filter();
	virtual ~Filter();
	virtual const wchar_t* get_name();
	void apply();
	void set_image(string);
	void set_image(Mat);
	Picture* get_base_image();
	Picture* get_result_image();
};