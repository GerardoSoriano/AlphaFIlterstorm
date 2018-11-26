#include "HistogramElement.h"



HistogramElement::HistogramElement(): value(0), frequency(0), cdf(0)
{
}

HistogramElement::~HistogramElement() = default;

float HistogramElement::get_sdk(const int _total_pixels) const
{
	return static_cast<float>(this->cdf) / static_cast<float>(_total_pixels);
}
