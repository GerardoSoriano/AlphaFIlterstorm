#include "Histogram.h"



Histogram::Histogram()
{
}

Histogram::~Histogram()
{
}

void Histogram::create_histogram(Mat _source, int _channel)
{
	total_pixels = _source.rows * _source.cols;

	for (int i = 0; i < 256; i++)
		data_aux.emplace_back();

	vector<int> aux;
	for (int i = 0; i < _source.rows; i++) {
		const auto p = _source.ptr<uchar>(i);
		for (auto j = 0; j < _source.cols * 3; j += 3) {
			aux.push_back(p[j + _channel]);
		}
	}

	std::sort(aux.begin(), aux.begin() + aux.size());

	int lastValue = -1;
	int cDFAux = 0;
	for (int i : aux){
		cDFAux++;
		if (i > lastValue) {
			lastValue = i; 
			HistogramElement newElement;
			newElement.value = i;
			newElement.frequency = 1;
			newElement.cdf = cDFAux;
			data.push_back(newElement);
			data_aux[i] = newElement;
		}
		else {
			data[data.size() - 1].frequency++;
			data[data.size() - 1].cdf++;
			data_aux[lastValue] = data[data.size() - 1];
		}
	}
}
