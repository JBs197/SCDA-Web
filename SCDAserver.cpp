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
	auto uniqueWtf = make_unique<WTPAINT>(vsTemp);
	wtf = uniqueWtf.get();
}
vector<vector<string>> SCDAserver::getCatalogue(string sYear, string sCategory, string sColTopic, string sRowTopic)
{
	// For a complete or incomplete set of criteria, return the list of catalogues that
	// match that criteria. Return form [year index][sYear, sCata0, sCata1, ...].
	if (sYear.size() < 1 || sCategory.size() < 1 || sColTopic.size() < 1 || sRowTopic.size() < 1) { jf.err("Missing input-SCDAserver.getCatalogue"); }
	vector<vector<string>> vvsCata, vvsResult;
	vector<string> vsResult, vsYear, conditions;
	string tname, result, sColDIM, sRowDIM;
	int colDIMIndex, rowDIMIndex;
	vector<string> search = { "Year" }, dirt = { "'" }, soap = { "''" };
	if (sYear == "*")
	{
		tname = "Census";
		sf.select(search, tname, vsYear);
	}
	else { vsYear = getYear(sYear); }

	if (sCategory == "*")  // Return results for all topical categories.
	{
		vvsCata.resize(vsYear.size(), vector<string>(1));
		for (int ii = 0; ii < vvsCata.size(); ii++)
		{
			vvsCata[ii][0] = vsYear[ii];
		}

		search = { "Catalogue" };
		for (int ii = 0; ii < vvsCata.size(); ii++)
		{
			vsResult.clear();
			tname = "Census$" + vvsCata[ii][0];
			sf.select(search, tname, vsResult);
			vvsCata[ii].resize(1 + vsResult.size());
			for (int jj = 0; jj < vsResult.size(); jj++)
			{
				vvsCata[ii][1 + jj] = vsResult[jj];
			}
		}		
	}
	else  // Return results for a given topical category.
	{
		// Establish which years contain this category.
		search = { "Topic Index" };
		jf.clean(sCategory, dirt, soap);
		conditions = { "Topic LIKE " + sCategory };
		for (int ii = 0; ii < vsYear.size(); ii++)
		{
			tname = "Topic$" + vsYear[ii];
			result.clear();
			sf.select(search, tname, result, conditions);
			if (result.size() > 0) 
			{ 
				vvsCata.push_back({ vsYear[ii] });
			}
		}
		if (vvsCata.size() < 1) { jf.err("Category not found-SCDAserver.getCatalogue"); }

		// Determine which catalogues, per year, contain this category.
		search = { "Catalogue" };
		for (int ii = 0; ii < vvsCata.size(); ii++)
		{
			tname = "Census$" + vvsCata[ii][0];
			sf.select(search, tname, vvsCata[ii], conditions);
		}

		// Eliminate catalogues that do not comply with the column/row criteria.
		search = { "DIMIndex", "DIM" };
		if (sColTopic != "*" || sRowTopic != "*")
		{
			for (int ii = 0; ii < vvsCata.size(); ii++)
			{
				for (int jj = 1; jj < vvsCata[ii].size(); jj++)
				{
					tname = "Census$" + vvsCata[ii][0] + "$" + vvsCata[ii][jj] + "$DIMIndex";
					vvsResult.clear();
					sf.select(search, tname, vvsResult);
					if (vvsResult.size() > 1)  // Normal catalogue.
					{
						colDIMIndex = vvsResult.size() - 1;
						rowDIMIndex = vvsResult.size() - 2;
						if (sColTopic == "*")
						{
							sRowDIM = vvsResult[rowDIMIndex][1];
							if (sRowDIM != sRowTopic)
							{
								vvsCata[ii].erase(vvsCata[ii].begin() + jj);
								jj--;
								continue;
							}
						}
						else if (sRowTopic == "*")
						{
							sColDIM = vvsResult[colDIMIndex][1];
							if (sColDIM != sColTopic)
							{
								vvsCata[ii].erase(vvsCata[ii].begin() + jj);
								jj--;
								continue;
							}
						}
						else
						{
							sColDIM = vvsResult[colDIMIndex][1];
							sRowDIM = vvsResult[rowDIMIndex][1];
							if (sColDIM != sColTopic || sRowDIM != sRowTopic)
							{
								vvsCata[ii].erase(vvsCata[ii].begin() + jj);
								jj--;
								continue;
							}
						}
					}
					else if (vvsResult.size() == 1)  // noDIM catalogue.
					{
						if (sColTopic != "*" && sColTopic != "Value")
						{ 
							vvsCata[ii].erase(vvsCata[ii].begin() + jj);
							jj--;
							continue; 
						}
						sRowDIM = vvsResult[0][1];
						if (sRowTopic != "*" && sRowTopic != sRowDIM)
						{
							vvsCata[ii].erase(vvsCata[ii].begin() + jj);
							jj--;
							continue;
						}
					}
					else { jf.err("Empty DIMIndex-SCDAserver.getCatalogue"); }
				}
				if (vvsCata[ii].size() < 2)
				{
					vvsCata.erase(vvsCata.begin() + ii);
					ii--;
				}
			}
		}
	}
	return vvsCata;
}
vector<string> SCDAserver::getDataIndex(string sYear, string sCata, string sDIM)
{
	// Returns the unique DataIndex for the given combination of DIMs.
	string temp;
	vector<string> vsDataIndex, vsDIM, conditions;
	size_t pos1 = 0;
	size_t pos2 = sDIM.find('$');
	while (pos2 < sDIM.size())
	{
		vsDIM.push_back(sDIM.substr(pos1, pos2 - pos1));
		pos1 = pos2 + 1;
		pos2 = sDIM.find('$', pos1);
	}
	vsDIM.push_back(sDIM.substr(pos1));

	string tname = "DataIndex$" + sYear + "$" + sCata;
	vector<string> search = { "DataIndex" };
	for (int ii = 0; ii < vsDIM.size(); ii++)
	{
		temp.clear();
		if (ii > 0) { temp += "AND "; }
		temp += "DIM" + to_string(ii) + " = " + vsDIM[ii];
		conditions.push_back(temp);
	}
	sf.select(search, tname, vsDataIndex, conditions);
	if (vsDataIndex.size() < 1) { jf.err("No DataIndex found-SCDAserver.getDataIndex"); }
	return vsDataIndex;
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
vector<string> SCDAserver::getTopicList(string sYear)
{
	// Returns an alphabetically-sorted list of topical categories for the 
	// given external year.
	vector<string> vsTopic;
	string tname;
	set<string> setTopic;
	vector<string> search = { "Topic" };
	vector<string> vsYear = getYear(sYear);
	lock_guard<mutex> lg(m_server);
	for (int ii = 0; ii < vsYear.size(); ii++)
	{
		tname = "Topic$" + vsYear[ii];
		sf.select(search, tname, vsTopic);
	}
	for (int ii = 0; ii < vsTopic.size(); ii++)  // Filter out repeated entries.
	{
		if (setTopic.count(vsTopic[ii])) 
		{ 
			vsTopic.erase(vsTopic.begin() + ii);
			ii--; 
		}
		else { setTopic.emplace(vsTopic[ii]); }
	}
	sort(vsTopic.begin(), vsTopic.end());
	return vsTopic;
}
vector<string> SCDAserver::getYear(string sYear)
{
	// Returns a list of internal years represented by a single external year. 
	vector<string> vsYear;
	if (sYear == "2016") { vsYear = { "2016", "2017" }; }
	else if (sYear == "2011") { vsYear = { "2011", "2013" }; }
	else { vsYear = { sYear }; }
	return vsYear;
}
void SCDAserver::postDataEvent(const DataEvent& event, string sID)
{
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
void SCDAserver::pullTable(vector<string> prompt)
{
	// Prompt has form [id, year, cata, sDIM, GEO_CODE].
	vector<vector<string>> vvsTable;
	vector<string> vsCol, vsRow, vsResult, vsDataIndex, conditions;
	string tnameDIM, tnameDim, tnameData;
	int iCol, iRow, index;
	vector<string> search = { "DIMIndex" };
	string tname = "Census$" + prompt[1] + "$" + prompt[2] + "$DIMIndex";
	sf.select(search, tname, vsResult);
	if (vsResult.size() == 0) { jf.err("Empty DIMIndex-SCDAserver.pullTable"); }
	else if (vsResult.size() == 1)  // noDIM table.
	{
		tnameDim = "Census$" + prompt[1] + "$" + prompt[2] + "$Dim";
		search = { "Dim" };
		sf.select(search, tnameDim, vsRow);
		if (vsRow.size() < 1) { jf.err("No dim found for noDIM-SCDAserver.pullTable"); }
		vsCol = { "Value" };
		tnameData = "Data$" + prompt[1] + "$" + prompt[2] + "$" + prompt[4];
		search = { "*" };
		sf.select(search, tnameData, vvsTable);
		if (vvsTable.size() < 1) { jf.err("No data returned for noDIM-SCDAserver.pullTable"); }
	}
	else
	{
		tnameDim = "Census$" + prompt[1] + "$" + prompt[2] + "$Dim";
		search = { "Dim" };
		sf.select(search, tnameDim, vsCol);
		if (vsCol.size() < 1) { jf.err("No columns found-SCDAserver.pullTable"); }
		iRow = vsResult.size() - 2;
		tnameDIM = "Census$" + prompt[1] + "$" + prompt[2] + "$DIM$" + to_string(iRow);
		search = { "DIM" };
		sf.select(search, tnameDIM, vsRow);
		if (vsRow.size() < 1) { jf.err("No rows found-SCDAserver.pullTable"); }

		vsDataIndex = getDataIndex(prompt[1], prompt[2], prompt[3]);
		tnameData = "Data$" + prompt[1] + "$" + prompt[2] + "$" + prompt[4];
		search = { "*" };
		for (int ii = 0; ii < vsDataIndex.size(); ii++)
		{
			vsResult.clear();
			conditions = { "DataIndex = " + vsDataIndex[ii] };
			sf.select(search, tnameData, vsResult, conditions);
			if (vsResult.size() < 1) { jf.err("No data found-SCDAserver.pullTable"); }
			index = vvsTable.size();
			vvsTable.push_back(vector<string>());
			vvsTable[index].assign(vsResult.begin() + 1, vsResult.end());
		}
	}
	postDataEvent(DataEvent(DataEvent::Table, prompt[0], vvsTable, vsCol, vsRow), prompt[0]);
}
void SCDAserver::pullTopic(vector<string> prompt)
{
	// Posts an event containing the list of options for the column topic and
	// the row topic. The prompt has form [id, year, category, colTopic, rowTopic].
	vector<vector<string>> vvsCata = getCatalogue(prompt[1], prompt[2], prompt[3], prompt[4]);
	vector<vector<string>> vvsResult;
	vector<string> vsColTopic, vsRowTopic;
	string tname, temp, sCol, sRow;
	set<string> setColTopic, setRowTopic;
	vector<string> search = { "DIMIndex", "DIM" }, dirt = {"'"}, soap = {"''"};

	int numCata = 0;
	for (int ii = 0; ii < vvsCata.size(); ii++)
	{
		numCata += vvsCata[ii].size() - 1;
	}
	if (numCata == 0) { jf.err("All catalogues were eliminated-SCDAserver.pullTopic"); }

	lock_guard<mutex> lg(m_server);
	for (int ii = 0; ii < vvsCata.size(); ii++)
	{
		for (int jj = 1; jj < vvsCata[ii].size(); jj++)
		{
			tname = "Census$" + vvsCata[ii][0] + "$" + vvsCata[ii][jj] + "$DIMIndex";
			vvsResult.clear();
			sf.select(search, tname, vvsResult);
			if (vvsResult.size() > 1)  // Normal table.
			{
				sCol = vvsResult[vvsResult.size() - 1][1];
				sRow = vvsResult[vvsResult.size() - 2][1];
				if (prompt[3] == "*" || prompt[3] == sCol)
				{
					if (!setRowTopic.count(sRow))
					{
						vsRowTopic.push_back(sRow);
						setRowTopic.emplace(sRow);
					}
				}
				if (prompt[4] == "*" || prompt[4] == sRow)
				{
					if (!setColTopic.count(sCol))
					{
						vsColTopic.push_back(sCol);
						setColTopic.emplace(sCol);
					}
				}
			}
			else if (vvsResult.size() == 1)  // noDIM table.
			{
				sRow = vvsResult[vvsResult.size() - 1][1];
				if (prompt[3] == "*")
				{
					if (!setRowTopic.count(sRow))
					{
						vsRowTopic.push_back(sRow);
						setRowTopic.emplace(sRow);
					}
				}
			}
			else { jf.err("No DIMIndex found-SCDAserver.pullTopic"); }
		}
	}
	if (vsRowTopic.size() < 1) { jf.err("All row topics eliminated-SCDAserver.pullTopic"); }
	if (vsColTopic.size() < 1) 
	{ 
		vsColTopic.push_back("Value");
	}
	postDataEvent(DataEvent(DataEvent::Topic, prompt[0], vsColTopic, vsRowTopic), prompt[0]);
}
void SCDAserver::pullVariable(vector<string> prompt)
{
	// Creates a "variable" event for the next unspecified DIM. If all DIMs are set,
	// then this function will create a "table" event instead. 
	// Prompt has form [id, year, category, colTopic, rowTopic, sDIM].
	if (prompt[3] == "*" || prompt[4] == "*") { jf.err("No wildcards allowed-SCDAserver.pullVariable"); }
	vector<vector<string>> vvsResult;
	vector<vector<string>> vvsCata = getCatalogue(prompt[1], prompt[2], prompt[3], prompt[4]);
	int numCata = 0;
	for (int ii = 0; ii < vvsCata.size(); ii++)
	{
		numCata += vvsCata[ii].size() - 1;
	}
	if (numCata != 1) { jf.err("Multiple catalogues-SCDAserver.pullVariable"); }

	lock_guard<mutex> lg(m_server);
	vector<string> search = { "DIMIndex", "DIM" };
	string tname = "Census$" + vvsCata[0][0] + "$" + vvsCata[0][1] + "$DIMIndex";
	sf.select(search, tname, vvsResult);
	if (vvsResult.size() == 0) { jf.err("Empty DIMIndex-SCDAserver.pullVariable"); }
	else if (vvsResult.size() == 1)
	{

	}

}
