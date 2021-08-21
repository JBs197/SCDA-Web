#pragma once
#include <Wt/WServer.h>
#include <functional>
#include "jtree.h"
#include "sqlfunc.h"
#include "winfunc.h"
#include "wtpaint.h"

using namespace std;
extern const string sroot;
extern mutex m_server;

class DataEvent
{
public:
	const JTREE tree;
	const vector<string> list, listCol, listRow;
	const vector<vector<string>> catalogue, table, variable;
	const vector<string> ancestry;
	const string stext;
	const vector<Wt::WPainterPath> wpPaths;
	const vector<vector<vector<double>>> areas;
	const vector<double> regionData;

	enum eType { Connect, Label, Map, Table, Topic, Tree, Variable };
	string getSessionID() { return sessionID; }
	vector<string> get_ancestry() const { return ancestry; }
	vector<vector<vector<double>>> get_areas() const { return areas; }
	vector<vector<string>> getCata() const { return catalogue; }
	vector<string> get_list() const { return list; }
	vector<string> getListCol() const { return listCol; }
	vector<string> getListRow() const { return listRow; }
	vector<double> get_regionData() const { return regionData; }
	vector<vector<string>> getTable() const { return table; }
	string get_text() const { return stext; }
	JTREE getTree() const { return tree; }
	vector<vector<string>> getVariable() const { return variable; }
	vector<Wt::WPainterPath> get_wpPaths() const { return wpPaths; }
	int type() const { return etype; }
	
private:
	eType etype;
	string sessionID;

	// Constructor for eType Connect.
	DataEvent(eType et, const string& sID) : etype(et), sessionID(sID) {}

	// Constructor for eType Label.
	DataEvent(eType et, const string& sID, const string& txt)
		: etype(et), sessionID(sID), stext(txt) {}

	// Constructor for eType Map.
	DataEvent(eType et, const string& sID, const vector<string>& vsRegion, const vector<vector<vector<double>>>& area, const vector<double>& rData)
		: etype(et), sessionID(sID), list(vsRegion), areas(area), regionData(rData) {}

	// Constructor for eType Table.
	DataEvent(eType et, const string& sID, const vector<vector<string>>& vvsTable, const vector<string>& vsCol, const vector<string>& vsRow)
		: etype(et), sessionID(sID), table(vvsTable), listCol(vsCol), listRow(vsRow) {}

	// Constructor for eType Topic.
	DataEvent(eType et, const string& sID, const vector<string>& vsCol, const vector<string>& vsRow)
		: etype(et), sessionID(sID), listCol(vsCol), listRow(vsRow) {}

	// Constructor for eType Tree.
	DataEvent(eType et, const string& sID, const JTREE& jt)
		: etype(et), sessionID(sID), tree(jt) {}

	// Constructor for eType Variable.
	DataEvent(eType et, const string& sID, const vector<vector<string>>& var, const vector<vector<string>>& cata, const vector<string>& DIMIndex)
		: etype(et), sessionID(sID), variable(var), catalogue(cata), list(DIMIndex) {}

	friend class SCDAserver;
};

typedef function<void(const DataEvent&)> DataEventCallback;

class SCDAserver
{
public:
	SCDAserver(Wt::WServer& wtServer, string& dbPath) 
		: serverRef(wtServer), db_path(dbPath) {
		sf.init(db_path);
	}
	SCDAserver(const SCDAserver&) = delete;
	SCDAserver& operator=(const SCDAserver&) = delete;
	class User {};

	void addFrameKM(vector<vector<vector<double>>>& borderKM, vector<string>& vsGeoCode, string sYear);
	vector<vector<int>> binMapBorder(string& tname0);
	vector<vector<vector<int>>> binMapFrames(string& tname0);
	string binMapParent(string& tname0);
	vector<double> binMapPosition(string& tname0);
	double binMapScale(string& tname0);
	bool connect(User* user, const DataEventCallback& handleEvent);
	void init();
	vector<vector<vector<double>>> getBorderKM(vector<string>& vsGeoCode, string sYear);
	vector<vector<string>> getCatalogue(vector<string>& vsPrompt);
	vector<vector<string>> getCatalogue(vector<string>& vsPrompt, vector<vector<string>>& vvsVariable);
	vector<string> getColTitle(string sYear, string sCata);
	vector<double> getDataFamily(string sYear, string sCata, vector<string> vsIndex, vector<string> vsGeoCode);
	vector<string> getDataIndex(string sYear, string sCata, vector<vector<string>>& vvsDIM);
	vector<string> getDIMIndex(vector<vector<string>>& vvsCata);
	vector<vector<string>> getGeo(string sYear, string sCata);
	void getGeoFamily(vector<vector<string>>& geo, vector<string>& vsGeoCode, vector<string>& vsRegionName);
	string getLinearizedColTitle(string& sCata, string& rowTitle, string& colTitle);
	vector<string> getRowTitle(string sYear, string sCata);
	long long getTimer();
	vector<string> getTopicList(string sYear);
	vector<vector<string>> getVariableCandidate(vector<vector<string>>& vvsCata, vector<vector<string>>& vvsVariable);
	vector<string> getYear(string sYear);
	void pullMap(vector<string> prompt, vector<vector<string>> vvsDIM);
	void pullTable(vector<string> prompt, vector<vector<string>> vvsDIM);
	void pullTopic(vector<string> prompt);
	void pullTree(vector<string> prompt);
	void pullVariable(vector<string> prompt, vector<vector<string>> variable);

private:
	JFUNC jf;
	SQLFUNC sf;
	vector<string> vsTemp;
	WTPAINT *wtf;
	struct UserInfo
	{
		string sessionID;
		DataEventCallback eventCallback;
	};

	unordered_map<string, int> cataMap;  // Cata desc -> gidTree index.
	const string db_path;
	vector<vector<vector<int>>> gidTreeSt;  // Form [gidTree index][node index][ancestry, node, children]
	vector<vector<int>> gidTreePl;  // Form [gidTree index][node index] GID.
	Wt::WServer& serverRef;
	typedef map<User*, UserInfo> userMap;
	userMap users;

	void err(string);
	void postDataEvent(const DataEvent& event, string sID);
};

