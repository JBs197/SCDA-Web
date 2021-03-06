#include "SCDAwidget.h"

using namespace std;
mutex m_config, m_err, m_server;
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
	this->enableUpdates(1);

	setTitle("SCDA Web Tool");
	auto BootstrapTheme = make_unique<Wt::WBootstrapTheme>();
	BootstrapTheme->setVersion(Wt::BootstrapVersion::v3);
	this->setTheme(move(BootstrapTheme));
	
	vector<unsigned char> binIconChevronDown = serverRef.jf.loadBin(docRoot() + "/ChevronDown_Icon_16px.png");
	vector<unsigned char> binIconChevronRight = serverRef.jf.loadBin(docRoot() + "/ChevronRight_Icon_16px.png");
	vector<unsigned char> binIconClose = serverRef.jf.loadBin(docRoot() + "/Close_Icon_16px.png");
	vector<unsigned char> binIconTrash = serverRef.jf.loadBin(docRoot() + "/DragIntoTrash_Icon_42px.png");

	const string mrb = docRoot() + "\\SCDA-Wt";
	this->messageResourceBundle().use(mrb);
	//auto cssLink = Wt::WLink(docRoot() + "\\SCDA-Wt.css");
	//this->useStyleSheet(cssLink);

	//root()->addWidget(make_unique<Wt::WText>(Wt::WString::tr("introduction")));
	SCDAwidget* scdaWidget = root()->addWidget(make_unique<SCDAwidget>(serverRef));
	scdaWidget->iconChevronDown = scdaWidget->loadIcon(binIconChevronDown);
	scdaWidget->iconChevronRight = scdaWidget->loadIcon(binIconChevronRight);
	scdaWidget->iconClose = scdaWidget->loadIcon(binIconClose);
	scdaWidget->iconTrash = scdaWidget->loadIcon(binIconTrash);
	this->globalKeyWentUp().connect(scdaWidget, &SCDAwidget::displayCata);
}

string getDocPath(const string& execPath)
{
	size_t pos1 = execPath.rfind('\\');  // Deployment folder.
	pos1 = execPath.rfind('\\', pos1 - 1); // Debug or release.
	pos1 = execPath.rfind('\\', pos1 - 1);  // Project folder.
	string projDir = execPath.substr(0, pos1);
	pos1 = projDir.find('\\');
	while (pos1 < projDir.size())
	{
		projDir[pos1] = '/';
		pos1 = projDir.find('\\', pos1 + 1);
	}
	string docPath = projDir + "/html";
	return docPath;
}
unique_ptr<Wt::WApplication> makeApp(const Wt::WEnvironment& env, SCDAserver& myServer)
{
	return make_unique<SCDAapp>(env, myServer);
}
vector<string> make_wrun_args(string exec_dir)
{
	size_t pos1 = exec_dir.rfind('\\');  // Executable folder.
	pos1 = exec_dir.rfind('\\', pos1 - 1); // Debug or release.
	pos1 = exec_dir.rfind('\\', pos1 - 1);  // Project folder.
	string proj_dir = exec_dir.substr(0, pos1);
	pos1 = proj_dir.find('\\');
	while (pos1 < proj_dir.size())
	{
		proj_dir[pos1] = '/';
		pos1 = proj_dir.find('\\', pos1 + 1);
	}
	string default_http_addr = "192.168.0.25";
	string default_http_port = "8181";
	vector<string> args(5);
	args[0] = "--http-address=" + default_http_addr;
	args[1] = "--http-port=" + default_http_port;
	args[2] = "--approot=" + proj_dir;
	args[3] = "--docroot=" + proj_dir + "/html";
	args[4] = "-c" + proj_dir + "/html/wt_config.xml";
	return args;
}

int main()
{
	JFUNC jf;
	WINFUNC wf;
	int signal;
	const string execPath = wf.get_exec_path(), wtAppPath = "";
	string dbPath = sroot + "\\SCDA.db";
	const vector<string> args = make_wrun_args(execPath);
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