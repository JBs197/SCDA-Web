#include "SCDAwindow.h"
#include "SCDAserver.h"
#include "SCDAwidget.h"

using namespace std;
mutex m_err, m_server;
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

unique_ptr<Wt::WApplication> makeApp(const Wt::WEnvironment& env, SCDAserver& myServer)
{
	return make_unique<SCDAapp>(env, myServer);
}

vector<string> make_wrun_args(string exec_dir)
{
	// Temporary bootloader function. Will be made more flexible later.

	size_t pos1 = exec_dir.rfind('\\');  // Debug or release.
	pos1 = exec_dir.rfind('\\', pos1 - 1);  // Project folder.
	string proj_dir = exec_dir.substr(0, pos1);

	string default_deploy_path = "/";
	string default_http_addr = "0.0.0.0";
	string default_http_port = "8080";
	vector<string> args(5);
	args[0] = "--http-address=" + default_http_addr;
	args[1] = "--http-port=" + default_http_port;
	args[2] = "--deploy-path=" + default_deploy_path;
	args[3] = "--approot=" + proj_dir;
	args[4] = "--docroot=" + proj_dir + "\\html";

	return args;
}

int main()
{
	WINFUNC wf;
	const string exec_dir = wf.get_exec_dir();
	const vector<string> args = make_wrun_args(exec_dir);

	
	Wt::WServer wtServer(exec_dir, args);
	SCDAserver myServer(wtServer);
	wtServer.addEntryPoint(Wt::EntryPointType::Application, bind(makeApp, placeholders::_1, ref(myServer)));

	int signal;
	if (wtServer.start())
	{
		signal = Wt::WServer::waitForShutdown();
		cerr << "wtServer is shutting down: " << signal << endl;
		wtServer.stop();
	}

	return 0;
}