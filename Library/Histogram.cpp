#include "Histogram.h"


Histogram::Histogram(): total_pixels(0), lower_frecuency(0), higher_frecuency(0)
{
}

Histogram::~Histogram()= default;

void Histogram::create_histogram(Mat _source, int _channel)
{
	total_pixels = _source.rows * _source.cols;

	for (int i = 0; i < 256; i++)
		data_aux.emplace_back();

	vector<int> aux;
	for (auto i = 0; i < _source.rows; i++) {
		const auto p = _source.ptr<uchar>(i);
		for (auto j = 0; j < _source.cols * 3; j += 3) {
			aux.push_back(p[j + _channel]);
		}
	}
	sort(aux.begin(), aux.end());

	auto last_value = -1;
	auto cdf = 0;
	for (auto i : aux){
		cdf++;
		if (i > last_value) {
			last_value = i; 
			HistogramElement newElement;
			newElement.value = i;
			newElement.frequency = 1;
			newElement.cdf = cdf;
			data.push_back(newElement);
			data_aux[i] = newElement;
		}
		else {
			data[data.size() - 1].frequency++;
			data[data.size() - 1].cdf++;
			data_aux[last_value] = data[data.size() - 1];
		}
	}

	sort(data_aux.begin(), data_aux.end());

	lower_frecuency = data_aux[0].frequency;
	higher_frecuency = data_aux.back().frequency;
}

HistogramElement Histogram::get_histogram_element_by_value(int _val)
{
	for (auto& i : data)
	{
		if (i.value == _val) {
			return i;
		}
	}
	return {};
}