#pragma once
#include "Picture.h"

class Filter
{
protected:
	int substractor;
	Picture *base, *result;
	virtual void bucle(uchar*&, uchar*&, uint, uint) = 0;
public:
	Filter();
	virtual ~Filter();
	virtual const wchar_t* get_name();
	void apply();
	void set_image(string);
	void set_image(Mat);
	Picture* get_base_image();
	Picture* get_result_image();
};

//class HistogramElement {
//public:
//	int value; //El valor que representa
//	int frequency; //La frecuencua en la que aparece este valor en el histograma
//	int cdf; //El acumulativo CDF
//
//			 //totalPixels es el total de los pixeles del histograma. Por ejemplo, en una imagen de 4x4, totalPixels debería ser 16
//	float getSdk(int totalPixels) {
//		return (float)(this->cdf) / (float)totalPixels;
//	}
//};
//
//class Histogram {
//public:
//	vector <HistogramElement> data;
//	vector <HistogramElement> dataWithIndexValue;
//
//	int totalPixels;
//
//	Histogram() {
//	}
//
//	//switchRGB puede ser:
//	// 2 = rojo
//	// 1 = verde
//	// 0 = azul
//	Histogram(Mat source, int switchRGB) {
//		totalPixels = source.rows * source.cols;
//		setHistogramWithImage(source, switchRGB);
//	}
//
//	void setHistogramWithImage(Mat source, int switchRGB) {
//
//		totalPixels = source.rows * source.cols;
//
//		for (int i = 0; i < 255; i++) {
//			dataWithIndexValue.push_back(HistogramElement());
//		}
//
//		uchar *p, *q;
//		vector<int> aux;
//		for (int i = 0; i < source.rows; i++) {
//			p = source.ptr<uchar>(i);
//			for (int j = 0; j < source.cols * 3; j += 3) {
//				aux.push_back(p[j + switchRGB]);
//			}
//		}
//
//		std::sort(aux.begin(), aux.begin() + aux.size());
//
//		int lastValue = -1;
//		int cDFAux = 0;
//		//int frequencyAux = 1;
//		for (int i = 0; i < aux.size(); i++) {
//			cDFAux++;
//			if (aux[i] > lastValue) {
//				//frequencyAux = 1;
//				lastValue = aux[i]; //¿¡¡¡¡PORQUE DEMONIOS ESTO NO ESTABA!!!!?
//									//Si la condicion se cumple, entonces este es un nuevo valor en el histograma
//				HistogramElement newElement;
//				newElement.value = aux[i];
//				newElement.frequency = 1;
//				newElement.cdf = cDFAux;
//				data.push_back(newElement);
//				dataWithIndexValue[aux[i]] = newElement;
//			}
//			else {
//				//Entonces TIENE QUE ser igual al valor
//				//frequencyAux++;
//				//HistogramElement newElement;
//				//newElement.value = aux[i];
//				//newElement.frequency = frequencyAux;
//				//newElement.cdf = cDFAux;
//				//data.push_back(newElement);
//
//				/*HistogramElement newElement;
//				newElement.value = aux[i];
//				newElement.frequency = 1;
//				newElement.cdf = cDFAux;
//				data.push_back(newElement);*/
//
//				//data.push_back(HistogramElement());
//
//				//dataWithIndexValue[aux[i]] = newElement;
//
//
//				data[data.size() - 1].frequency++;
//				data[data.size() - 1].cdf++;
//				dataWithIndexValue[lastValue] = data[data.size() - 1];
//
//
//
//				//dataWithIndexValue[lastValue].frequency++;
//				//dataWithIndexValue[lastValue].cdf++;
//				//data
//				//cDFAux++;
//
//			}
//
//
//
//		}
//	}
//
//	//Devuelve un numero que tiende a 1 cuanto más se parescan los histogramas
//	//Devuelve un numero que tiende a 0 cuanto más se diferencien los histogramas.
//	float histogramPorcentage(Histogram b) {
//		float result = 0;
//		/*for (int i = 0; i < data.size(); i++) {
//		result = data[i].getSdk(this->totalPixels);
//		}
//		return result;*/
//		vector<float> porcentajes;
//		for (int i = 0; i < 255; i++) {
//			//Esta es la misma tecnica que la de la normalizacion de vectores.
//			float numerator = dataWithIndexValue[i].frequency - b.dataWithIndexValue[i].frequency;
//			float denominator = dataWithIndexValue[i].frequency + b.dataWithIndexValue[i].frequency;
//			float res = 0;
//			if (abs(numerator - denominator) <= 1.0) {
//				res = 0;
//			}
//			else {
//				res = abs(numerator / denominator);
//			}
//
//			//porcentajes.push_back(res);
//			result = result + res;
//		}
//
//		//Entonces si el histograma es idéntico, result va a valer 0, y cuanto más diferente sea, más tenderá a 1
//		//Para cada uno de los elementos del for, es decir que si es "completamente diferente" result debería ser 255
//		//Así que normalizamos ese valor
//		result = result / 255.0;
//
//		//Y lo invertimos.
//
//		result = 1 - result;
//		return result;
//
//		/*for (int i = 0; i < 255; i++) {
//
//		}*/
//
//
//	}
//
//	HistogramElement getHistogramElementWithValue(int value) {
//		for (int i = 0; i < data.size(); i++) {
//			if (data[i].value == value) {
//				return data[i];
//			}
//		}
//	}
//}