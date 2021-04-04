#pragma once
#include <Wt/WServer.h>
#include <functional>
#include "jfunc.h"
#include "sqlfunc.h"

using namespace std;
extern const string sroot;
extern mutex m_server;
const int maxTreeLayers = 6; // Not counting the root.

class DataEvent
{
public:
	const vector<vector<int>> tree_st;
	const vector<wstring> wtree_pl;
	const vector<string> tree_pl;
	const vector<string> list;
	const vector<string> ancestry;
	const string stext;

	enum eType { Connect, Tree, Subtree, Table, Label, YearList, DescList };
	string getSessionID() { return sessionID; }
	vector<string> get_ancestry() const { return ancestry; }
	vector<string> get_list() const { return list; }
	string get_text() const { return stext; }
	vector<string> get_tree_pl() const { return tree_pl; }
	vector<vector<int>> get_tree_st() const { return tree_st; }
	vector<wstring> get_wtree_pl() const { return wtree_pl; }
	int type() const { return etype; }
	
private:
	eType etype;
	string sessionID;

	// Constructor for eType Connect.
	DataEvent(eType et, const string& sID) : etype(et), sessionID(sID) {}

	// Constructor for eType List.
	DataEvent(eType et, const string& sID, const vector<string>& lst) 
		: etype(et), sessionID(sID), list(lst) {}

	// Constructor for eTypes Tree, Table.
	DataEvent(eType et, const string& sID, const vector<vector<int>>& t_st, 
		const vector<string>& t_pl) : etype(et), sessionID(sID), tree_st(t_st), tree_pl(t_pl) {}

	// Constructors for eType Subtree.
	DataEvent(eType et, const vector<string>& anc, const vector<vector<int>>& t_st,
		const vector<string>& t_pl) : etype(et),
		sessionID(anc[0]), tree_st(t_st), tree_pl(t_pl), ancestry(anc) {}
	DataEvent(eType et, const vector<string>& anc, const vector<vector<int>>& t_st,
		const vector<wstring>& wt_pl) : etype(et), 
		sessionID(anc[0]), tree_st(t_st), wtree_pl(wt_pl), ancestry(anc) {}

	// Constructor for eType Label.
	DataEvent(eType et, const string& sID, const string& txt)
		: etype(et), sessionID(sID), stext(txt) {}

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

	bool connect(User* user, const DataEventCallback& handleEvent);
	void init(int&);
	long long getTimer();
	vector<string> getYearList();
	void pullRegion(vector<string>);
	void pullRegion2(vector<string>);
	void pullTree(string, vector<string>);
	void setYear(vector<string>);

private:
	JFUNC jf;
	SQLFUNC sf;
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

