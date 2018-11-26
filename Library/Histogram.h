#pragma once
#include <vector>
#include <opencv2/core/mat.hpp>
#include "HistogramElement.h"
using namespace std;
using namespace cv;

class Histogram {
public:
	int total_pixels;
	vector <HistogramElement> data;
	vector <HistogramElement> data_aux;
	Histogram();
	~Histogram();
	void create_histogram(Mat, int);

	//Devuelve un numero que tiende a 1 cuanto m�s se parescan los histogramas
	//Devuelve un numero que tiende a 0 cuanto m�s se diferencien los histogramas.
	float histogramPorcentage(Histogram b) {
		float result = 0;
		/*for (int i = 0; i < data.size(); i++) {
		result = data[i].getSdk(this->total_pixels);
		}
		return result;*/
		vector<float> porcentajes;
		for (int i = 0; i < 255; i++) {
			//Esta es la misma tecnica que la de la normalizacion de vectores.
			float numerator = data_aux[i].frequency - b.data_aux[i].frequency;
			float denominator = data_aux[i].frequency + b.data_aux[i].frequency;
			float res = 0;
			if (abs(numerator - denominator) <= 1.0) {
				res = 0;
			}
			else {
				res = abs(numerator / denominator);
			}

			//porcentajes.push_back(res);
			result = result + res;
		}

		//Entonces si el histograma es id�ntico, result va a valer 0, y cuanto m�s diferente sea, m�s tender� a 1
		//Para cada uno de los elementos del for, es decir que si es "completamente diferente" result deber�a ser 255
		//As� que normalizamos ese valor
		result = result / 255.0;

		//Y lo invertimos.

		result = 1 - result;
		return result;

		/*for (int i = 0; i < 255; i++) {

		}*/


	}

	HistogramElement getHistogramElementWithValue(int value) {
		for (int i = 0; i < data.size(); i++) {
			if (data[i].value == value) {
				return data[i];
			}
		}
	}

};
