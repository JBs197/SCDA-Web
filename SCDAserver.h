#pragma once
#include <Wt/WServer.h>
#include <Wt/WOverlayLoadingIndicator.h>
#include <Wt/WCanvasPaintDevice.h>
#include <Wt/WDateTime.h>
#include <Wt/WDate.h>
#include <Wt/WMeasurePaintDevice.h>
#include <Wt/WMemoryResource.h>
#include <Wt/WTime.h>
#include <Wt/WTree.h>
#include <Wt/WTreeNode.h>
#include <functional>
#include "jcrc32.h"
#include "jscalebar.h"
#include "jtree.h"
#include "sqlfunc.h"
#include "winfunc.h"
#include "wjbargraph.h"
#include "wjconfig.h"
#include "wjdownload.h"
#include "wjdrag.h"
#include "wjmap.h"
#include "wjtable.h"
#include "wtpaint.h"

using namespace std;
extern mutex m_server;

class DataEvent
{
public:
	const JTREE tree;
	const int numCata;
	const vector<string> list, listCol, listRow, vsNamePop;
	const vector<vector<string>> catalogue, table, treeCol, treeRow, variable;
	const vector<string> ancestry;
	const vector<vector<vector<string>>> parameter;
	const string sYear, sCata, sRegion, title;
	const vector<Wt::WPainterPath> wpPaths;
	const vector<vector<vector<double>>> areas, frames;
	const vector<vector<double>> regionData;

	enum eType { Catalogue, Category, Connection, Demographic, Differentiation, Map, Parameter, Table, Topic, Tree };
	string getSessionID() { return sessionID; }
	vector<string> get_ancestry() const { return ancestry; }
	vector<vector<vector<double>>> getAreas() const { return areas; }
	vector<vector<string>> getCata() const { return catalogue; }
	vector<vector<string>> getCol() const { return treeCol; }
	vector<vector<vector<double>>> getFrames() const { return frames; }
	vector<string> get_list() const { return list; }
	vector<string> getListCol() const { return listCol; }
	vector<string> getListRow() const { return listRow; }
	vector<string> getNamePop() const { return vsNamePop; }
	int getNumCata() const { return numCata; }
	vector<vector<vector<string>>> getParameter() const { return parameter; }
	vector<vector<double>> getRegionData() const { return regionData; }
	vector<vector<string>> getRow() const { return treeRow; }
	string getSCata() const { return sCata; }
	string getSRegion() const { return sRegion; }
	string getSYear() const { return sYear; }
	vector<vector<string>> getTable() const { return table; }
	string getTitle() const { return title; }
	const JTREE getTree() const { return tree; }
	vector<vector<string>> getVariable() const { return variable; }
	vector<Wt::WPainterPath> get_wpPaths() const { return wpPaths; }
	int type() const { return etype; }
	
private:
	eType etype;
	string sessionID;

	// Constructor for eType Catalogue.
	DataEvent(eType et, const string& sID, const string& sy, const string& sc)
		: etype(et), sessionID(sID), numCata(1), sYear(sy), sCata(sc) {}

	// Constructor for eTypes Category.
	DataEvent(eType et, const string& sID, const int& nC, const vector<string>& vsList)
		: etype(et), sessionID(sID), numCata(nC), list(vsList) {}

	// Constructor for eType Connection.
	DataEvent(eType et, const string& sID)
		: etype(et), sessionID(sID), numCata(1) {}

	// Constructor for eType Demographic.
	DataEvent(eType et, const string& sID, const int& nC, const vector<vector<string>>& vvs)
		: etype(et), sessionID(sID), numCata(nC), variable(vvs) {}

	// Constructor for eType Differentiator.
	DataEvent(eType et, const string& sID, const int& nC, const vector<string>& vsList, const string& sTitle)
		: etype(et), sessionID(sID), numCata(nC), list(vsList), title(sTitle) {}

	// Constructor for eType Map.
	DataEvent(eType et, const string& sID, const vector<string>& vsRegion, const vector<vector<vector<double>>>& frame, const vector<vector<vector<double>>>& area, const vector<vector<double>>& rData)
		: etype(et), sessionID(sID), list(vsRegion), frames(frame), areas(area), regionData(rData), numCata(1) {}

	// Constructor for eType Parameter.
	DataEvent(eType et, const string& sID, const int& nC, const vector<vector<vector<string>>>& param, const vector<vector<string>>& cata)
		: etype(et), sessionID(sID), numCata(nC), parameter(param), catalogue(cata){}

	// Constructor for eType Table.
	DataEvent(eType et, const string& sID, const vector<vector<string>>& vvsTable, const vector<vector<string>>& vvsCol, const vector<vector<string>>& vvsRow, const vector<string>& regionNamePop)
		: etype(et), sessionID(sID), table(vvsTable), treeCol(vvsCol), treeRow(vvsRow), vsNamePop(regionNamePop), numCata(1) {}

	// Constructor for eType Topic.
	DataEvent(eType et, const string& sID, const int& nC, const vector<string>& vsCol, const vector<string>& vsRow)
		: etype(et), sessionID(sID), numCata(nC), listCol(vsCol), listRow(vsRow) {}

