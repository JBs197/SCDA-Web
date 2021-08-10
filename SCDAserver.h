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
	const vector<vector<int>> tree_st;
	const vector<wstring> wtree_pl;
	const vector<string> tree_pl;
	const vector<string> list, listCol, listRow;
	const vector<wstring> wlist;
	const vector<vector<string>> table;
	const vector<string> ancestry;
	const string stext;
	const vector<Wt::WPainterPath> wpPaths;
	const vector<vector<Wt::WPointF>> areas;
	const vector<double> regionData;

	enum eType { Connect, Label, Map, Table, Topic, Variable };
	string getSessionID() { return sessionID; }
	vector<string> get_ancestry() const { return ancestry; }
	vector<vector<Wt::WPointF>> get_areas() const { return areas; }
	vector<string> get_list() const { return list; }
	vector<string> getListCol() const { return listCol; }
	vector<string> getListRow() const { return listRow; }
	vector<wstring> get_wlist() const { return wlist; }
	vector<vector<string>> getTable() const { return table; }
	string get_text() const { return stext; }
	vector<string> get_tree_pl() const { return tree_pl; }
	vector<vector<int>> get_tree_st() const { return tree_st; }
	vector<wstring> get_wtree_pl() const { return wtree_pl; }
	int type() const { return etype; }
	vector<Wt::WPainterPath> get_wpPaths() const { return wpPaths; }
	vector<double> get_regionData() const { return regionData; }
	
private:
	eType etype;
	string sessionID;

	// Constructor for eType Connect.
	DataEvent(eType et, const string& sID) : etype(et), sessionID(sID) {}

	// Constructor for eType Label.
	DataEvent(eType et, const string& sID, const string& txt)
		: etype(et), sessionID(sID), stext(txt) {}

	// Constructor for eType Map.
	DataEvent(eType et, const vector<string>& sIDregion, const vector<vector<Wt::WPointF>>& area, const vector<double>& rData)
		: etype(et), sessionID(sIDregion[0]), list(sIDregion), areas(area), regionData(rData) {}

	// Constructor for eType Table.
	DataEvent(eType et, const string& sID, const vector<vector<string>>& vvsTable, const vector<string>& vsCol, const vector<string>& vsRow)
		: etype(et), sessionID(sID), table(vvsTable), listCol(vsCol), listRow(vsRow) {}

	// Constructor for eType Topic.
	DataEvent(eType et, const string& sID, const vector<string>& vsCol, const vector<string>& vsRow)
		: etype(et), sessionID(sID), listCol(vsCol), listRow(vsRow) {}

	// Constructor for eType Variable.
	DataEvent(eType et, const string& sID, const vector<string>& lst)
		: etype(et), sessionID(sID), list(lst) {}

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

	vector<vector<int>> binMapBorder(string& tname0);
	vector<vector<vector<int>>> binMapFrames(string& tname0);
	string binMapParent(string& tname0);
	vector<double> binMapPosition(string& tname0);
	double binMapScale(string& tname0);
	bool connect(User* user, const DataEventCallback& handleEvent);
	void init();
	vector<vector<string>> getCatalogue(string sYear, string sCategory, string sColTopic, string sRowTopic);
	vector<string> getDataIndex(string sYear, string sCata, string sDIM);
	string getLinearizedColTitle(string& sCata, string& rowTitle, string& colTitle);
	void getSmallGeo(vector<vector<string>>& smallGeo, string cataName);
	long long getTimer();
	vector<string> getTopicList(string sYear);
	vector<string> getYear(string sYear);
	void pullMap(vector<string> prompt);
	vector<Wt::WPointF> pullMapChild(vector<string>& geoLayers, vector<vector<string>>& smallGeo, int myIndex, vector<double>& mapScaling);
	vector<Wt::WPointF> pullMapParent(string& cataDesc, vector<string>& geoLayers, vector<vector<string>>& smallGeo, vector<double>& mapScaling);
	void pullMapParentBackspace(vector<vector<string>>& smallGeo, string cataName);
	void pullTable(vector<string> prompt);
	void pullTopic(vector<string> prompt);
	void pullVariable(vector<string> prompt);

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

