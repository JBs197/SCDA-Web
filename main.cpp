#define HPDF_DLL
#include "SCDAwidget.h"

using namespace std;
mutex m_config, m_err, m_server;
JLOG* JLOG::instance = 0;
int JNODE::nextID{ 0 };

class SCDAapp : public Wt::WApplication
{
	SCDAserver& serverRef;
	const Wt::WEnvironment& envRef;

public:
	SCDAapp(const Wt::WEnvironment&, SCDAserver&);

};
SCDAapp::SCDAapp(const Wt::WEnvironment& env, SCDAserver& serv) : WApplication(env), serverRef(serv), envRef(env)
{
	enableUpdates(1);
	setTitle("SCDA Web Tool");
	//auto BootstrapTheme = make_unique<Wt::WBootstrapTheme>();
	//BootstrapTheme->setVersion(Wt::BootstrapVersion::v3);
	//this->setTheme(move(BootstrapTheme));
	
	string docRoot = Wt::WApplication::docRoot();
	serv.cleanTempFolder(docRoot);

	vector<vector<string>> vvsTag = serverRef.jparse.getXML(serverRef.configXML, { "settings", "require" });
	for (int ii = 0; ii < vvsTag.size(); ii++) {
		require(vvsTag[ii][1]);  // Javascript libraries, such as PDF.
	}

	string cssPath = serverRef.jparse.getXML1(serverRef.configXML, { "path", "css_main" });
	useStyleSheet(cssPath.c_str());

	SCDAwidget* scdaWidget = root()->addWidget(make_unique<SCDAwidget>(serverRef));
	scdaWidget->setStyleClass("SCDAwidget");


	int binSize, index;
	vector<unsigned char> binResource;
	vvsTag = serverRef.jparse.getXML(serverRef.configXML, { "path", "css" });
	for (int ii = 0; ii < vvsTag.size(); ii++) {
		serverRef.jfile.load(binResource, vvsTag[ii][1]);
		index = (int)scdaWidget->mapResource.size();
		scdaWidget->vResource.emplace_back(make_shared<Wt::WMemoryResource>("binCSS"));
		scdaWidget->mapResource.emplace(vvsTag[ii][0], index);
		binSize = (int)binResource.size();
		scdaWidget->vResource.back()->setData(&binResource[0], binSize);
	}
	vvsTag = serverRef.jparse.getXML(serverRef.configXML, { "path", "icon" });
	for (int ii = 0; ii < vvsTag.size(); ii++) {
		serverRef.jfile.load(binResource, vvsTag[ii][1]);
		index = (int)scdaWidget->mapResource.size();
		scdaWidget->vResource.emplace_back(make_shared<Wt::WMemoryResource>("binIcon"));
		scdaWidget->mapResource.emplace(vvsTag[ii][0], index);
		binSize = (int)binResource.size();
		scdaWidget->vResource.back()->setData(&binResource[0], binSize);
	}


	this->setLoadingIndicator(make_unique<Wt::WOverlayLoadingIndicator>());
}

void getMiscPath(vector<string>& vsMisc, JPARSE& jparse, string& xmlConfig)
{
	// Output form [log, database].
	vsMisc.resize(2);
	vsMisc[0] = jparse.getXML1(xmlConfig, { "directory", "log" });
	vsMisc[1] = jparse.getXML1(xmlConfig, { "path", "database" });
}
unique_ptr<Wt::WApplication> makeApp(const Wt::WEnvironment& env, SCDAserver& myServer)
{
	return make_unique<SCDAapp>(env, myServer);
}
const vector<string> makeServerArgs(JPARSE& jparse, string& xmlConfig)
{
	vector<string> vsArg;
	vector<vector<string>> vvsTag = jparse.getXML(xmlConfig, { "server_args" });
	int numArg = (int)vvsTag.size();
	vsArg.resize(numArg);
	for (int ii = 0; ii < numArg; ii++) {
		vsArg[ii] = std::move(vvsTag[ii][1]);
	}
	return vsArg;
}

int main()
{
	JPARSE jparse;
	WINFUNC wf;
	int signal;
	const string execDir = wf.getExecPath(1), wtAppPath = "";
	string xmlConfig;
	wf.load(xmlConfig, execDir + "/SCDA_Web_Config.xml");
	vector<string> vsMisc;
	getMiscPath(vsMisc, jparse, xmlConfig);
	JLOG::getInstance()->init(vsMisc[0], "SCDA-Web");
	const vector<string> vsArg = makeServerArgs(jparse, xmlConfig);

	Wt::WServer wtServer(execDir + "/SCDA-Wt.exe", vsArg, wtAppPath);
	SCDAserver myServer(wtServer, vsMisc[1]);
	wtServer.addEntryPoint(Wt::EntryPointType::Application, bind(makeApp, placeholders::_1, ref(myServer)));
	myServer.configXML = std::move(xmlConfig);
	if (wtServer.start()) {
		signal = Wt::WServer::waitForShutdown();
		cerr << "wtServer is shutting down: " << signal << endl;
		wtServer.stop();
	}
	return 0;
}