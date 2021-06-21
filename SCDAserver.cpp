#include "SCDAserver.h"

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
	vector<double> output = { -1.0, -1.0 };
	if (sPosition.size() == 1) { return output; }
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
void SCDAserver::init()
{
	sf.init(db_path);
	auto uniqueWtf = make_unique<WTFUNC>(vsTemp);
	wtf = uniqueWtf.get();
}
string SCDAserver::getLinearizedColTitle(string& sCata, string& rowTitle, string& colTitle)
{
	size_t pos1 = rowTitle.find_first_not_of('+');
	string rowStub = rowTitle.substr(pos1), temp = "'", LCT, param;
	vector<string> vsTemp, search = {"*"}, params;
	jf.clean(rowStub, vsTemp, temp);
	string tname = "TG_Row$" + sCata;
	vector<string> conditions = { "\"Row Title\" LIKE " + rowStub };
	sf.select(search, tname, vsTemp, conditions);
	while (vsTemp.back() == "") { vsTemp.pop_back(); }
	search = { "Row Title" };
	for (int ii = 2; ii < vsTemp.size(); ii++)
	{
		param.clear();
		conditions = { "\"Row Index\" = " + vsTemp[ii] };
		sf.select(search, tname, param, conditions);
		temp.clear();
		for (int jj = 0; jj < params.size(); jj++)
		{
			temp += "+";
		}
		params.push_back(temp + param);
	}
	temp.clear();
	for (int jj = 0; jj < params.size(); jj++)
	{
		temp += "+";
	}
	params.push_back(temp + rowStub);
	for (int ii = 0; ii < params.size(); ii++)
	{
		LCT += params[ii] + "@";
	}
	LCT += colTitle;
	return LCT;
}
void SCDAserver::getSmallGeo(vector<vector<string>>& smallGeo, string cataName)
{
	// Adds the geo data for the given parent's immediate children.

	// Obtain the gid ancestry list for the children.
	vector<string> gidRow, gidList, vsResult;
	vector<string> search = { "*" };
	string temp, tname = "TG_Region$" + cataName;
	vector<string> conditions = { "[Region Name] LIKE " + smallGeo[0][1] };
	sf.select(search, tname, vsResult, conditions);
	while (vsResult.back() == "") { vsResult.pop_back(); }
	gidRow.resize(vsResult.size() - 1);
	for (int ii = 2; ii < vsResult.size(); ii++)
	{
		gidRow[ii - 2] = vsResult[ii];
	}
	gidRow[gidRow.size() - 1] = vsResult[0];

	// Get the children's GIDs.
	vector<vector<string>> vvsResult;
	conditions.clear();
	for (int ii = 0; ii < gidRow.size(); ii++)
	{
		temp.clear();
		if (ii > 0) { temp += " AND "; }
		temp += "param" + to_string(ii + 2) + " LIKE " + gidRow[ii];
		conditions.push_back(temp);
	}
	sf.select(search, tname, vvsResult, conditions);
	for (int ii = 0; ii < vvsResult.size(); ii++)
	{
		while (vvsResult[ii].back() == "") { vvsResult[ii].pop_back(); }
		if (vvsResult[ii].size() == gidRow.size() + 2)
		{
			gidList.push_back(vvsResult[ii][0]);
		}
	}

	// Get the children's geo rows.
	tname = cataName + "$Geo";
	for (int ii = 0; ii < gidList.size(); ii++)
	{
		conditions = { "GID LIKE " + gidList[ii] };
		vsResult.clear();
		sf.select(search, tname, vsResult, conditions);
		smallGeo.push_back(vsResult);
	}
	int bbq = 1;
}
long long SCDAserver::getTimer()
{
	return jf.timerStop();
}
vector<string> SCDAserver::getYearList()
{
	return sf.selectYears();
}
void SCDAserver::pullMap(vector<string> prompt)
{
	// prompt has form [sessionID, cata desc, parent region name, sWindowDim, rowTitle, columnTitle].
	string temp, cataName = prompt[1];  // cataDesc->cataName
	vector<string> geoLayers = { prompt[2], prompt[3] };  // regionName->geoLayers, window dimensions.
	vector<string> vsTemp;
	size_t pos1, pos2;
	int inum;
	vector<vector<string>> smallGeo, TMI;

	// Load the border coordinates for the parent and children.
	vector<vector<Wt::WPointF>> areas(1);
	vector<double> regionData, mapScaling;  // Form [parentRatio, parentScale, parentWindowWidth, parentWindowHeight].
	areas[0] = pullMapParent(cataName, geoLayers, smallGeo, mapScaling);
	getSmallGeo(smallGeo, cataName);
	if (smallGeo[0][2] == "canada" && smallGeo[1][2] == "province")
	{
		for (int ii = 1; ii < smallGeo.size(); ii++)
		{
			smallGeo[ii][1] += "(Canada)";
		}
	}
	areas.resize(smallGeo.size());
	vector<string> sIDregion(areas.size() + 1), conditions;  
	sIDregion[0] = prompt[0];
	sIDregion[1] = smallGeo[0][1];  // Parent name.
	for (int ii = 1; ii < areas.size(); ii++)
	{
		sIDregion[ii + 1] = smallGeo[ii][1];
		areas[ii] = pullMapChild(geoLayers, smallGeo, ii, mapScaling);
	}

	// Load the requested table value for the parent and children.
	vector<string> search = { getLinearizedColTitle(cataName, prompt[4], prompt[5]) };
	sIDregion.push_back(search[0]);  // For the legend box.
	regionData.resize(smallGeo.size());
	for (int ii = 0; ii < smallGeo.size(); ii++)
	{
		temp.clear();
		conditions = { "GID = " + smallGeo[ii][0] };
		sf.select(search, cataName, temp, conditions);
		if (temp == "")
		{
			regionData[ii] = -1.0;
			continue;
		}
		pos1 = temp.find('.');
		if (pos1 < temp.size())
		{
			try { regionData[ii] = stod(temp); }
			catch (invalid_argument) { jf.err("stod-SCDAserver.pullMap"); }
		}
		else
		{
			try { inum = stoi(temp); }
			catch (invalid_argument) { jf.err("stoi-SCDAserver.pullMap"); }
			regionData[ii] = (double)inum;
		}
	}
	postDataEvent(DataEvent(DataEvent::Map, sIDregion, areas, regionData), prompt[0]);
}
vector<Wt::WPointF> SCDAserver::pullMapChild(vector<string>& geoLayers, vector<vector<string>>& smallGeo, int myIndex, vector<double>& mapScaling)
{
	// Build the tMap template.
	int indexLayer;
	string tname0;
	for (int ii = 0; ii < geoLayers.size(); ii++)
	{
		if (geoLayers[ii] == smallGeo[myIndex][2])
		{
			indexLayer = ii;
			break;
		}
	}
	tname0 = "TMap$";
	for (int ii = 1; ii <= indexLayer; ii++)
	{
		tname0 += geoLayers[ii] + "$";
	}
	tname0 += smallGeo[myIndex][1] + "$";

	// Load the bin data.
	vector<vector<vector<int>>> frames = binMapFrames(tname0);
	vector<double> position = binMapPosition(tname0);
	vector<vector<int>> border = binMapBorder(tname0);
	double myScale = binMapScale(tname0);
	position.push_back(mapScaling[0] * mapScaling[1] / myScale);
	if (position[0] >= 0.0)
	{
		position[0] *= mapScaling[2];
		position[1] *= mapScaling[3];
	}
	else
	{
		position[0] = 0.0;
		position[1] = 0.0;
	}

	// Create a WPainterPath to fit the painter widget.
	vector<Wt::WPointF> area = wtf->makeWPPath(frames[0], border, position);
	return area;
}
vector<Wt::WPointF> SCDAserver::pullMapParent(string& cataDesc, vector<string>& geoLayers, vector<vector<string>>& smallGeo, vector<double>& mapScaling)
{
	// Get the catalogue name.
	vector<string> search = { "Name" }, vsResult;
	string tname = "TCatalogueIndex", cataName;
	vector<string> conditions = { "Description LIKE " + cataDesc };
	sf.select(search, tname, cataName, conditions);
	cataDesc = cataName;

	// Get the catalogue's geo layers.
	string regionName = geoLayers[0], gid;
	string windowWH = geoLayers[1];
	geoLayers.clear();
	search = { "Layer" };
	tname = cataName + "$Geo_Layers";
	sf.select(search, tname, geoLayers);

	// Export the parent's row in smallGeo. 
	search = { "*" };
	tname = cataName + "$Geo";
	conditions = { "\"Region Name\" LIKE " + regionName };
	smallGeo.resize(1, vector<string>());
	sf.select(search, tname, smallGeo[0], conditions);
	int indexGL;
	for (int ii = 0; ii < geoLayers.size(); ii++)
	{
		if (geoLayers[ii] == smallGeo[0][2]) { indexGL = ii; break; }
	}
	if (indexGL == geoLayers.size() - 1)  // If this parent region has no children...
	{
		if (geoLayers[indexGL] == "province" && indexGL == 1)  // Display the Canada->province(Canada) map.
		{
			smallGeo[0].clear();
			regionName = "Canada";
			conditions = { "\"Region Name\" LIKE " + regionName };
			sf.select(search, tname, smallGeo[0], conditions);
			indexGL--;
		}
		else if (indexGL > 1)
		{
			pullMapParentBackspace(smallGeo, cataName);
			regionName = smallGeo[0][1];
			indexGL--;
		}
		else { jf.err("No children, no parent-SCDAserver.pullMapParent"); }
	}
	string tname0 = "TMap$";
	for (int ii = 1; ii <= indexGL; ii++)
	{
		tname0 += geoLayers[ii] + "$";
	}
	tname0 += regionName + "$";

	// Load the parent's bin data.
	vector<vector<vector<int>>> frames = binMapFrames(tname0);
	vector<vector<int>> border = binMapBorder(tname0);
	mapScaling.resize(4);  // Form [ratio, PPKM, parentWidth, parentHeight].
	mapScaling[1] = binMapScale(tname0);
	mapScaling[2] = (double)(frames[0][1][0] - frames[0][0][0]);
	mapScaling[3] = (double)(frames[0][1][1] - frames[0][0][1]);

	// Create a WPainterPath to fit the painter widget.
	vector<double> windowDim = jf.destringifyCoordD(windowWH);  // Unit is pixels.
	vector<Wt::WPointF> area = wtf->makeWPPath(frames[0], border, windowDim);	
	if (windowDim.size() != 1) { jf.err("windowDim-SCDAserver.pullMapParent"); }
	mapScaling[0] = windowDim[0];
	mapScaling[2] *= mapScaling[0];
	mapScaling[3] *= mapScaling[0];
	return area;
}
void SCDAserver::pullMapParentBackspace(vector<vector<string>>& smallGeo, string cataName)
{
	string gidChild = smallGeo[0][0];
	smallGeo[0].clear();
	string tname = "TG_Region$" + cataName;
	vector<string> search = { "*" };
	vector<string> conditions = { "GID = " + gidChild };
	vector<string> vsResult;
	sf.select(search, tname, vsResult, conditions);
	if (vsResult.size() < 2) { jf.err("TG_Region row not found-SCDAserver.pullMapParentBackspace"); }
	while (vsResult.back() == "") { vsResult.pop_back(); }
	string gidParent = vsResult.back();
	tname = cataName + "$Geo";
	conditions = { "GID = " + gidParent };
	sf.select(search, tname, smallGeo[0], conditions);
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
	vector<string> colTitles;
	vector<wstring> ancestry;
	vector<vector<wstring>> results;
	int maxCol;
	long long timer;

	switch (layer)
	{
	case 0:  // set Root
		// prompt has form [sID].
		yearList = sf.selectYears();
		postDataEvent(DataEvent(DataEvent::RootLayer, prompt[0], yearList), prompt[0]);
		break;
	case 1:  // set Year
	{
		// prompt has form [sID, sYear].
		search = { "Description" };
		tname = "TCatalogueIndex";
		conditions = {
			"Year LIKE " + prompt[1],
			" AND Description NOT LIKE Incomplete" };
		sf.select(search, tname, descList, conditions);
		postDataEvent(DataEvent(DataEvent::YearLayer, prompt[0], descList), prompt[0]);
		break;
	}
	case 2:  // set Desc  NOTE: this layer returns the first TWO layers of regions.
	{
		// prompt has form [sID, sYear, sDesc].
		search = { "Name" };
		tname = "TCatalogueIndex";
		sf.sclean(prompt[2], 1);  // Double apostraphes.
		conditions = { "Description LIKE " + prompt[2] };
		sf.select(search, tname, sname, conditions);
		if (sname.size() < 1) { jf.err("select-SCDAserver.pullLayer"); }
		tname = "TG_Region$" + sname;
		sf.get_col_titles(tname, colTitles);
		if (colTitles.size() == 2)
		{
			search = { "GID", "Region Name"};
			sf.select(search, tname, results);
		}
		else if (colTitles.size() == 3)
		{
			search = { "GID", "Region Name", "param2" };
			sf.select(search, tname, results);
		}
		else if (colTitles.size() > 3)
		{
			search = { "GID", "Region Name", "param2" };
			conditions = { "param3 IS NULL" };
			sf.select(search, tname, results, conditions);
		}
		else { jf.err("colTitles-SCDAserver.pullLayer"); }
		jf.removeBlanks(results);
		postDataEvent(DataEvent(DataEvent::DescLayer, prompt[0], results), prompt[0]);
		break;
	}
	case 3:  // set Region   NOTE: this layer returns the remainder of TGRegion.
	{
		search = { "Name" };
		tname = "TCatalogueIndex";
		conditions = { "Description LIKE " + prompt[2] };
		sf.select(search, tname, sname, conditions);
		
		tname = "TG_Region$" + sname;
		maxCol = sf.get_num_col(tname);
		if (maxCol < 4)
		{
			results.clear();
			postDataEvent(DataEvent(DataEvent::RegionLayer, prompt[0], results), prompt[0]);
			break;
		}
		search = { "GID" };  
		conditions = { "[Region Name] LIKE " + prompt[3] };
		sf.select(search, tname, gid, conditions);

		search = { "GID", "Region Name" };
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
		conditions = { "Description LIKE " + prompt[2] };
		sf.select(search, tname, sname, conditions);

		tname = "TG_Region$" + sname;
		maxCol = sf.get_num_col(tname);

		search = { "*" };
		conditions = { "[Region Name] LIKE " + prompt[3] };
		sf.select(search, tname, divRow, conditions);

		search = { "Region Name" };
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
	timer = jf.timerStop();
	jf.logTime("pullLayer(" + to_string(layer) + ")", timer);
}
void SCDAserver::pullList(vector<string> prompt)
{
	// prompt has form [sessionID, table name].
	vector<string> sList;
	if (prompt[1] == "all")
	{
		sf.all_tables(sList);
	}
	else 
	{
		sList = sf.getTableList(prompt[1]);
	}
	postDataEvent(DataEvent(DataEvent::List, prompt[0], sList), prompt[0]);
}
void SCDAserver::pullRegion(vector<string> prompt)
{
	// prompt has form [sessionID, cata desc, region name].
	vector<string> search = { "Name" };
	string tname = "TCatalogueIndex";
	string cataName;
	vector<string> conditions = { "Description LIKE " + prompt[1] };
	sf.select(search, tname, cataName, conditions);

	string gid;
	search = { "GID" };
	tname = "TG_Region$" + cataName;
	string temp = prompt[2];
	sf.sclean(temp, 1);
	conditions = { "[Region Name] LIKE " + temp };
	sf.select(search, tname, gid, conditions);

	tname = cataName + "$" + gid;
	vector<vector<wstring>> theTable(1, vector<wstring>());
	vector<string> colTitles;
	sf.get_col_titles(tname, colTitles);
	theTable[0].resize(colTitles.size());
	for (int ii = 0; ii < colTitles.size(); ii++)
	{
		theTable[0][ii] = jf.utf8to16(colTitles[ii]);
	}
	search = { "*" };
	sf.select(search, tname, theTable);
	postDataEvent(DataEvent(DataEvent::Table, prompt[0], theTable), prompt[0]);
}
void SCDAserver::pullTable(vector<string> prompt)
{
	// prompt has form [sessionID, table name].
	vector<vector<wstring>> theTable;
	vector<string> colTitles;
	sf.get_col_titles(prompt[1], colTitles);
	vector<string> search = { "*" };
	sf.select(search, prompt[1], theTable);
	postDataEvent(DataEvent(DataEvent::Table, prompt[0], theTable), prompt[0]);
}

