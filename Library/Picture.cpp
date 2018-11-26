#include "Picture.h"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/video.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
using namespace std;

Picture::Picture()
{
}

Picture::Picture(Mat _image) : image(_image.clone()), rows(image.rows), cols(image.cols)
{
	hist_b = Histogram();
	hist_b.create_histogram(image, CHANNEL_B);
	hist_g = Histogram();
	hist_g.create_histogram(image, CHANNEL_G);
	hist_r = Histogram();
	hist_r.create_histogram(image, CHANNEL_R);
};

Picture::Picture(string _path) : image(imread(_path)), rows(image.rows), cols(image.cols)
{
	hist_b = Histogram();
	hist_b.create_histogram(image, CHANNEL_B);
	hist_g = Histogram();
	hist_g.create_histogram(image, CHANNEL_G);
	hist_r = Histogram();
	hist_r.create_histogram(image, CHANNEL_R);
}

Picture::~Picture()
{
}

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

Mat Picture::GetSquareImage(HWND canvas)
{
	int width = image.cols,
		height = image.rows;

	RECT parentRect;
	GetClientRect(canvas, &parentRect);
	int target_width = parentRect.right;

	Mat square = Mat::zeros(target_width, target_width, image.type());

	int max_dim = (width >= height) ? width : height;
	float scale = ((float)target_width) / max_dim;
	Rect roi;
	if (width >= height)
	{
		roi.width = target_width;
		roi.x = 0;
		roi.height = height * scale;
		roi.y = (target_width - roi.height) / 2;
	}
	else
	{
		roi.y = 0;
		roi.height = target_width;
		roi.width = width * scale;
		roi.x = (target_width - roi.width) / 2;
	}

	resize(image, square(roi), roi.size());

	return square;
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
