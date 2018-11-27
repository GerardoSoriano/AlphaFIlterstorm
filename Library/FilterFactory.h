#pragma once
#include "Filter.h"

class FilterFactory
{
private:
	FilterList choise;
	FilterFactory();
public:
	~FilterFactory();
	static FilterFactory& get_instance();
	void change_choise(FilterList);
	Filter* createFilter() const;
};

