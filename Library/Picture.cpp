#include "Picture.h"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/video.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

// Función para dividir el array y hacer los intercambios
int divide(int *array, int start, int end) {
	int x = array[start];
	int i = start;
	int j = end;

	while (true) {
		do {
			i++;
		} while (i < end && array[i] < x);
		do {
			j--;
		} while (j > start && array[j] > x);

		if (i < j) {
			int tmp = array[i];
			array[i++] = array[j];
			array[j--] = tmp;
		}
		else {
			return j;
		}
	}
}

// Función recursiva para hacer el ordenamiento
void quicksort(int *array, int start, int end)
{
	while (start < end)
	{
		int pivot = divide(array, start, end);
		if(pivot - start <= end - (pivot + 1))
		{
			quicksort(array, start, pivot);
			start = pivot + 1;
		}else
		{
			quicksort(array, pivot + 1, end);
			end = pivot;
		}

	}
	//if (start < end) {
	//	pivot = divide(array, start, end);

	//	// Ordeno la lista de los menores
	//	quicksort(array, start, pivot - 1);

	//	// Ordeno la lista de los mayores
	//	quicksort(array, pivot + 1, end);
	//}
}

Picture::Picture(Mat _image) : image(_image.clone()), rows(image.rows), cols(image.cols)
{
	fill_array_colors();
};

Picture::Picture(string path) : image(imread(path)), rows(image.rows), cols(image.cols)
{
	fill_array_colors();
}

void Picture::fill_array_colors()
{
	/*b_colors = new int[rows*cols];
	g_colors = new int[rows*cols];
	r_colors = new int[rows*cols];
	float *arr = new float[10];
	arr[0] = 3;
	arr[1] = 10;
	arr[2] = 1;
	arr[3] = 7;
	arr[4] = 8;
	arr[5] = 15;
	arr[6] = 15;
	arr[7] = 9;
	arr[8] = 8;
	arr[9] = 5;
	int idx = 0;
	for (uint y = 0; y < rows; y++)
	{
		uchar *p = image.ptr<uchar>(y);
		for (uint x = 0; x < cols; x++)
		{
			b_colors[idx] = p[x * 3];
			g_colors[idx] = p[x * 3 + 1];
			r_colors[idx] = p[x * 3 + 2];
			idx++;
		}
	}
	quicksort(b_colors, 0, idx-1);
	for (int i = 0; i < idx; i++)
		cout << b_colors[i] << endl;

	system("pause");*/
};

Mat Picture::getHistogram(uint histSizeX, uint histSizeY, uint channel)
{
	std::vector<Mat>	bgr;
	int					histSize = 256;
	float				range[] = { 0, 256 };
	const float*		histRange = { range };
	Mat					hist_b, hist_g, hist_r;

	split(image, bgr);
	calcHist(&bgr[0], 1, 0, Mat(), hist_b, 1, &histSize, &histRange, true, false);
	calcHist(&bgr[1], 1, 0, Mat(), hist_g, 1, &histSize, &histRange, true, false);
	calcHist(&bgr[2], 1, 0, Mat(), hist_r, 1, &histSize, &histRange, true, false);

	int histWidth = cvRound((double)histSizeX / histSize);
	Mat histImage(histSizeY, histSizeX, CV_8UC3, Scalar(0, 0, 0));

	normalize(hist_b, hist_b, 0, histImage.rows, NORM_MINMAX, -1, Mat());
	normalize(hist_g, hist_g, 0, histImage.rows, NORM_MINMAX, -1, Mat());
	normalize(hist_r, hist_r, 0, histImage.rows, NORM_MINMAX, -1, Mat());

	for (int i = 1; i < histSize; i++)
	{
		switch (channel)
		{
		case CHANNEL_RGB:
			line(histImage, Point(histWidth*(i - 1), histSizeY - cvRound(hist_b.at<float>(i - 1))), Point(histWidth*(i), histSizeY - cvRound(hist_b.at<float>(i))), Scalar(255, 0, 0), 1, 8, 0);
			line(histImage, Point(histWidth*(i - 1), histSizeY - cvRound(hist_g.at<float>(i - 1))), Point(histWidth*(i), histSizeY - cvRound(hist_g.at<float>(i))), Scalar(0, 255, 0), 1, 8, 0);
			line(histImage, Point(histWidth*(i - 1), histSizeY - cvRound(hist_r.at<float>(i - 1))), Point(histWidth*(i), histSizeY - cvRound(hist_r.at<float>(i))), Scalar(0, 0, 255), 1, 8, 0);
			break;
		case CHANNEL_R:
			line(histImage, Point(histWidth*(i - 1), histSizeY - cvRound(hist_r.at<float>(i - 1))), Point(histWidth*(i), histSizeY - cvRound(hist_r.at<float>(i))), Scalar(0, 0, 255), 1, 8, 0);
			break;
		case CHANNEL_G:
			line(histImage, Point(histWidth*(i - 1), histSizeY - cvRound(hist_g.at<float>(i - 1))), Point(histWidth*(i), histSizeY - cvRound(hist_g.at<float>(i))), Scalar(0, 255, 0), 1, 8, 0);
			break;
		case CHANNEL_B:
			line(histImage, Point(histWidth*(i - 1), histSizeY - cvRound(hist_b.at<float>(i - 1))), Point(histWidth*(i), histSizeY - cvRound(hist_b.at<float>(i))), Scalar(255, 0, 0), 1, 8, 0);
			break;
		}
	}

	return histImage;
}

void Picture::resize(uint targetWidth)
{
	int		width = image.cols,
			height = image.rows;

	Mat		square = Mat::zeros(targetWidth, targetWidth, image.type());

	int		maxDim = (width >= height) ? width : height;
	float	scale = ((float)targetWidth) / maxDim;
	Rect	roi;

	if (width >= height)
	{
		roi.width = targetWidth;
		roi.x = 0;
		roi.height = height * scale;
		roi.y = (targetWidth - roi.height) / 2;
	}
	else
	{
		roi.y = 0;
		roi.height = targetWidth;
		roi.width = width * scale;
		roi.x = (targetWidth - roi.width) / 2;
	}

	cv::resize(image, square(roi), roi.size());

	rows = square.rows;
	cols = square.cols;
	image = square;
}

bool Picture::adaptControl(HWND hwnd, string name)
{
	string WIN_NAME_CV = name;

	namedWindow(WIN_NAME_CV, CV_WINDOW_KEEPRATIO);

	HWND cvWnd = (HWND)cvGetWindowHandle(WIN_NAME_CV.c_str());
	if (!cvWnd) return false;

	HWND hOldParent = GetParent(cvWnd);
	SetParent(cvWnd, hwnd);
	ShowWindow(hOldParent, SW_HIDE);

	//Ajustes varios
	RECT parentRect;
	GetClientRect(hwnd, &parentRect);
	cvResizeWindow(WIN_NAME_CV.c_str(), parentRect.right, parentRect.bottom);
	return true;
}
