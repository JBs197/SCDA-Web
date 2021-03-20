#include "wtfunc.h"

void WTFUNC::init_app_box(Wt::WPanel* app_box)
{
	app_box->setTitle(L"Statistics Canada Database Analysis Tool");
	app_box->
}
vector<string> WTFUNC::make_wrun_args(string exec_dir)
{
	// Temporary bootloader function. Will be made more flexible later.
	
	size_t pos1 = exec_dir.rfind('\\');  // Debug or release.
	pos1 = exec_dir.rfind('\\', pos1 - 1);  // Project folder.
	string proj_dir = exec_dir.substr(0, pos1);
	
	vector<string> args(5);
	args[0] = "--http-address=" + default_http_addr;
	args[1] = "--http-port=" + default_http_port;
	args[2] = "--deploy-path=" + default_deploy_path;
	args[3] = "--approot=" + proj_dir;
	args[4] = "--docroot=" + proj_dir + "\\html";
	
	return args;
}
