#include "SCDAserver.h"

SCDAserver::SCDAserver(Wt::WServer& wtServer) : serverRef(wtServer) {}

bool SCDAserver::connect(User* user, const DataEventCallback& handleEvent)
{
	lock_guard<mutex> lock(m_server);
	if (users.count(user) == 0)
	{
		UserInfo userInfo;
		userInfo.sessionID = Wt::WApplication::instance()->sessionId();
		userInfo.eventCallback = handleEvent;
		users[user] = userInfo;
		return 1;
	}
	else { return 0; }
}
void SCDAserver::init(int& numTables)
{
	sf.init(db_path);
	sf.all_tables(numTables);
}
vector<string> SCDAserver::getYearList()
{
	return sf.select_years();
}
void SCDAserver::postDataEvent(const DataEvent& event, string sID)
{
	lock_guard<mutex> lock(m_server);
	string temp;
	for (userMap::const_iterator ii = users.begin(); ii != users.end(); ii++)
	{
		temp = ii->second.sessionID;
		if (temp == sID)
		{
			serverRef.post(sID, bind(ii->second.eventCallback, event));
			break;
		}
	}
}
void SCDAserver::updateFilter(string sID, vector<string> filters)
{
	lock_guard<mutex> lock(m_server);
	Wt::WApplication* app = Wt::WApplication::instance();

	// Get the list of years to display.
	vector<string> tree_pl;
	tree_pl.push_back("Census Tables");
	vector<vector<int>> tree_st;
	tree_st.push_back({ 0 });
	vector<string> results1;
	if (filters[0] == "All")
	{
		results1 = sf.select_years();
		tree_pl.resize(1 + results1.size());
		tree_st.resize(1 + results1.size());
		for (int ii = 0; ii < results1.size(); ii++)
		{
			tree_pl[1 + ii] = results1[ii];
			tree_st[1 + ii] = { 0, ii + 1 };
		}
	}
	else 
	{
		tree_pl.push_back(filters[0]); 
		tree_st.push_back({ 0,1 });
	}

	postDataEvent(DataEvent(DataEvent::Tree, sID, tree_st, tree_pl), sID);
}
