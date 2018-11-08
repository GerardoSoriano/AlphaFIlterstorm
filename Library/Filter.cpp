#include "Filter.h"



Filter::Filter() : base(nullptr), result(nullptr) {};


Filter::~Filter()
{
	delete base, result;
};

void Filter::apply()
{
	if(base != nullptr)
	{
		for (uint y = 1; y < (base->rows - 1); y++)
		{
			uchar *input = nullptr;
			uchar *output = result->image.ptr<uchar>(y);
			for (uint x = 1; x < (base->cols - 1); x++)
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
