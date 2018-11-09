#include <opencv2/opencv.hpp>
#include <iostream>
#include "Picture.h"
#include "FilterFactory.h"
#include "MedianFilter.h"
#include "WeightedMedianFilter.h"
#include "MinusMedianFilter.h"
#include "AverageFilter.h"
using namespace std;
using namespace cv;

int main()
{
	Picture *pic = new Picture(R"(C:\Users\gerar\OneDrive\Im�genes\kidface.jpg)");
	if (pic->image.empty())
	{
		cout << "No se pudo abrir la imagen chavo" << endl;
		return -1;
	}
	FilterFactory *ff = new FilterFactory(_Average);
	Filter *f = ff->createFilter();
	f->set_image(pic->image);
	f->apply();
	Picture *base = f->get_base_image();
	Picture *result = f->get_result_image();

	cout << "Tu imagen es de: " << pic->cols << " x " << pic->rows << endl;

	namedWindow("Test01");
	imshow("Test01", base->image);

	namedWindow("Test02");
	imshow("Test02", result->image);

	dynamic_cast<AverageFilter*>(f)->modify(10);
	f->apply();
	result = f->get_result_image();

	namedWindow("Test03");
	imshow("Test03", result->image);

	waitKey(0);
	return 1;
}