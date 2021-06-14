#pragma once
#include <Wt/WServer.h>
#include <functional>
#include "jtree.h"
#include "sqlfunc.h"
#include "winfunc.h"
#include "wtfunc.h"

using namespace std;
extern const string sroot;
extern mutex m_server;

class DataEvent
{
public:
	const vector<vector<int>> tree_st;
	const vector<wstring> wtree_pl;
	const vector<string> tree_pl;
	const vector<string> list;
	const vector<wstring> wlist;
	const vector<vector<wstring>> wtable;
	const vector<string> ancestry;
	const string stext;
	const vector<Wt::WPainterPath> wpPaths;
	const vector<vector<Wt::WPointF>> areas;
	const vector<double> regionData;

	enum eType { Connect, Table, Map, RootLayer, YearLayer, DescLayer, RegionLayer, DivLayer };
	string getSessionID() { return sessionID; }
	vector<string> get_ancestry() const { return ancestry; }
	vector<vector<Wt::WPointF>> get_areas() const { return areas; }
	vector<string> get_list() const { return list; }
	vector<wstring> get_wlist() const { return wlist; }
	vector<vector<wstring>> get_wtable() const { return wtable; }
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

	// Constructor for eTypes Table, DescLayer, RegionLayer.
	DataEvent(eType et, const string& sID, const vector<vector<wstring>>& wtbl)
		: etype(et), sessionID(sID), wtable(wtbl) {}

	// Constructor for eTypes RootLayer, YearLayer.
	DataEvent(eType et, const string& sID, const vector<string>& lst) 
		: etype(et), sessionID(sID), list(lst) {}

	// Constructor for eType DivLayer.
	DataEvent(eType et, const string& sID, const vector<wstring>& wlst)
		: etype(et), sessionID(sID), wtree_pl(wlst) {}

	// Constructor for eType Label.
	DataEvent(eType et, const string& sID, const string& txt)
		: etype(et), sessionID(sID), stext(txt) {}

	// Constructor for eType Map.
	DataEvent(eType et, const vector<string>& sIDregion, const vector<vector<Wt::WPointF>>& area, const vector<double>& rData)
		: etype(et), sessionID(sIDregion[0]), list(sIDregion), areas(area), regionData(rData) {}

	friend class SCDAserver;
};

typedef function<void(const DataEvent&)> DataEventCallback;

class SCDAserver
{
public:
	SCDAserver(Wt::WServer&);
	SCDAserver(const SCDAserver&) = delete;
	SCDAserver& operator=(const SCDAserver&) = delete;
	class User {};

	vector<vector<int>> binMapBorder(string& tname0);
	vector<vector<vector<int>>> binMapFrames(string& tname0);
	string binMapParent(string& tname0);
	vector<double> binMapPosition(string& tname0);
	double binMapScale(string& tname0);
	bool connect(User* user, const DataEventCallback& handleEvent);
	void init(int&);
	string getLinearizedColTitle(string& sCata, string& rowTitle, string& colTitle);
	vector<vector<string>> getSmallGeo(vector<vector<string>>& cataGeo, string sParent);
	long long getTimer();
	vector<string> getYearList();
	void pullMap(vector<string> prompt);
	vector<Wt::WPointF> pullMapChild(vector<string>& geoLayers, vector<vector<string>>& smallGeo, int myIndex, vector<double>& mapScaling);
	vector<Wt::WPointF> pullMapParent(string& cataDesc, vector<string>& geoLayers, vector<vector<string>>& cataGeo, vector<double>& mapScaling);
	void pullLayer(int layer, vector<string> prompt);
	void pullRegion(vector<string> prompt);
	void pullTable(vector<string> prompt);
	void setDesc(vector<string>);
	void setYear(vector<string>);

private:
	vector<string> vsTemp;
	JFUNC jf;
	SQLFUNC sf;
	WTFUNC *wtf;
	struct UserInfo
	{
		string sessionID;
		DataEventCallback eventCallback;
	};

	unordered_map<string, int> cataMap;  // Cata desc -> gidTree index.
	const string db_path = sroot + "\\SCDA.db";
	vector<vector<vector<int>>> gidTreeSt;  // Form [gidTree index][node index][ancestry, node, children]
	vector<vector<int>> gidTreePl;  // Form [gidTree index][node index] GID.
	Wt::WServer& serverRef;
	typedef map<User*, UserInfo> userMap;
	userMap users;

	void err(string);
	void postDataEvent(const DataEvent& event, string sID);
};

