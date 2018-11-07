#include "SobelFilter.h"



SobelFilter::SobelFilter()
{
	maskX[0][0] = -1; maskX[0][1] = 0; maskX[0][2] = 1;
	maskX[1][0] = -2; maskX[1][1] = 0; maskX[1][2] = 2;
	maskX[2][0] = -1; maskX[2][1] = 0; maskX[2][2] = 1;

	maskY[0][0] = -1;	maskY[0][1] = -2;	maskY[0][2] = -1;
	maskY[1][0] = 0;	maskY[1][1] = 0;	maskY[1][2] = 0;
	maskY[2][0] = 1;	maskY[2][1] = 2;	maskY[2][2] = 1;
}

SobelFilter::~SobelFilter()
{
}

void SobelFilter::reset() const
{
}

void SobelFilter::modify() const
{
}

void SobelFilter::bucle(uchar*& _input, uchar*& _output, uint _x, uint _y)
{
	int sum = 0, sum_x = 0, sum_y = 0;
	float gray;

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			_input = base->image.ptr<uchar>((_y - 1) + i);

			const float b = _input[((_x - 1) + j) * 3];;
			const float g = _input[((_x - 1) + j) * 3 + 1];
			const float r = _input[((_x - 1) + j) * 3 + 2];

			gray = (b + g + r) / 3;

			sum_x = sum_x + maskX[j][i] * gray;
			sum_y = sum_y + maskY[j][i] * gray;
		}
	}
	sum = sqrt(pow(sum_x, 2) + pow(sum_y, 2));

	if (sum < 0)
		sum = 0;
	if (sum > 255)
		sum = 255;

	_output[_x * 3] = sum;
	_output[_x * 3 + 1] = sum;
	_output[_x * 3 + 2] = sum;
}
