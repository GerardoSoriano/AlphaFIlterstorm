#pragma once
#include "Filter.h"
class SobelFilter :
	public Filter
{
private:
	int maskX[3][3], maskY[3][3];
	void bucle(uchar*&, uchar*&, uint, uint) override;
public:
	SobelFilter();
	~SobelFilter();
	void reset() const;
	void modify() const;
};
