#pragma once
#include <string>
#include <vector>
#include <Wt/WPanel.h>

using namespace std;

class WTFUNC
{
	string default_deploy_path = "/";
	string default_http_addr = "0.0.0.0";
	string default_http_port = "8080";

public:
	explicit WTFUNC() {}
	~WTFUNC() {}
	void init_app_box(Wt::WPanel*);
	vector<string> make_wrun_args(string);

};

