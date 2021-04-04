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
void SCDAserver::err(string func)
{
	jf.err(func);
}
void SCDAserver::init(int& numTables)
{
	sf.init(db_path);
	sf.all_tables(numTables);
}
long long SCDAserver::getTimer()
{
	return jf.timerStop();
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
void SCDAserver::pullRegion(vector<string> ancestry)
{
	jf.timerStart();
	// ancestry has form [sessionID, syear, sdesc].
	vector<string> cataNames;
	vector<string> search = { "Name" };
	string tname = "TCatalogueIndex";
	vector<string> conditions = { "Description = '" + ancestry[2] + "'" };
	sf.select(search, tname, cataNames, conditions);
	if (cataNames.size() != 1) { err("Not-one names returned from desc-SCDAserver.pullRegion"); }

	vector<vector<int>> tree_st;
	vector<wstring> wtree_pl;
	tname = "TG_Region$" + cataNames[0];
	sf.select_tree(tname, tree_st, wtree_pl);
	postDataEvent(DataEvent(DataEvent::Subtree, ancestry, tree_st, wtree_pl), ancestry[0]);
	long long timer = jf.timerStop();
	cout << "pullRegion: " << timer << "ms" << endl;
}
void SCDAserver::pullRegion2(vector<string> ancestry)
{
	jf.timerStart();  // THIS FUNCTION IS TBD: make it to reduce catalogue load times.
	// ancestry has form [sessionID, syear, sdesc].
	vector<string> cataNames;
	vector<string> search = { "Name" };
	string tname = "TCatalogueIndex";
	vector<string> conditions = { "Description = '" + ancestry[2] + "'" };
	sf.select(search, tname, cataNames, conditions);
	if (cataNames.size() != 1) { err("Not-one names returned from desc-SCDAserver.pullRegion"); }

	vector<vector<int>> tree_st;
	vector<wstring> wtree_pl;
	tname = "TG_Region$" + cataNames[0];
	sf.select_tree(tname, tree_st, wtree_pl);
	postDataEvent(DataEvent(DataEvent::Subtree, ancestry, tree_st, wtree_pl), ancestry[0]);
	long long timer = jf.timerStop();
	cout << "pullRegion: " << timer << "ms" << endl;
}
void SCDAserver::pullTree(string sID, vector<string> filters)
{
	// Get the list of years to display.
	vector<string> tree_pl;
	tree_pl.push_back("Census Tables");
	vector<vector<int>> tree_st;
	tree_st.push_back({ 0 });
	vector<string> yearsFiltered;
	if (filters[0] == "All")
	{
		yearsFiltered = sf.select_years();
	}
	else
	{
		yearsFiltered.push_back(filters[0]);
	}
	int numYears = yearsFiltered.size();
	tree_pl.resize(1 + numYears);
	tree_st.resize(1 + numYears);
	for (int ii = 0; ii < numYears; ii++)
	{
		tree_pl[1 + ii] = yearsFiltered[ii];
		tree_st[1 + ii] = { 0, -1 * (ii + 1) };
		tree_st[0].push_back(1 + ii);
	}

	// Add the list of catalogue descriptions to the tree.
	string syear;
	vector<vector<string>> cataNameDesc;  // Form [cata index][cata name, cata desc].
	vector<string> search = { "Name", "Description" };
	string tname = "TCatalogueIndex";
	vector<string> conditions(1);
	vector<int> ivec;
	int numCatas = 0;
	int nextIndex = 1 + numYears;
	for (int ii = 1; ii <= numYears; ii++)  // Do not include the root.
	{
		syear = tree_pl[ii];  // ii is the tree index of the parent year node.
		conditions[0] = "Year = " + syear;
		cataNameDesc.clear();
		sf.select(search, tname, cataNameDesc, conditions);
		numCatas += cataNameDesc.size();
		tree_pl.resize(1 + numYears + numCatas);
		tree_st.resize(1 + numYears + numCatas);
		ivec = tree_st[ii];
		ivec[ivec.size() - 1] *= -1;
		for (int jj = 0; jj < cataNameDesc.size(); jj++)  // jj is the catalogue index.
		{
			tree_pl[nextIndex + jj] = cataNameDesc[jj][1];
			tree_st[nextIndex + jj] = ivec;
			tree_st[nextIndex + jj].push_back(-1 * (nextIndex + jj));
			tree_st[ii].push_back(nextIndex + jj);
		}
		nextIndex = 1 + numYears + numCatas;
	}

	postDataEvent(DataEvent(DataEvent::Tree, sID, tree_st, tree_pl), sID);
}
void SCDAserver::setYear(vector<string> ancestry)
{
	jf.timerStart();
	// ancestry has form [sessionID, syear].
	vector<string> results = { ancestry[1] };
	vector<string> search = { "Description" };
	string tname = "TCatalogueIndex";
	vector<string> conditions = { "Year = " + ancestry[1] };
	sf.select(search, tname, results, conditions);
	postDataEvent(DataEvent(DataEvent::DescList, ancestry[0], results), ancestry[0]);
	long long timer = jf.timerStop();
	cout << "server setYear: " << timer << "ms" << endl;
}
