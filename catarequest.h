#pragma once
#include "jstring.h"

// This struct contains a client's data requests for a particular catalogue.
struct CataRequest : public JSTRING
{
	int parentGeoCode{ -1 };
	std::string sCata, sYear;
	std::vector<std::string> vsDIM, vsGeoCode;
};
