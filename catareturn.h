#pragma once
#include "jstring.h"

// This struct contains the server's response to the client's data request.
struct CataReturn : public JSTRING
{
	CataReturn(std::string& year, std::string& cata) : sYear(year), sCata(cata) {}
	CataReturn() {}
	~CataReturn() = default;

	std::string sCata, sYear;
	std::vector<std::string> vsMapGeo;
	std::vector<std::vector<std::string>> vvsGeo, vvsDIM;
	std::vector<std::vector<std::vector<std::string>>> vvvsMap, vvvsMapFrame;
};
