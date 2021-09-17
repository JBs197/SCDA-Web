#pragma once
#include <Wt/WServer.h>
#include <Wt/WCanvasPaintDevice.h>
#include <Wt/WMeasurePaintDevice.h>
#include <Wt/WTree.h>
#include <Wt/WTreeNode.h>
#include <functional>
#include "jtree.h"
#include "sqlfunc.h"
#include "winfunc.h"
#include "wjconfig.h"
#include "wjpdf.h"
#include "wjtable.h"
#include "wtpaint.h"

using namespace std;
extern const string sroot;
extern mutex m_server;

class DataEvent
{
public:
	const JTREE tree;
	const int numCata;
	const vector<string> list, listCol, listRow;
	const vector<vector<string>> catalogue, table, variable;
	const vector<string> ancestry;
	const string sYear, sCata, sRegion;
	const vector<Wt::WPainterPath> wpPaths;
	const vector<vector<vector<double>>> areas;
	const vector<vector<double>> regionData;

	enum eType { Catalogue, Category, Connect, Demographic, Differentiation, Map, Parameter, Table, Topic, Tree };
	string getSessionID() { return sessionID; }
	vector<string> get_ancestry() const { return ancestry; }
	vector<vector<vector<double>>> get_areas() const { return areas; }
	vector<vector<string>> getCata() const { return catalogue; }
	vector<string> get_list() const { return list; }
	vector<string> getListCol() const { return listCol; }
	vector<string> getListRow() const { return listRow; }
	int getNumCata() const { return numCata; }
	vector<vector<double>> getRegionData() const { return regionData; }
	string getSCata() const { return sCata; }
	string getSRegion() const { return sRegion; }
	string getSYear() const { return sYear; }
	vector<vector<string>> getTable() const { return table; }
	JTREE getTree() const { return tree; }
	vector<vector<string>> getVariable() const { return variable; }
	vector<Wt::WPainterPath> get_wpPaths() const { return wpPaths; }
	int type() const { return etype; }
	
private:
	eType etype;
	string sessionID;

	// Constructor for eType Catalogue.
	DataEvent(eType et, const string& sID, const string& sy, const string& sc)
		: etype(et), sessionID(sID), numCata(1), sYear(sy), sCata(sc) {}

	// Constructor for eType Connect.
	DataEvent(eType et, const string& sID, const int& nC) 
		: etype(et), sessionID(sID), numCata(nC) {}

	// Constructor for eType Demographic.
	DataEvent(eType et, const string& sID, const int& nC, const vector<vector<string>>& vvs)
		: etype(et), sessionID(sID), numCata(nC), variable(vvs) {}

	// Constructor for eTypes Category and Differentiation.
	DataEvent(eType et, const string& sID, const int& nC, const vector<string>& vsList)
		: etype(et), sessionID(sID), numCata(nC), list(vsList) {}

	// Constructor for eType Map.
	DataEvent(eType et, const string& sID, const vector<string>& vsRegion, const vector<vector<vector<double>>>& area, const vector<vector<double>>& rData)
		: etype(et), sessionID(sID), list(vsRegion), areas(area), regionData(rData), numCata(1) {}

	// Constructor for eType Parameter.
	DataEvent(eType et, const string& sID, const int& nC, const vector<vector<string>>& var, const vector<vector<string>>& cata, const vector<string>& DIMIndex)
		: etype(et), sessionID(sID), numCata(nC), variable(var), catalogue(cata), list(DIMIndex) {}

	// Constructor for eType Table.
	DataEvent(eType et, const string& sID, const vector<vector<string>>& vvsTable, const vector<string>& vsCol, const vector<string>& vsRow, const string& region)
		: etype(et), sessionID(sID), table(vvsTable), listCol(vsCol), listRow(vsRow), sRegion(region), numCata(1) {}

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

public:
	SCDAserver(Wt::WServer& wtServer, string& dbPath) 
		: serverRef(wtServer), db_path(dbPath) {
		sf.init(db_path);
	}
	SCDAserver(const SCDAserver&) = delete;
	SCDAserver& operator=(const SCDAserver&) = delete;
	class User {};

	const double cellMargin = 4.0;  // Unit of pixels, applied vertically and horizontally.
	Wt::WFont wfTable = Wt::WFont();

	void addFrameKM(vector<vector<vector<double>>>& borderKM, vector<string>& vsGeoCode, string sYear);
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
	vector<string> getColTitle(string sYear, string sCata);
	vector<double> getDataFamily(string sYear, string sCata, vector<string> vsIndex, vector<string> vsGeoCode);
	vector<string> getDataIndex(string sYear, string sCata, vector<vector<string>>& vvsDIM);
	vector<string> getDIMIndex(vector<vector<string>>& vvsCata);
	vector<vector<string>> getForWhom(vector<vector<string>>& vvsCata);
	vector<vector<string>> getGeo(string sYear, string sCata);
	int getGeoFamily(vector<vector<string>>& geo, vector<string>& vsGeoCode, vector<string>& vsRegionName);
	string getLinearizedColTitle(string& sCata, string& rowTitle, string& colTitle);
	vector<vector<string>> getParameter(vector<vector<string>>& vvsCata, vector<vector<string>>& vvsFixed);
	vector<double> getPopulationFamily(string sYear, vector<string>& vsGeoCode);
	vector<string> getRowTitle(string sYear, string sCata);
	long long getTimer();
	vector<string> getTopicList(vector<string> vsYear);
	string getUnit(int clientIndex, string sYear, string sCata, string sDimMID);
	vector<string> getVariable(vector<vector<string>>& vvsCata, vector<string>& vsFixed);
	vector<string> getYear(string sYear);
	string getYear(string sYear, string sCata);
	void pullCategory(vector<string> prompt);
	void pullDifferentiator(vector<string> prompt, vector<vector<string>> vvsCata);
	void pullMap(vector<string> prompt, vector<vector<string>> vvsDIM);
	void pullTable(vector<string> prompt, vector<vector<string>> vvsDIM);
	void pullTopic(vector<string> prompt);
	void pullTree(vector<string> prompt);
	void pullVariable(vector<string> prompt, vector<vector<string>> variable);

private:
	JFUNC jf;
	JTREE jt;
	SQLFUNC sf;
	vector<shared_ptr<WJTABLE>> wjTable;
	vector<shared_ptr<WTPAINT>> wtPaint;
	struct UserInfo
	{
		string sessionID;
		DataEventCallback eventCallback;
	};

	unordered_map<string, int> cataMap;  // Cata desc -> gidTree index.
	const string db_path;
	unordered_map<string, string> mapPopCata;  // externalYear -> internalYear$sCata (general population)
	unordered_map<string, string> mapPopDI;  // sCata -> dataIndex$dimIndex (general population)
	Wt::WServer& serverRef;
	typedef map<User*, UserInfo> userMap;
	userMap users;
	Wt::WFont wFont;

	void err(string);
	void postDataEvent(const DataEvent& event, string sID);
};

