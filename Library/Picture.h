#pragma once

#include <windows.h>
#include <opencv2/core/mat.hpp>
#include "Histogram.h"
using namespace cv;
using namespace std;

#define CHANNEL_RGB 0
#define CHANNEL_R 1
#define CHANNEL_G 2
#define CHANNEL_B 3

class Picture
{
public:
	Mat image;
	int rows, cols;
	Histogram hist;
	Picture();
	explicit Picture(Mat);
	explicit Picture(string);
	~Picture();
	void open();
	void close();
	void save();
	void restore();
	void make_histogram();
	void convert_to_gray();
	Mat getHistogram(uint histSizeX, uint histSizeY, uint channel);
	Mat GetSquareImage(HWND canvas);
	static bool adaptControl(HWND hwnd, string name);
};