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
	this->enableUpdates(1);
	this->require("//mozilla.github.io/pdf.js/build/pdf.js");

	setTitle("SCDA Web Tool");
	auto BootstrapTheme = make_unique<Wt::WBootstrapTheme>();
	BootstrapTheme->setVersion(Wt::BootstrapVersion::v3);
	this->setTheme(move(BootstrapTheme));
	
	string docRoot = Wt::WApplication::docRoot();
	serv.cleanTempFolder(docRoot);

	vector<unsigned char> binCSSplain, binCSSshaded, binIconChevronDown;
	vector<unsigned char> binIconChevronRight, binIconClose, binIconTrash;
	serverRef.jfile.load(binCSSplain, docRoot + "/pdfTextPlain.css");
	serverRef.jfile.load(binCSSshaded, docRoot + "/pdfTextShaded.css");
	serverRef.jfile.load(binIconChevronDown, docRoot + "/ChevronDown_Icon_16px.png");
	serverRef.jfile.load(binIconChevronRight, docRoot + "/ChevronRight_Icon_16px.png");
	serverRef.jfile.load(binIconClose, docRoot + "/Close_Icon_16px.png");
	serverRef.jfile.load(binIconTrash, docRoot + "/DragIntoTrash_Icon_42px.png");

	SCDAwidget* scdaWidget = root()->addWidget(make_unique<SCDAwidget>(serverRef));
	scdaWidget->cssTextPlain = scdaWidget->loadCSS(binCSSplain);
	scdaWidget->cssTextShaded = scdaWidget->loadCSS(binCSSshaded);
	scdaWidget->iconChevronDown = scdaWidget->loadIcon(binIconChevronDown);
	scdaWidget->iconChevronRight = scdaWidget->loadIcon(binIconChevronRight);
	scdaWidget->iconClose = scdaWidget->loadIcon(binIconClose);
	scdaWidget->iconTrash = scdaWidget->loadIcon(binIconTrash);
	this->globalKeyWentUp().connect(scdaWidget, &SCDAwidget::displayCata);

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