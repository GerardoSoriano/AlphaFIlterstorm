#pragma once
class HistogramElement {
public:
	int value;
	int frequency;
	int cdf;
	HistogramElement();
	~HistogramElement();
	float get_sdk(int) const;
	bool operator < (const HistogramElement& str) const
	{
		return (frequency < str.frequency);
	};
};
