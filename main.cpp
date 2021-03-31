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

int main()
{
	WINFUNC wf;
	const string exec_dir = wf.get_exec_dir();
	WTFUNC wtf;
	const vector<string> args = wtf.make_wrun_args(exec_dir);

	
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
	

	/*
	Wt::WRun(exec_dir, args, [](const Wt::WEnvironment& env) {
		return make_unique<SCDAwindow>(env);
	});
	*/
	return 0;
}