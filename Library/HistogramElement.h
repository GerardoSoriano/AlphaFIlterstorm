#pragma once
class HistogramElement {
public:
	int value; //El valor que representa
	int frequency; //La frecuencua en la que aparece este valor en el histograma
	int cdf; //El acumulativo CDF

	HistogramElement();
	~HistogramElement();
	//total_pixels es el total de los pixeles del histograma. Por ejemplo, en una imagen de 4x4, total_pixels debería ser 16
	float getSdk(int totalPixels) {
		return (float)(this->cdf) / (float)totalPixels;
	}
};
