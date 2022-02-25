#pragma once
#include "jstring.h"

struct CataFilter : public JSTRING
{
	std::string category, colTopic, differentiator, rowTopic, year;
	std::vector<std::string> vParameter;
};
