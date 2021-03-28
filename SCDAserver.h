#pragma once
#include <Wt/WServer.h>

using namespace std;

class SCDAserver
{
	Wt::WServer& serverRef;

public:
	SCDAserver(Wt::WServer&);
	SCDAserver(const SCDAserver&) = delete;
	SCDAserver& operator=(const SCDAserver&) = delete;

};

