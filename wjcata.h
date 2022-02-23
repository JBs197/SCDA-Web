#pragma once
#include <string>
#include <vector>

struct WJCATA
{
	WJCATA() {}
	~WJCATA() = default;

	std::string category, colTopic, desc, name, rowTopic, year;
	std::vector<std::string> vParameter;
};
