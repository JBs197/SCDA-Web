#pragma once
#include <Wt/WServer.h>
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
	const vector<string> tree_pl;
	const vector<string> list;

	enum eType { Connect, Tree, Subtree, List, Table };
	string getSessionID() { return sessionID; }
	vector<string> get_list() const { return list; }
	vector<string> get_tree_pl() const { return tree_pl; }
	vector<vector<int>> get_tree_st() const { return tree_st; }
	int type() const { return etype; }
	
private:
	eType etype;
	string sessionID;

	// Constructor for eType Connect.
	DataEvent(eType et, const string& sID) : etype(et), sessionID(sID) {}

	// Constructor for eType List.
	DataEvent(eType et, const string& sID, const vector<string>& lst) 
		: etype(et), sessionID(sID), list(lst) {}

	// Constructor for eTypes Tree, Subtree, Table.
	DataEvent(eType et, const string& sID, const vector<vector<int>>& t_st, 
		const vector<string>& t_pl) : etype(et), sessionID(sID), tree_st(t_st), tree_pl(t_pl) {}

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
	vector<string> getYearList();
	void pullRegion(string, string);
	void pullTree(string, vector<string>);

private:
	JFUNC jf;
	SQLFUNC sf;
	struct UserInfo
	{
		string sessionID;
		DataEventCallback eventCallback;
	};

	const string db_path = sroot + "\\SCDA.db";
	Wt::WServer& serverRef;
	typedef map<User*, UserInfo> userMap;
	userMap users;

	void err(string);
	void postDataEvent(const DataEvent& event, string sID);
};

