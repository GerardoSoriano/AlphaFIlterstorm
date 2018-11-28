#include "NormalizeHistogramFilter.h"


void NormalizeHistogramFilter::bucle(uchar*& _input, uchar*& _output, uint _x, uint _y)
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

void NormalizeHistogramFilter::computed_results()
{
	for (auto i = 0; i < result->hist.data.size(); i++)
	{
		results.emplace_back();
		const auto he = result->hist.data[i];
		const auto res = (he.frequency / result->hist.higher_frecuency) * 255;
		results[i].origin = he.value;
		results[i].result = res;
	}
}

NormalizeHistogramFilter::NormalizeHistogramFilter(): Filter()
{
	need_hist = true;
	substractor = 0;
}


NormalizeHistogramFilter::~NormalizeHistogramFilter() = default;

const wchar_t* NormalizeHistogramFilter::get_name()
{
	return TEXT("Filtro del histograma normalizado");
}
