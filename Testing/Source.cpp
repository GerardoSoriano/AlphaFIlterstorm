#include <opencv2/opencv.hpp>
#include <iostream>
#include "Picture.h"
#include "FilterFactory.h"
#include "MedianFilter.h"
#include "WeightedMedianFilter.h"
#include "MinusMedianFilter.h"
#include "AverageFilter.h"
#include "LaplacianFilter.h"
#include "MinusLaplacianFilter.h"
#include "DirectionalNorthFilter.h"
#include "DirectionalEastFilter.h"
#include "GrayscaleLuminanceFilter.h"
#include "SobelFilter.h"
#include "GaussianFilter.h"
using namespace std;
using namespace cv;

int main()
{
	Picture *pic = new Picture(R"(C:\Users\gerar\OneDrive\Imágenes\kidface.jpg)");
	if (pic->image.empty())
	{
		cout << "No se pudo abrir la imagen chavo" << endl;
		return -1;
	}
	FilterFactory ff = FilterFactory::get_instance();
	ff.change_choise(_NormalizeHistogram);
	Filter *f = ff.createFilter();
	f->set_image(pic->image);
	f->apply();
	Picture *base = f->get_base_image();
	Picture *result = f->get_result_image();
	cout << "Tu imagen es de: " << pic->cols << " x " << pic->rows << endl;

	namedWindow("Test01");
	imshow("Test01", base->image);

	namedWindow("HistTest1");
	imshow("HistTest1", base->getHistogram(300, 300, CHANNEL_RGB));
		
	namedWindow("Test02");
	imshow("Test02", result->image);

	namedWindow("HistTest2");
	imshow("HistTest2", result->getHistogram(300, 300, CHANNEL_RGB));

	/*dynamic_cast<GaussianFilter*>(f)->modify(3, 1.5);
	f->apply();
	result = f->get_result_image();
	
	namedWindow("Test03");
	imshow("Test03", result->image);*/

	waitKey(0);
	return 1;
}