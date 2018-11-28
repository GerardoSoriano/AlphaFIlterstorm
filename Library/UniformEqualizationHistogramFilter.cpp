#include "UniformEqualizationHistogramFilter.h"



void UniformEqualizationHistogramFilter::bucle(uchar*& _input, uchar*& _output, uint _x, uint _y)
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

void UniformEqualizationHistogramFilter::computed_results()
{
	for (auto i = 0; i < result->hist.data.size(); i++)
	{
		results.emplace_back();
		const auto he = result->hist.data[i];
		const auto res = (result->hist.data.back().value - result->hist.data[0].value) * (he.cdf / result->hist.data.back().cdf) + result->hist.data[0].value;
		results[i].origin = he.value;
		results[i].result = res;
	}
}

UniformEqualizationHistogramFilter::UniformEqualizationHistogramFilter(): Filter()
{
	need_hist = true;
	substractor = 0;
}

UniformEqualizationHistogramFilter::~UniformEqualizationHistogramFilter() = default;

const wchar_t * UniformEqualizationHistogramFilter::get_name()
{
	return TEXT("Filtro del histograma con ecualización uniforme");
}
