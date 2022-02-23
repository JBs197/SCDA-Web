#pragma once
#include <Wt/WServer.h>
#include <Wt/WOverlayLoadingIndicator.h>
#include <Wt/WCanvasPaintDevice.h>
#include <Wt/WDateTime.h>
#include <Wt/WDate.h>
#include <Wt/WMeasurePaintDevice.h>
#include <Wt/WMemoryResource.h>
#include <Wt/WTime.h>
#include <functional>
#include "catafilter.h"
#include "catafind.h"
#include "dataevent.h"
#include "jcrc32.h"
#include "jfile.h"
#include "jscalebar.h"
#include "jsort.h"
#include "jtree.h"
#include "winfunc.h"
#include "wjbargraph.h"
#include "wjcata.h"
#include "wjconfig.h"
#include "wjdownload.h"
#include "wjdrag.h"
#include "wjmap.h"
#include "wjtable.h"
#include "wjtree.h"
#include "wtpaint.h"

extern std::mutex m_err, m_server;

class SCDAserver
{
	std::shared_ptr<CataFind> cataFind;
	std::unordered_map<std::string, int> mapClientIndex;  // sessionID -> shared_ptr index

	void err(std::string message);

public:
	SCDAserver(Wt::WServer& wtServer, std::string& dbPath)
		: serverRef(wtServer), db_path(dbPath) {
		sf.init(db_path);
		cataFind = std::make_shared<CataFind>(dbPath);
	}
	SCDAserver(const SCDAserver&) = delete;
	SCDAserver& operator=(const SCDAserver&) = delete;
	~SCDAserver() = default;

	const double cellMargin = 4.0;  // Unit of pixels, applied vertically and horizontally.
	std::string configXML;
	Wt::WFont wfTable = Wt::WFont();
	JFILE jfile;
	JPARSE jparse;
	class User {};

	int applyCataFilter(std::vector<std::vector<std::string>>& vvsCata, std::vector<std::vector<std::string>>& vvsDIM);
	std::vector<std::vector<int>> binMapBorder(std::string& tname0);
	std::vector<std::vector<std::vector<int>>> binMapFrames(std::string& tname0);
	std::string binMapParent(std::string& tname0);
	std::vector<double> binMapPosition(std::string& tname0);
	double binMapScale(std::string& tname0);
	void cleanTempFolder(std::string& docRoot);
	std::vector<std::vector<std::string>> completeVariable(std::vector<std::vector<std::string>>& vvsCata, std::vector<std::vector<std::string>>& vvsFixed, std::string sYear);
	bool connect(User* user, const DataEventCallback& handleEvent);
	void errClient(std::string& message, std::string widget);
	int init(std::string sessionID);
	void initPopulation();
	std::vector<std::vector<std::vector<double>>> getBorderKM(std::vector<std::string>& vsGeoCode, std::string sYear);
	std::vector<std::vector<std::string>> getCatalogue(std::vector<std::string>& vsPrompt);
	std::vector<std::vector<std::string>> getCatalogue(std::vector<std::string>& vsPrompt, std::vector<std::vector<std::string>>& vvsVariable);
	std::vector<std::vector<std::string>> getColTitle(std::string sYear, std::string sCata);
	std::vector<double> getDataFamily(std::string sYear, std::string sCata, std::vector<std::string> vsIndex, std::vector<std::string> vsGeoCode);
	std::vector<std::string> getDataIndex(std::string sYear, std::string sCata, std::vector<std::vector<std::string>>& vvsDIM);
	std::vector<std::string> getDataIndex(std::string sYear, std::string sCata, std::vector<std::string>& vsDIMtitle, std::vector<int>& viMID);
	std::vector<std::string> getDifferentiatorMID(std::vector<std::vector<std::string>>& vvsCata, std::vector<std::vector<std::string>>& vvsFixed);
	std::vector<std::string> getDifferentiatorTitle(std::vector<std::vector<std::string>>& vvsCata, std::vector<std::string>& vsFixed);
	std::vector<std::string> getDIMIndex(std::vector<std::vector<std::string>>& vvsCata);
	std::vector<std::vector<std::vector<double>>> getFrameKM(std::vector<std::string>& vsGeoCode, std::string sYear);
	std::vector<std::vector<std::string>> getForWhom(std::vector<std::vector<std::string>>& vvsCata);
	std::vector<std::vector<std::string>> getGeo(std::string sYear, std::string sCata);
	int getGeoFamily(std::vector<std::vector<std::string>>& geo, std::vector<std::string>& vsGeoCode, std::vector<std::string>& vsRegionName);
	std::string getLinearizedColTitle(std::string& sCata, std::string& rowTitle, std::string& colTitle);
	std::vector<std::vector<std::vector<std::string>>> getParameter(std::vector<std::vector<std::string>>& vvsCata, std::vector<std::vector<std::string>>& vvsFixed);
	std::vector<double> getPopulationFamily(std::string sYear, std::vector<std::string>& vsGeoCode);
	std::vector<std::vector<std::string>> getRowTitle(std::string sYear, std::string sCata);
	long long getTimer();
	std::vector<std::string> getTopicList(std::vector<std::string> vsYear);
	std::string getUnit(int clientIndex, std::string sYear, std::string sCata, std::string sDimMID);
	std::vector<std::string> getYear(std::string sYear);
	std::string getYear(std::string sYear, std::string sCata);
	
	void log(std::vector<std::string> vsColumn);
	void makeTreeGeo(JTREE& jt, std::vector<std::vector<std::string>>& geo);
	
	void pullCata(std::string sessionID, CataFilter cataFilter);
	void pullCataAll(std::string sessionID);
	void pullCategory(std::vector<std::string> prompt);
	void pullConnection(std::string sessionID);
	void pullDifferentiator(std::string prompt, std::vector<std::vector<std::string>> vvsCata, std::vector<std::vector<std::string>> vvsDiff);
	void pullMap(std::vector<std::string> prompt, std::vector<std::string> vsDIMtitle, std::vector<int> viMID);
	void pullTable(std::vector<std::string> prompt, std::vector<std::string> vsDIMtitle, std::vector<int> viMID);
	void pullTopic(std::vector<std::string> prompt);
	void pullTree(std::vector<std::string> prompt);
	void pullVariable(std::vector<std::string> prompt, std::vector<std::vector<std::string>> variable);

private:
	JNUMBER jnumber;
	JSORT jsort;
	JSTRING jstr;
	JTIME jtime;
	JTREE jt;
	SQLFUNC sf, sfLog;
	std::vector<std::shared_ptr<WJTABLE>> wjTable;
	std::vector<std::shared_ptr<WTPAINT>> wtPaint;
	struct UserInfo
	{
		std::string sessionID;
		DataEventCallback eventCallback;
	};

	std::unordered_map<std::string, int> cataMap;  // Cata desc -> gidTree index.
	const std::string db_path;
	std::string dbLogPath;
	std::unordered_map<std::string, std::string> mapPopCata;  // externalYear -> internalYear$sCata (general population)
	std::unordered_map<std::string, std::string> mapPopDI;  // sCata -> dataIndex$dimIndex (general population)
	Wt::WServer& serverRef;
	typedef std::map<User*, UserInfo> userMap;
	userMap users;
	Wt::WFont wFont;

	void initLog();
	void postDataEvent(const DataEvent& event, std::string sID);
};

