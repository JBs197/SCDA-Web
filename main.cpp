#include "SCDAwidget.h"

using namespace std;
mutex m_err, m_server, m_map;
const string sroot = "F:";

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
	projDir += "\\html\\SCDA-Wt.xml";
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
	//string default_deploy_path = exec_dir.substr(0, pos1);
	pos1 = exec_dir.rfind('\\', pos1 - 1);  // Project folder.
	string proj_dir = exec_dir.substr(0, pos1);
	string default_http_addr = "192.168.1.67";
	string default_http_port = "8080";
	vector<string> args(4);
	args[0] = "--http-address=" + default_http_addr;
	args[1] = "--http-port=" + default_http_port;
	//args[2] = "--deploy-path=" + default_deploy_path;
	args[2] = "--approot=" + proj_dir;
	args[3] = "--docroot=" + proj_dir + "\\html";

	return args;
}

int main()
{
	JFUNC jf;
	WINFUNC wf;
	const string execPath = wf.get_exec_path();
	const vector<string> args = make_wrun_args(execPath);
	string wtConfigPath = getConfigPath(execPath);
	//const string exec_dir = "*";
	//const string wtConfigPath = "/etc/wt/wt_config.xml";
	const string wtConfigFile = jf.load(wtConfigPath);
	int signal;
	
	try 
	{
		Wt::WServer wtServer(execPath, args, wtConfigFile);
		SCDAserver myServer(wtServer);
		wtServer.addEntryPoint(Wt::EntryPointType::Application, bind(makeApp, placeholders::_1, ref(myServer)));
		if (wtServer.start())
		{
			signal = Wt::WServer::waitForShutdown();
			cerr << "wtServer is shutting down: " << signal << endl;
			wtServer.stop();
		}
	}
	catch (Wt::WServer::Exception& e) 
	{
		cout << "WServer error:" << endl;
		cerr << e.what() << endl; 
	}
	catch (exception &e) 
	{
		cout << "Std error:" << endl;
		cerr << e.what() << endl;
	}

	return 0;
}