	// Constructor for eType Tree.
	DataEvent(eType et, const string& sID, const JTREE& jt)
		: etype(et), sessionID(sID), numCata(1), tree(jt) {}

	friend class SCDAserver;
};

typedef function<void(const DataEvent&)> DataEventCallback;

class SCDAserver
{
	unordered_map<string, int> mapClientIndex;  // sessionID -> shared_ptr index

	void err(string message);

public:
	SCDAserver(Wt::WServer& wtServer, string& dbPath) 
		: serverRef(wtServer), db_path(dbPath) {
		sf.init(db_path);
		//initLog();
	}
	SCDAserver(const SCDAserver&) = delete;
	SCDAserver& operator=(const SCDAserver&) = delete;
	class User {};

	const double cellMargin = 4.0;  // Unit of pixels, applied vertically and horizontally.
	Wt::WFont wfTable = Wt::WFont();
	JFUNC jf;

	int applyCataFilter(vector<vector<string>>& vvsCata, vector<vector<string>>& vvsDIM);
	vector<vector<int>> binMapBorder(string& tname0);
	vector<vector<vector<int>>> binMapFrames(string& tname0);
	string binMapParent(string& tname0);
	vector<double> binMapPosition(string& tname0);
	double binMapScale(string& tname0);
	vector<vector<string>> completeVariable(vector<vector<string>>& vvsCata, vector<vector<string>>& vvsFixed, string sYear);
	bool connect(User* user, const DataEventCallback& handleEvent);
	int init(string sessionID);
	void initPopulation();
	vector<vector<vector<double>>> getBorderKM(vector<string>& vsGeoCode, string sYear);
	vector<vector<string>> getCatalogue(vector<string>& vsPrompt);
	vector<vector<string>> getCatalogue(vector<string>& vsPrompt, vector<vector<string>>& vvsVariable);
	vector<vector<string>> getColTitle(string sYear, string sCata);
	vector<double> getDataFamily(string sYear, string sCata, vector<string> vsIndex, vector<string> vsGeoCode);
	vector<string> getDataIndex(string sYear, string sCata, vector<vector<string>>& vvsDIM);
	vector<string> getDataIndex(string sYear, string sCata, vector<string>& vsDIMtitle, vector<int>& viMID);
	vector<string> getDifferentiatorMID(vector<vector<string>>& vvsCata, vector<vector<string>>& vvsFixed);
	vector<string> getDifferentiatorTitle(vector<vector<string>>& vvsCata, vector<string>& vsFixed);
	vector<string> getDIMIndex(vector<vector<string>>& vvsCata);
	vector<vector<vector<double>>> getFrameKM(vector<string>& vsGeoCode, string sYear);
	vector<vector<string>> getForWhom(vector<vector<string>>& vvsCata);
	vector<vector<string>> getGeo(string sYear, string sCata);
	int getGeoFamily(vector<vector<string>>& geo, vector<string>& vsGeoCode, vector<string>& vsRegionName);
	string getLinearizedColTitle(string& sCata, string& rowTitle, string& colTitle);
	vector<vector<vector<string>>> getParameter(vector<vector<string>>& vvsCata, vector<vector<string>>& vvsFixed);
	vector<double> getPopulationFamily(string sYear, vector<string>& vsGeoCode);
	vector<vector<string>> getRowTitle(string sYear, string sCata);
	long long getTimer();
	vector<string> getTopicList(vector<string> vsYear);
	string getUnit(int clientIndex, string sYear, string sCata, string sDimMID);
	vector<string> getYear(string sYear);
	string getYear(string sYear, string sCata);
	void log(vector<string> vsColumn);
	void pullCategory(vector<string> prompt);
	void pullConnection(string sessionID);
	void pullDifferentiator(string prompt, vector<vector<string>> vvsCata, vector<vector<string>> vvsDiff);
	void pullMap(vector<string> prompt, vector<string> vsDIMtitle, vector<int> viMID);
	void pullTable(vector<string> prompt, vector<string> vsDIMtitle, vector<int> viMID);
	void pullTopic(vector<string> prompt);
	void pullTree(vector<string> prompt);
	void pullVariable(vector<string> prompt, vector<vector<string>> variable);

private:
	JSTRING jstr;
	JTREE jt;
	SQLFUNC sf, sfLog;
	vector<shared_ptr<WJTABLE>> wjTable;
	vector<shared_ptr<WTPAINT>> wtPaint;
	struct UserInfo
	{
		string sessionID;
		DataEventCallback eventCallback;
	};

	unordered_map<string, int> cataMap;  // Cata desc -> gidTree index.
	const string db_path;
	string dbLogPath;
	unordered_map<string, string> mapPopCata;  // externalYear -> internalYear$sCata (general population)
	unordered_map<string, string> mapPopDI;  // sCata -> dataIndex$dimIndex (general population)
	Wt::WServer& serverRef;
	typedef map<User*, UserInfo> userMap;
	userMap users;
	Wt::WFont wFont;

	void initLog();
	void postDataEvent(const DataEvent& event, string sID);
};

