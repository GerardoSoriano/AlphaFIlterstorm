#include "ExponentialEqualizationHistogramFilter.h"



void ExponentialEqualizationHistogramFilter::bucle(uchar*& _input, uchar*& _output, uint _x, uint _y)
{
	float tone = _output[_x * 3];

	for (auto& i : results)
		if (i.origin == tone)
		{
			tone = i.result;
			break;
		}

	_output[_x * 3] = static_cast<int>(tone);
	_output[_x * 3 + 1] = static_cast<int>(tone);
	_output[_x * 3 + 2] = static_cast<int>(tone);
}

void ExponentialEqualizationHistogramFilter::computed_results()
{
	for (auto i = 0; i < result->hist.data.size(); i++)
	{
		results.emplace_back();
		const auto he = result->hist.data[i];
		const auto res = result->hist.data[0].value - (1 / alpha) * log(1 - (he.cdf / result->hist.data.back().cdf));
		results[i].origin = he.value;
		results[i].result = res;
	}
}

ExponentialEqualizationHistogramFilter::ExponentialEqualizationHistogramFilter(): Filter(), alpha(0.01)
{
	need_hist = true;
	substractor = 0;
}


ExponentialEqualizationHistogramFilter::~ExponentialEqualizationHistogramFilter() = default;

const wchar_t * ExponentialEqualizationHistogramFilter::get_name()
{
	return TEXT("Filtro del histograma con ecualización exponencial");
}

void ExponentialEqualizationHistogramFilter::reset()
{
	modify(0.01);
}

void ExponentialEqualizationHistogramFilter::modify(int _alpha)
{
	delete result;
	alpha = _alpha;
	result = new Picture(base->image);
}
