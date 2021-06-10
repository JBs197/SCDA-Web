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
void SCDAserver::init(int& numTables)
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
	vector<string> conditions = { "\"Row Title\" LIKE '" + rowStub + "'" };
	sf.select(search, tname, vsTemp, conditions);
	while (vsTemp.back() == "") { vsTemp.pop_back(); }
	search = { "Row Title" };
	for (int ii = 2; ii < vsTemp.size(); ii++)
	{
		param.clear();
		conditions = { "\"Row Index\" LIKE '" + vsTemp[ii] + "'" };
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
vector<vector<string>> SCDAserver::getSmallGeo(vector<vector<string>>& cataGeo, string sParent)
{
	// Returns the geo data for the parent and its immediate children.
	string layerParent, layerChild;
	int indexParent = -1, index = 0, smallSize;
	vector<int> indexChildren;
	for (int ii = 0; ii < cataGeo.size(); ii++)
	{
		if (cataGeo[ii][1] == sParent)
		{
			layerParent = cataGeo[ii][2];
			indexParent = ii;
			if (ii < cataGeo.size() - 1) { layerChild = cataGeo[ii + 1][2]; }
		}
		else if (indexParent >= 0)
		{
			if (cataGeo[ii][2] == layerChild)
			{
				indexChildren.push_back(ii);
			}
			else if (cataGeo[ii][2] == layerParent) { break; }
		}
	}
	smallSize = indexChildren.size() + 1;
	vector<vector<string>> smallGeo(smallSize, vector<string>(3));
	smallGeo[0][0] = cataGeo[indexParent][0];
	smallGeo[0][1] = cataGeo[indexParent][1];
	smallGeo[0][2] = cataGeo[indexParent][2];
	for (int ii = 1; ii < smallSize; ii++)
	{
		smallGeo[ii][0] = cataGeo[indexChildren[index]][0];
		smallGeo[ii][1] = cataGeo[indexChildren[index]][1];
		smallGeo[ii][2] = cataGeo[indexChildren[index]][2];
		index++;
	}
	return smallGeo;
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
	vector<vector<string>> cataGeo, smallGeo, TMI;

	// Load the border coordinates for the parent and children.
	vector<vector<Wt::WPointF>> areas(1);
	vector<double> regionData, mapScaling;  // Form [parentRatio, parentScale, parentWindowWidth, parentWindowHeight].
	areas[0] = pullMapParent(cataName, geoLayers, cataGeo, mapScaling);
	smallGeo = getSmallGeo(cataGeo, prompt[2]);
	if (prompt[2] == "Canada")
	{
		for (int ii = 1; ii < smallGeo.size(); ii++)
		{
			smallGeo[ii][1] += "(Canada)";
		}
	}
	areas.resize(smallGeo.size());
	vector<string> sIDregion(areas.size() + 1), conditions;  
	sIDregion[0] = prompt[0];
	sIDregion[1] = prompt[2];  // Parent name.
	for (int ii = 1; ii < areas.size(); ii++)
	{
		if (smallGeo[ii].size() == 3) { sIDregion[ii + 1] = smallGeo[ii][1]; }
		else if (smallGeo[ii].size() == 1)
		{
			pos1 = smallGeo[ii][0].rfind('$') + 1;
			sIDregion[ii + 1] = smallGeo[ii][0].substr(pos1);
		}
		else { jf.err("smallGeo-SCDAserver.pullMap"); }
		areas[ii] = pullMapChild(geoLayers, smallGeo, ii, mapScaling);
	}

	// Load the requested table value for the parent and children.
	vector<string> search = { getLinearizedColTitle(cataName, prompt[4], prompt[5]) };
	sIDregion.push_back(search[0]);  // For the legend box.
	regionData.resize(smallGeo.size());
	for (int ii = 0; ii < smallGeo.size(); ii++)
	{
		temp.clear();
		conditions = { "GID LIKE " + smallGeo[ii][0] };
		sf.select(search, cataName, temp, conditions);
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
	if (smallGeo[myIndex].size() == 3)
	{
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
	}
	else if (smallGeo[myIndex].size() == 1)
	{
		tname0 = smallGeo[myIndex][0] + "$";
	}
	else { jf.err("smallGeo-SCDAserver.pullMapChild"); }

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
vector<Wt::WPointF> SCDAserver::pullMapParent(string& cataDesc, vector<string>& geoLayers, vector<vector<string>>& cataGeo, vector<double>& mapScaling)
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
	long long timer;

	switch (layer)
	{
	case 0:  // set Root
		//yearList = sf.selectYears();
		yearList = sf.selectYears();
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
		search = { "GID", "Region Name", "param2" };
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
		conditions = { "Description = '" + prompt[2] + "'" };
		sf.select(search, tname, sname, conditions);

		tname = "TG_Region$" + sname;
		maxCol = sf.get_num_col(tname);

		search = { "*" };
		conditions = { "[Region Name] = '" + prompt[3] + "'" };
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
	vector<string> search = { "GID", "Region Name", "param2", "param3" };
	vector<string> conditions = { "param3 IS NULL" };
	sf.select(search, prompt[1], theTable, conditions);
	postDataEvent(DataEvent(DataEvent::Table, prompt[0], theTable), prompt[0]);
}

