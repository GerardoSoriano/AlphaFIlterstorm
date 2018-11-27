#pragma once
class HistogramElement {
public:
	float value;
	float frequency;
	float cdf;
	HistogramElement();
	~HistogramElement();
	bool operator < (const HistogramElement& str) const
	{
		return (frequency < str.frequency);
	};
};
