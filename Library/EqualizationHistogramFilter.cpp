#include "EqualizationHistogramFilter.h"


void EqualizationHistogramFilter::bucle(uchar*& _input, uchar*& _output, uint _x, uint _y)
{
	float tone = _output[_x * 3];

	for (auto& i : results)
		if(i.origin == tone)
		{
			tone = i.result;
			break;
		}

	_output[_x * 3] = static_cast<int>(tone);
	_output[_x * 3 + 1] = static_cast<int>(tone);
	_output[_x * 3 + 2] = static_cast<int>(tone);
}

void EqualizationHistogramFilter::computed_results()
{

	for (auto i = 0; i < result->hist.data.size(); i++)
	{
		results.emplace_back();
		const auto he = result->hist.data[i];
		const auto res = (he.cdf - result->hist.data[0].cdf) / (result->hist.total_pixels - result->hist.data[0].cdf) * 255;
		results[i].origin = he.value;
		results[i].result = res;
	}
}

EqualizationHistogramFilter::EqualizationHistogramFilter(): Filter()
{
	need_hist = true;
	substractor = 0;
}

EqualizationHistogramFilter::~EqualizationHistogramFilter() = default;

const wchar_t* EqualizationHistogramFilter::get_name()
{
	return TEXT("Filtro del histograma ecualizado");
}
