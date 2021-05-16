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
void SCDAserver::loadMap(vector<string> prompt)
{
	// prompt has form [sessionID].

	//postDataEvent(DataEvent(DataEvent::Table, prompt[0], theTable), prompt[0]);
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
void SCDAserver::pullLayer(int layer, vector<string> prompt)
{
	// prompt form [sessionID, syear, sdesc, sregion, sdiv, ...] as applicable.
	jf.timerStart();
	string tname, sname, gid;
	wstring wtemp;
	vector<string> search, conditions, yearList, descList, regionList, divRow;
	vector<wstring> ancestry;
	vector<vector<wstring>> results;
	int maxCol;

	switch (layer)
	{
	case 0:  // set Root
		yearList = sf.select_years();
		postDataEvent(DataEvent(DataEvent::RootLayer, prompt[0], yearList), prompt[0]);
		break;
	case 1:  // set Year
		search = { "Description" };
		tname = "TCatalogueIndex";
		conditions = { "Year = " + prompt[1] };
		sf.select(search, tname, descList, conditions);
		postDataEvent(DataEvent(DataEvent::YearLayer, prompt[0], descList), prompt[0]);
		break;
	case 2:  // set Desc  NOTE: this layer returns the first TWO layers of regions.
	{
		search = { "Name" };
		tname = "TCatalogueIndex";
		conditions = { "Description = '" + prompt[2] + "'" };
		sf.select(search, tname, sname, conditions);
		tname = "TG_Region$" + sname;
		search = { "GID", "[Region Name]", "param2" };
		conditions = { "param3 IS NULL" };
		sf.select(search, tname, results, conditions);
		jf.removeBlanks(results);
		postDataEvent(DataEvent(DataEvent::DescLayer, prompt[0], results), prompt[0]);
		break;
	}
	case 3:  // set Region   NOTE: this layer returns the remainder of TGRegion.
	{
		search = { "Name" };
		tname = "TCatalogueIndex";
		conditions = { "Description = '" + prompt[2] + "'" };
		sf.select(search, tname, sname, conditions);
		
		tname = "TG_Region$" + sname;
		maxCol = sf.get_num_col(tname);

		search = { "GID" };  
		conditions = { "[Region Name] = '" + prompt[3] + "'" };
		sf.select(search, tname, gid, conditions);

		search = { "GID", "[Region Name]" };
		conditions = { "param3 = " + gid };
		for (int ii = 4; ii < maxCol; ii++)
		{
			search.push_back("param" + to_string(ii));
		}
		sf.select(search, tname, results, conditions);
		jf.removeBlanks(results);
		postDataEvent(DataEvent(DataEvent::RegionLayer, prompt[0], results), prompt[0]);
		break;
	}
	case 4:  // set Division   NOTE: this layer returns the remainder of TGRegion.
	{
		search = { "Name" };
		tname = "TCatalogueIndex";
		conditions = { "Description = '" + prompt[2] + "'" };
		sf.select(search, tname, sname, conditions);

		tname = "TG_Region$" + sname;
		maxCol = sf.get_num_col(tname);

		search = { "*" };
		conditions = { "[Region Name] = '" + prompt[3] + "'" };
		sf.select(search, tname, divRow, conditions);

		search = { "[Region Name]" };
		jf.removeBlanks(divRow);
		for (int ii = 2; ii < divRow.size(); ii++)
		{
			conditions = { "GID = " + divRow[ii] };
			sf.select(search, tname, wtemp, conditions);
			ancestry.push_back(wtemp);
		}
		ancestry.push_back(jf.utf8to16(prompt[3]));

		postDataEvent(DataEvent(DataEvent::DivLayer, prompt[0], ancestry), prompt[0]);
		break;
	}
	}
	long long timer = jf.timerStop();
	jf.logTime("pullLayer(" + to_string(layer) + ")", timer);
}
void SCDAserver::pullRegion(vector<string> prompt)
{
	// prompt has form [sessionID, cata desc, region name].
	vector<string> search = { "Name" };
	string tname = "TCatalogueIndex";
	string cataName;
	vector<string> conditions = { "Description = '" + prompt[1] + "'" };
	sf.select(search, tname, cataName, conditions);

	string gid;
	search = { "GID" };
	tname = "TG_Region$" + cataName;
	sf.makeANSI(prompt[2]);
	conditions = { "[Region Name] = '" + prompt[2] + "'" };
	sf.select(search, tname, gid, conditions);

	tname = cataName + "$" + gid;
	vector<vector<wstring>> theTable(1, vector<wstring>());
	sf.get_col_titles(tname, theTable[0]);
	search = { "*" };
	sf.select(search, tname, theTable);
	postDataEvent(DataEvent(DataEvent::Table, prompt[0], theTable), prompt[0]);
}
void SCDAserver::pullTable(vector<string> prompt)
{
	// prompt has form [sessionID, table name].
	vector<vector<wstring>> theTable(1, vector<wstring>());
	sf.get_col_titles(prompt[1], theTable[0]);
	vector<string> search = { "GID", "[Region Name]", "param2", "param3" };
	vector<string> conditions = { "param3 IS NULL" };
	sf.select(search, prompt[1], theTable, conditions);
	postDataEvent(DataEvent(DataEvent::Table, prompt[0], theTable), prompt[0]);
}

/*
void SCDAserver::setDesc(vector<string> ancestry)
{
	// ancestry has form [sessionID, syear, sdesc].
	jf.timerStart();
	
	// Get the catalogue name.
	string sname;
	vector<string> search = { "Name" };
	string tname = "TCatalogueIndex";
	vector<string> conditions = { "Description = " + ancestry[2] };
	sf.select(search, tname, sname, conditions);
	
	// Get the first two layers from that catalogue's region tree.
	tname = "TG_Region$" + sname;
	search = { "[Region Name]" };
	conditions = { "param3 = NULL" };
	wstring wtemp = jf.utf8to16(ancestry[2]);
	vector<wstring> regions = { wtemp };
	sf.select(search, tname, regions, conditions);

	postDataEvent(DataEvent(DataEvent::RegionList, ancestry[0], regions), ancestry[0]);
	long long timer = jf.timerStop();  // RESUME HERE. Should this be a list or subtree?
	cout << "server setDesc: " << timer << "ms" << endl;
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
*/
