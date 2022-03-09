#pragma once
#include "jstring.h"

// This struct contains the server's response to the client's data request.
struct CataReturn : public JSTRING
{
	std::vector<std::string> vsMapGeo;
	std::vector<std::vector<std::string>> vvsGeo, vvsDIM;
	std::vector<std::vector<std::vector<std::string>>> vvvsMap, vvvsMapFrame;
};
