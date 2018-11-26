#include "Histogram.h"


Histogram::Histogram()
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

	std::sort(aux.begin(), aux.begin() + aux.size());

	int last_value = -1;
	int cdf = 0;
	for (int i : aux){
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

float Histogram::histogram_porcentage(Histogram _hist)
{
	float result = 0;
	vector<float> porcentajes;
	for (auto i = 0; i < 256; i++) {
		const float numerator = data_aux[i].frequency - _hist.data_aux[i].frequency;
		const float denominator = data_aux[i].frequency + _hist.data_aux[i].frequency;
		const auto res = (abs(numerator - denominator) <= 1.0) ? 0 : abs(numerator / denominator);
		result = result + res;
	}
	result = result / 255.0;
	result = 1 - result;
	return result;
}

HistogramElement Histogram::get_histogram_element_by_value(int _val)
{
	for (int i = 0; i < data.size(); i++) {
		if (data[i].value == _val) {
			return data[i];
		}
	}
	return {};
}