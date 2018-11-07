#include "Picture.h"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/video.hpp>
#include <opencv2/highgui/highgui.hpp>

Picture::Picture(Mat _image) : image(_image.clone()), rows(image.rows), cols(image.cols) {};

Picture::Picture(string path) : image(imread(path)), rows(image.rows), cols(image.cols) {};

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
