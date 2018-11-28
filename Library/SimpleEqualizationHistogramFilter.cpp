#include "SimpleEqualizationHistogramFilter.h"


void SimpleEqualizationHistogramFilter::bucle(uchar*& _input, uchar*& _output, uint _x, uint _y)
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

void SimpleEqualizationHistogramFilter::computed_results()
{
	for (auto i = 0; i < result->hist.data.size(); i++)
	{
		results.emplace_back();
		const auto he = result->hist.data[i];
		const auto res = (he.cdf / result->hist.data.back().cdf) * 255;
		results[i].origin = he.value;
		results[i].result = res;
	}
}

SimpleEqualizationHistogramFilter::SimpleEqualizationHistogramFilter(): Filter()
{
	need_hist = true;
	substractor = 0;
}


SimpleEqualizationHistogramFilter::~SimpleEqualizationHistogramFilter() = default;

const wchar_t* SimpleEqualizationHistogramFilter::get_name()
{
	return TEXT("Filtro del histograma con ecualización simple");
}
