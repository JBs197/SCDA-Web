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
	vector<unsigned char> binCSSplain = serverRef.jf.loadBin(docRoot + "/pdfTextPlain.css");
	vector<unsigned char> binCSSshaded = serverRef.jf.loadBin(docRoot + "/pdfTextShaded.css");
	vector<unsigned char> binIconChevronDown = serverRef.jf.loadBin(docRoot + "/ChevronDown_Icon_16px.png");
	vector<unsigned char> binIconChevronRight = serverRef.jf.loadBin(docRoot + "/ChevronRight_Icon_16px.png");
	vector<unsigned char> binIconClose = serverRef.jf.loadBin(docRoot + "/Close_Icon_16px.png");
	vector<unsigned char> binIconTrash = serverRef.jf.loadBin(docRoot + "/DragIntoTrash_Icon_42px.png");

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

void cleanTemp(JFUNC& jf, WINFUNC& wf, string& xmlFile) {
	string tempPath;
	vector<string> vsTag = { "directory", "temp" };
	vector<vector<string>> vvsTag = jf.getXML(xmlFile, vsTag);
	if (vvsTag.size() < 1) {
		cout << "ERROR: xml file contained no directory for temp files." << endl;
		cin >> tempPath;
		exit(EXIT_FAILURE);
	}
	vector<string> tempList = wf.getFileList(vvsTag[0][1], "*");
	for (int ii = 0; ii < tempList.size(); ii++) {
		tempPath = vvsTag[0][1] + "/" + tempList[ii];
		wf.deleteFile(tempPath);
	}
}
vector<string> getMiscPath(JFUNC& jf, string& xmlFile)
{
	// Output form [log, database].
	vector<string> vsMisc(2);
	vector<string> vsTag = { "directory", "log" };
	vsMisc[0] = jf.getXML1(xmlFile, vsTag);

	vsTag = { "path", "database" };
	vsMisc[1] = jf.getXML1(xmlFile, vsTag);
	return vsMisc;
}
string loadXML(WINFUNC& wf, const string& execPath)
{
	string xmlFile, xmlPath;
	vector<string> vsFileName;
	size_t pos1 = execPath.find_last_of("/\\");
	string folderPath = execPath.substr(0, pos1);
	while (1) {
		vsFileName = wf.getFileList(folderPath, "*.xml");
		for (int ii = 0; ii < vsFileName.size(); ii++) {
			xmlPath = folderPath + "/" + vsFileName[ii];
			xmlFile = wf.load(xmlPath);
			if (xmlFile.size() > 0) { return xmlFile; }
		}

		pos1 = folderPath.find_last_of("/\\");
		if (pos1 > folderPath.size()) {
			cout << "ERROR: Failed to locate a configuration .xml file ";
			cout << "in the executable directory or any parent directory." << endl;
			cin >> xmlPath;
			exit(EXIT_FAILURE);
		}
		folderPath = folderPath.substr(0, pos1);
	}
	return "";
}
unique_ptr<Wt::WApplication> makeApp(const Wt::WEnvironment& env, SCDAserver& myServer)
{
	return make_unique<SCDAapp>(env, myServer);
}
vector<string> makeServerArgs(JFUNC& jf, string& xmlFile)
{
	vector<string> vsTag = { "server_args" };
	vector<vector<string>> vvsTag = jf.getXML(xmlFile, vsTag);
	int numArg = (int)vvsTag.size();
	if (numArg < 1) {
		cout << "ERROR: xml file contained no server input arguments." << endl;
		cin >> vsTag[0];
		exit(EXIT_FAILURE);
	}
	vector<string> vsArgs(numArg);
	for (int ii = 0; ii < numArg; ii++) {
		vsArgs[ii] = vvsTag[ii][1];
	}
	return vsArgs;
}

int main()
{
	JFUNC jf;
	WINFUNC wf;
	int signal;
	const string execPath = wf.getExecPath(), wtAppPath = "";
	string xmlFile = loadXML(wf, execPath);
	vector<string> vsMisc = getMiscPath(jf, xmlFile);
	JLOG::getInstance()->init(vsMisc[0], "SCDA-Web");
	const vector<string> args = makeServerArgs(jf, xmlFile);
	cleanTemp(jf, wf, xmlFile);

	Wt::WServer wtServer(execPath, args, wtAppPath);
	SCDAserver myServer(wtServer, vsMisc[1]);
	wtServer.addEntryPoint(Wt::EntryPointType::Application, bind(makeApp, placeholders::_1, ref(myServer)));
	myServer.configXML = xmlFile;
	if (wtServer.start()) {
		signal = Wt::WServer::waitForShutdown();
		cerr << "wtServer is shutting down: " << signal << endl;
		wtServer.stop();
	}
	return 0;
}