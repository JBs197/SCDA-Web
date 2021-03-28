#include "SCDAwindow.h"
#include "SCDAserver.h"

using namespace std;
mutex m_err;
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

	SCDAwidget* scdaWidget;
}

int main()
{
	WINFUNC wf;
	const string exec_dir = wf.get_exec_dir();
	WTFUNC wtf;
	const vector<string> args = wtf.make_wrun_args(exec_dir);

	Wt::WServer wtServer(exec_dir, args);
	SCDAserver myServer(wtServer);
	wtServer.addEntryPoint(Wt::EntryPointType::Application, )

	Wt::WRun(exec_dir, args, [](const Wt::WEnvironment& env) {
		return make_unique<SCDAwindow>(env);
	});

	return 0;
}