#pragma once
#include <string>
#include <vector>
#include <Wt/WApplication.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WPanel.h>
#include <Wt/WLink.h>
#include <Wt/WText.h>

using namespace std;

class WTFUNC
{
	string default_deploy_path = "/";
	string default_http_addr = "0.0.0.0";
	string default_http_port = "8080";
	string proj_dir;

public:
	explicit WTFUNC() {}
	~WTFUNC() {}
	//vector<Wt::WString> get_wsvec(vector<string>&);
	void init_app_box(Wt::WContainerWidget*);
	void init_proj_dir(string);
	vector<string> make_wrun_args(string);

};

