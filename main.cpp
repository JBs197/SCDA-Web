#include "SCDAwidget.h"

using namespace std;
mutex m_err, m_server, m_map;
const string sroot = "E:";

class SCDAapp : public Wt::WApplication
{
	SCDAserver& serverRef;
	const Wt::WEnvironment& envRef;
	
public:
	SCDAapp(const Wt::WEnvironment&, SCDAserver&);
};
SCDAapp::SCDAapp(const Wt::WEnvironment& env, SCDAserver& serv) : WApplication(env), serverRef(serv), envRef(env)
{
	setTitle("SCDA Web Tool");
	auto BootstrapTheme = make_unique<Wt::WBootstrapTheme>();
	BootstrapTheme->setVersion(Wt::BootstrapVersion::v3);
	this->setTheme(move(BootstrapTheme));
	// Background things ... ?

	const string mrb = docRoot() + "\\SCDA-Wt";
	this->messageResourceBundle().use(mrb);
	
	root()->addWidget(make_unique<Wt::WText>(Wt::WString::tr("introduction")));
	SCDAwidget* scdaWidget = root()->addWidget(make_unique<SCDAwidget>(serverRef));
	// Style input ... ?
}

string getConfigPath(const string& execPath)
{
	size_t pos1 = execPath.rfind('\\');  // Deployment folder.
	pos1 = execPath.rfind('\\', pos1 - 1); // Debug or release.
	pos1 = execPath.rfind('\\', pos1 - 1);  // Project folder.
	string projDir = execPath.substr(0, pos1);
	projDir += "\\html\\wt_config.xml";
	return projDir;
}
unique_ptr<Wt::WApplication> makeApp(const Wt::WEnvironment& env, SCDAserver& myServer)
{	
	return make_unique<SCDAapp>(env, myServer);
}
vector<string> make_wrun_args(string exec_dir)
{
	// Temporary bootloader function. Will be made more flexible later.

	size_t pos1 = exec_dir.rfind('\\');  // Executable folder.
	pos1 = exec_dir.rfind('\\', pos1 - 1); // Debug or release.
	pos1 = exec_dir.rfind('\\', pos1 - 1);  // Project folder.
	string proj_dir = exec_dir.substr(0, pos1);
	string default_http_addr = "0.0.0.0";
	string default_http_port = "8181";
	vector<string> args(4);
	args[0] = "--http-address=" + default_http_addr;
	args[1] = "--http-port=" + default_http_port;
	args[2] = "--approot=" + proj_dir;
	args[3] = "--docroot=" + proj_dir + "\\html";
	return args;
}


int main()
{
	JFUNC jf;
	WINFUNC wf;
	const string execPath = wf.get_exec_path(), wtAppPath = "";
	string dbPath = sroot + "\\SCDA.db";
	const vector<string> args = make_wrun_args(execPath);
	int signal;
	Wt::WServer wtServer(execPath, args, wtAppPath);
	SCDAserver myServer(wtServer, dbPath);
	wtServer.addEntryPoint(Wt::EntryPointType::Application, bind(makeApp, placeholders::_1, ref(myServer)));
	if (wtServer.start())
	{
		signal = Wt::WServer::waitForShutdown();
		cerr << "wtServer is shutting down: " << signal << endl;
		wtServer.stop();
	}

	return 0;
}
