#include "SCDAserver.h"

SCDAserver::SCDAserver(Wt::WServer& wtServer) : serverRef(wtServer) {}

vector<vector<int>> SCDAserver::binMapBorder(string& tname0)
{
	string tname = tname0 + "border";
	vector<string> search = { "*" };
	vector<vector<string>> sBorder;
	sf.select(search, tname, sBorder);
	vector<vector<int>> iBorder(sBorder.size(), vector<int>(2));
	for (int ii = 0; ii < sBorder.size(); ii++)
	{
		try
		{
			iBorder[ii][0] = stoi(sBorder[ii][0]);
			iBorder[ii][1] = stoi(sBorder[ii][1]);
		}
		catch (invalid_argument) { jf.err("stoi-SCDAserver.binMapBorder"); }
	}
	return iBorder;
}
vector<vector<vector<int>>> SCDAserver::binMapFrames(string& tname0)
{
	// Returns in the form [map, scale, position][TL, BR][coords].
	vector<string> search = { "*" };
	string tname = tname0 + "frames";
	vector<vector<string>> sFrames;
	sf.select(search, tname, sFrames);
	int xy, TLBR = 0, coord1, coord2, coord3;
	vector<vector<vector<int>>> iFrames(3, vector<vector<int>>(2, vector<int>(2)));
	for (int ii = 0; ii < sFrames.size(); ii++)
	{
		xy = ii % 2;
		if (ii == 2) { TLBR = 1; }
		try
		{
			iFrames[0][TLBR][xy] = stoi(sFrames[ii][0]);
			iFrames[1][TLBR][xy] = stoi(sFrames[ii][1]);
			iFrames[2][TLBR][xy] = stoi(sFrames[ii][2]);
		}
		catch (invalid_argument) { jf.err("stoi-SCDAserver.binMapFrames"); }
	}
	return iFrames;
}
vector<double> SCDAserver::binMapPosition(string& tname0)
{
	string tname = tname0 + "position";
	vector<string> sPosition;
	vector<string> search = { "WH Fraction" };
	sf.select(search, tname, sPosition);
	vector<double> output(2);
	try
	{
		output[0] = stod(sPosition[0]);
		output[1] = stod(sPosition[1]);
	}
	catch (invalid_argument) { jf.err("stod-SCDAserver.binMapPosition"); }
	return output;
}
string SCDAserver::binMapParent(string& tname0)
{
	string tname = tname0 + "parent";
	vector<string> search = { "Region Name" };
	string sParent;
	sf.select(search, tname, sParent);
	return sParent;
}
double SCDAserver::binMapScale(string& tname0)
{
	string tname = tname0 + "scale";
	vector<string> search = { "*" };
	string sScale;
	sf.select(search, tname, sScale);
	double dScale;
	try { dScale = stod(sScale); }
	catch (invalid_argument) { jf.err("stod-SCDAserver.binMapScale"); }
	return dScale;
}
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
	//sf.all_tables(numTables);
}
long long SCDAserver::getTimer()
{
	return jf.timerStop();
}
vector<string> SCDAserver::getYearList()
{
	return sf.select_years();
}
void SCDAserver::pullMap(vector<string> prompt)
{
	// prompt has form [sessionID, cata desc, parent region name].
	string cataName = prompt[1];  // cataDesc->cataName
	vector<string> geoLayers = { prompt[2], prompt[3] };  // regionName->geoLayers, window dimensions.
	vector<vector<string>> cataGeo;
	vector<Wt::WPainterPath> wpPaths(1);
	wpPaths[0] = pullMapParent(cataName, geoLayers, cataGeo);


	postDataEvent(DataEvent(DataEvent::Map, prompt[0], wpPaths), prompt[0]);
}
Wt::WPainterPath SCDAserver::pullMapParent(string& cataDesc, vector<string>& geoLayers, vector<vector<string>>& cataGeo)
{
	// Get the catalogue name.
	vector<string> search = { "Name" };
	string tname = "TCatalogueIndex", cataName, parentLayer;
	vector<string> conditions = { "Description = '" + cataDesc + "'" };
	sf.select(search, tname, cataName, conditions);
	cataDesc = cataName;

	// Get the catalogue's geo layers.
	string regionName = geoLayers[0];
	string windowWH = geoLayers[1];
	geoLayers.clear();
	search = { "Layer" };
	tname = cataName + "$Geo_Layers";
	sf.select(search, tname, geoLayers);

	// Load the catalogue's geo file, and build the parent's TMap template.
	search = { "*" };
	tname = cataName + "$Geo";
	sf.select(search, tname, cataGeo);
	for (int ii = 0; ii < cataGeo.size(); ii++)
	{
		if (cataGeo[ii][1] == regionName)
		{
			parentLayer = cataGeo[ii][2];
			break;
		}
	}
	int indexGL;
	for (int ii = 0; ii < geoLayers.size(); ii++)
	{
		if (geoLayers[ii] == parentLayer) { indexGL = ii; break; }
	}
	string tname0 = "TMap$";
	for (int ii = 1; ii <= indexGL; ii++)
	{
		tname0 += geoLayers[ii] + "$";
	}
	tname0 += regionName + "$";

	// Load the parent's bin data.
	vector<vector<vector<int>>> frames = binMapFrames(tname0);
	double scale = binMapScale(tname0);
	vector<double> position = binMapPosition(tname0);
	vector<vector<int>> border = binMapBorder(tname0);

	// Create a WPainterPath to fit the painter widget.
	vector<double> windowDim = jf.destringifyCoordD(windowWH);  // Unit is pixels.
	Wt::WPainterPath wpPath;
	wtf.makeWPPath(frames[0], border, windowDim, wpPath);
	return wpPath;
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
		sf.sclean(prompt[2], 1);  // Double apostraphes.
		conditions = { "Description LIKE '" + prompt[2] + "'" };
		sf.select(search, tname, sname, conditions);
		if (sname.size() < 1) { jf.err("select-SCDAserver.pullLayer"); }
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
