#pragma once
#include <Wt/WServer.h>
#include "sqlfunc.h"
#include "wtfunc.h"

using namespace std;
extern const string sroot;
extern mutex m_server;
const int maxTreeLayers = 6; // Not counting the root.

class DataEvent
{
public:
	vector<vector<int>> tree_st;
	vector<string> tree_pl;

	enum eType { Connect, Tree, Table };
	string getSessionID() { return sessionID; }
	int type() const { return etype; }
	
private:
	eType etype;
	string sessionID;

	// Constructor for eType Connect.
	DataEvent(eType et, const string& sID) : etype(et), sessionID(sID) {}

	// Constructor for eTypes Tree, Table.
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
	void updateFilter(string, vector<string>);

private:
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

	void postDataEvent(const DataEvent& event, string sID);
};

