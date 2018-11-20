#include "Filter.h"



Filter::Filter() : base(nullptr), result(nullptr), substractor(1) {};


Filter::~Filter()
{
	delete base, result;
}
const wchar_t * Filter::get_name()
{
	return nullptr;
}
;

void Filter::apply()
{
	if (base != nullptr)
	{
		for (uint y = substractor; y < (base->rows - substractor); y++)
		{
			uchar *input = nullptr;
			uchar *output = result->image.ptr<uchar>(y);
			for (uint x = substractor; x < (base->cols - substractor); x++)
			{
				bucle(input, output, x, y);
			}
		}
	}
}

void Filter::set_image(string _path)
{
	this->base = new Picture(_path);
	this->result = new Picture(_path);
}

void Filter::set_image(Mat _img)
{
	this->base = new Picture(_img);
	this->result = new Picture(_img);
}

Picture * Filter::get_base_image()
{
	return this->base;
}

Picture * Filter::get_result_image()
{
	return this->result;
}
