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
#include "jcrc32.h"
#include "jfile.h"
#include "jscalebar.h"
#include "jsort.h"
#include "jtree.h"
#include "sqlfunc.h"
#include "winfunc.h"
#include "wjbargraph.h"
#include "wjconfig.h"
#include "wjdownload.h"
#include "wjdrag.h"
#include "wjmap.h"
#include "wjtable.h"
#include "wjtree.h"
#include "wtpaint.h"

extern std::mutex m_server;

class DataEvent
{
public:
	const JTREE tree;
	const int numCata;
	const std::vector<std::string> list, listCol, listRow, vsNamePop;
	const std::vector<std::vector<std::string>> catalogue, table, treeCol, treeRow, variable;
	const std::vector<std::string> ancestry;
	const std::vector<std::vector<std::vector<std::string>>> parameter;
	const std::string sYear, sCata, sRegion, title;
	const std::vector<Wt::WPainterPath> wpPaths;
	const std::vector<std::vector<std::vector<double>>> areas, frames;
	const std::vector<std::vector<double>> regionData;

	enum eType { Catalogue, Category, Connection, Demographic, Differentiation, Map, Parameter, Table, Topic, Tree };
	std::string getSessionID() { return sessionID; }
	std::vector<std::string> get_ancestry() const { return ancestry; }
	std::vector<std::vector<std::vector<double>>> getAreas() const { return areas; }
	std::vector<std::vector<std::string>> getCata() const { return catalogue; }
	std::vector<std::vector<std::string>> getCol() const { return treeCol; }
	std::vector<std::vector<std::vector<double>>> getFrames() const { return frames; }
	std::vector<std::string> get_list() const { return list; }
	std::vector<std::string> getListCol() const { return listCol; }
	std::vector<std::string> getListRow() const { return listRow; }
	std::vector<std::string> getNamePop() const { return vsNamePop; }
	int getNumCata() const { return numCata; }
	std::vector<std::vector<std::vector<std::string>>> getParameter() const { return parameter; }
	std::vector<std::vector<double>> getRegionData() const { return regionData; }
	std::vector<std::vector<std::string>> getRow() const { return treeRow; }
	std::string getSCata() const { return sCata; }
	std::string getSRegion() const { return sRegion; }
	std::string getSYear() const { return sYear; }
	std::vector<std::vector<std::string>> getTable() const { return table; }
	std::string getTitle() const { return title; }
	const JTREE getTree() const { return tree; }
	std::vector<std::vector<std::string>> getVariable() const { return variable; }
	std::vector<Wt::WPainterPath> get_wpPaths() const { return wpPaths; }
	int type() const { return etype; }
	
private:
	eType etype;
	std::string sessionID;

	// Constructor for eType Catalogue.
	DataEvent(eType et, const std::string& sID, const std::string& sy, const std::string& sc)
		: etype(et), sessionID(sID), numCata(1), sYear(sy), sCata(sc) {}

	// Constructor for eTypes Category.
	DataEvent(eType et, const std::string& sID, const int& nC, const std::vector<std::string>& vsList)
		: etype(et), sessionID(sID), numCata(nC), list(vsList) {}

	// Constructor for eType Connection.
	DataEvent(eType et, const std::string& sID)
		: etype(et), sessionID(sID), numCata(1) {}

	// Constructor for eType Demographic.
	DataEvent(eType et, const std::string& sID, const int& nC, const std::vector<std::vector<std::string>>& vvs)
		: etype(et), sessionID(sID), numCata(nC), variable(vvs) {}

	// Constructor for eType Differentiator.
	DataEvent(eType et, const std::string& sID, const int& nC, const std::vector<std::string>& vsList, const std::string& sTitle)
		: etype(et), sessionID(sID), numCata(nC), list(vsList), title(sTitle) {}

	// Constructor for eType Map.
	DataEvent(eType et, const std::string& sID, const std::vector<std::string>& vsRegion, const std::vector<std::vector<std::vector<double>>>& frame, const std::vector<std::vector<std::vector<double>>>& area, const std::vector<std::vector<double>>& rData)
		: etype(et), sessionID(sID), list(vsRegion), frames(frame), areas(area), regionData(rData), numCata(1) {}

	// Constructor for eType Parameter.
	DataEvent(eType et, const std::string& sID, const int& nC, const std::vector<std::vector<std::vector<std::string>>>& param, const std::vector<std::vector<std::string>>& cata)
		: etype(et), sessionID(sID), numCata(nC), parameter(param), catalogue(cata){}

	// Constructor for eType Table.
	DataEvent(eType et, const std::string& sID, const std::vector<std::vector<std::string>>& vvsTable, const std::vector<std::vector<std::string>>& vvsCol, const std::vector<std::vector<std::string>>& vvsRow, const std::vector<std::string>& regionNamePop)
		: etype(et), sessionID(sID), table(vvsTable), treeCol(vvsCol), treeRow(vvsRow), vsNamePop(regionNamePop), numCata(1) {}

	// Constructor for eType Topic.
	DataEvent(eType et, const std::string& sID, const int& nC, const std::vector<std::string>& vsCol, const std::vector<std::string>& vsRow)
		: etype(et), sessionID(sID), numCata(nC), listCol(vsCol), listRow(vsRow) {}

	// Constructor for eType Tree.
	DataEvent(eType et, const std::string& sID, const JTREE& jt)
		: etype(et), sessionID(sID), numCata(1), tree(jt) {}

	friend class SCDAserver;
};

typedef std::function<void(const DataEvent&)> DataEventCallback;

class SCDAserver
{
	std::unordered_map<std::string, int> mapClientIndex;  // sessionID -> shared_ptr index

	void err(std::string message);

public:
	SCDAserver(Wt::WServer& wtServer, std::string& dbPath)
		: serverRef(wtServer), db_path(dbPath) {
		sf.init(db_path);
	}
	SCDAserver(const SCDAserver&) = delete;
	SCDAserver& operator=(const SCDAserver&) = delete;
	class User {};

	const double cellMargin = 4.0;  // Unit of pixels, applied vertically and horizontally.
	std::string configXML;
	Wt::WFont wfTable = Wt::WFont();
	JFILE jfile;

	int applyCataFilter(std::vector<std::vector<std::string>>& vvsCata, std::vector<std::vector<std::string>>& vvsDIM);
	std::vector<std::vector<int>> binMapBorder(std::string& tname0);
	std::vector<std::vector<std::vector<int>>> binMapFrames(std::string& tname0);
	std::string binMapParent(std::string& tname0);
	std::vector<double> binMapPosition(std::string& tname0);
	double binMapScale(std::string& tname0);
	void cleanTempFolder(std::string& docRoot);
	std::vector<std::vector<std::string>> completeVariable(std::vector<std::vector<std::string>>& vvsCata, std::vector<std::vector<std::string>>& vvsFixed, std::string sYear);
	bool connect(User* user, const DataEventCallback& handleEvent);
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

