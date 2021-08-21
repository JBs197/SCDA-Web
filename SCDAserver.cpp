#include "SCDAserver.h"

void SCDAserver::addFrameKM(vector<vector<vector<double>>>& borderKM, vector<string>& vsGeoCode, string sYear)
{
	// Append each region's TLBR frame coordinates to the list of border coordinates. 
	double xC, yC;
	string tname;
	vector<vector<string>> vvsFrame;
	vector<string> search = { "xFrameKM", "yFrameKM" };
	for (int ii = 0; ii < vsGeoCode.size(); ii++)
	{
		vvsFrame.clear();
		tname = "MapFrame$" + sYear + "$" + vsGeoCode[ii];
		sf.select(search, tname, vvsFrame);
		if (vvsFrame.size() != 2) { jf.err("No frame found-SCDAserver.addFrameKM"); }
		for (int jj = 0; jj < vvsFrame.size(); jj++)
		{
			xC = stod(vvsFrame[jj][0]);
			yC = stod(vvsFrame[jj][1]);
			borderKM[ii].push_back({ xC, yC });
		}
	}
}
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
	auto uniqueWtf = make_unique<WTPAINT>();
	wtf = uniqueWtf.get();
}
vector<vector<vector<double>>> SCDAserver::getBorderKM(vector<string>& vsGeoCode, string sYear)
{
	// Return form [region index][border point index][xCoord, yCoord].
	vector<vector<vector<double>>> border(vsGeoCode.size(), vector<vector<double>>());
	string tname;
	vector<vector<string>> vvsBorder;
	vector<string> search = { "xBorderKM", "yBorderKM" };
	for (int ii = 0; ii < vsGeoCode.size(); ii++)
	{
		tname = "Map$" + sYear + "$" + vsGeoCode[ii];
		vvsBorder.clear();
		sf.select(search, tname, vvsBorder);
		if (vvsBorder.size() < 1) { jf.err("No border found-SCDAserver.getBorderKM"); }
		border[ii].resize(vvsBorder.size(), vector<double>(2));
		for (int jj = 0; jj < vvsBorder.size(); jj++)
		{
			border[ii][jj][0] = stod(vvsBorder[jj][0]);
			border[ii][jj][1] = stod(vvsBorder[jj][1]);
		}
	}
	return border;
}
vector<vector<string>> SCDAserver::getCatalogue(vector<string>& vsPrompt)
{
	// No variables specified. 
	vector<vector<string>> vvsVariable;
	return getCatalogue(vsPrompt, vvsVariable);
}
vector<vector<string>> SCDAserver::getCatalogue(vector<string>& vsPrompt, vector<vector<string>>& vvsVariable)
{
	// For a complete or incomplete set of criteria, return the list of catalogues that
	// match that criteria. Return form [year index][sYear, sCata0, sCata1, ...].
	// vsPrompt form [sYear, sCategory, sColTopic, sRowTopic].
	// vvsVariable form [variable index][DIM title, DIM selected].
	if (vsPrompt.size() != 4) { jf.err("Missing prompt-SCDAserver.getCatalogue"); }
	for (int ii = 0; ii < vsPrompt.size(); ii++)
	{
		if (vsPrompt[ii].size() < 1) { jf.err("Missing input-SCDAserver.getCatalogue"); }
	}
	vector<vector<string>> vvsCata, vvsResult;
	vector<string> vsResult, vsYear, conditions;
	string tname, result, sColDIM, sRowDIM, tnameDIM;
	int colDIMIndex, rowDIMIndex, index, iDIMIndex, iSize;
	vector<string> search = { "Year" }, dirt = { "'" }, soap = { "''" };
	
	// Get the internal years to examine.
	if (vsPrompt[0] == "*")
	{
		tname = "Census";
		sf.select(search, tname, vsYear);
	}
	else { vsYear = getYear(vsPrompt[0]); }

	// Get a structured list of catalogues which contain the topical category.
	if (vsPrompt[1] == "*")  // Return results for all topical categories.
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
		jf.clean(vsPrompt[1], dirt, soap);
		conditions = { "Topic LIKE " + vsPrompt[1] };
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
	}
	if (vvsCata.size() < 1) { jf.err("No surviving catalogues-SCDAserver.getCatalogue"); }
	if (vsPrompt[2] == "*" && vsPrompt[3] == "*") { return vvsCata; }

	// Eliminate catalogues that do not comply with the column/row criteria.
	vector<vector<string>> cataDIMIndex;
	unordered_map<string, int> mapCataIndex;  // Form sCata->index.
	string orderby = "DIMIndex ASC";  
	search = { "DIM" };
	for (int ii = 0; ii < vvsCata.size(); ii++)
	{
		for (int jj = 1; jj < vvsCata[ii].size(); jj++)
		{
			index = cataDIMIndex.size();
			cataDIMIndex.push_back(vector<string>());
			tname = "Census$" + vvsCata[ii][0] + "$" + vvsCata[ii][jj] + "$DIMIndex";
			sf.selectOrderBy(search, tname, cataDIMIndex[index], orderby);
			if (cataDIMIndex[index].size() > 1)  // Normal catalogue.
			{
				colDIMIndex = cataDIMIndex[index].size() - 1;
				rowDIMIndex = cataDIMIndex[index].size() - 2;
				if (vsPrompt[2] == "*")
				{
					sRowDIM = cataDIMIndex[index][rowDIMIndex];
					if (sRowDIM != vsPrompt[3])
					{
						vvsCata[ii].erase(vvsCata[ii].begin() + jj);
						jj--;
						continue;
					}
					else { mapCataIndex.emplace(vvsCata[ii][jj], index); }
				}
				else if (vsPrompt[3] == "*")
				{
					sColDIM = cataDIMIndex[index][colDIMIndex];
					if (sColDIM != vsPrompt[2])
					{
						vvsCata[ii].erase(vvsCata[ii].begin() + jj);
						jj--;
						continue;
					}
					else { mapCataIndex.emplace(vvsCata[ii][jj], index); }
				}
				else
				{
					sColDIM = cataDIMIndex[index][colDIMIndex];
					sRowDIM = cataDIMIndex[index][rowDIMIndex];
					if (sColDIM != vsPrompt[2] || sRowDIM != vsPrompt[3])
					{
						vvsCata[ii].erase(vvsCata[ii].begin() + jj);
						jj--;
						continue;
					}
					else { mapCataIndex.emplace(vvsCata[ii][jj], index); }
				}
			}
			else if (cataDIMIndex[index].size() == 1)  // noDIM catalogue.
			{
				if (vsPrompt[2] != "*" && vsPrompt[2] != "Value")
				{
					vvsCata[ii].erase(vvsCata[ii].begin() + jj);
					jj--;
					continue;
				}
				sRowDIM = cataDIMIndex[index][0];
				if (vsPrompt[3] != "*" && vsPrompt[3] != sRowDIM)
				{
					vvsCata[ii].erase(vvsCata[ii].begin() + jj);
					jj--;
					continue;
				}
				else { mapCataIndex.emplace(vvsCata[ii][jj], index); }
			}
			else { jf.err("Empty DIMIndex-SCDAserver.getCatalogue"); }
		}
		if (vvsCata[ii].size() < 2)
		{
			vvsCata.erase(vvsCata.begin() + ii);
			ii--;
		}
	}
	if (vvsCata.size() < 1) { jf.err("No surviving catalogues-SCDAserver.getCatalogue"); }
	if (vvsVariable.size() < 1) { return vvsCata; }

	// Eliminate catalogues that do not comply with the specified variables.
	orderby = "MID ASC";
	for (int ii = 0; ii < vvsCata.size(); ii++)
	{
		for (int jj = 1; jj < vvsCata[ii].size(); jj++)  // For every catalogue...
		{
			index = mapCataIndex.at(vvsCata[ii][jj]);
			if (cataDIMIndex[index].size() < 3)  // Automatic fail if this catalogue does not have DIMs beyond col/row.
			{
				vvsCata[ii].erase(vvsCata[ii].begin() + jj);
				jj--;
				continue;
			}
			for (int kk = 0; kk < vvsVariable.size(); kk++)  // For every specified DIM ...
			{
				iDIMIndex = -1;
				for (int ll = 0; ll < cataDIMIndex[index].size() - 2; ll++)  // For every available DIM that isn't a column or row topic ...
				{
					if (vvsVariable[kk][0] == cataDIMIndex[index][ll])  // Found a DIM title match...
					{
						iDIMIndex = ll;
						break;
					}
					else if (ll == cataDIMIndex[index].size() - 1)
					{
						vvsCata[ii].erase(vvsCata[ii].begin() + jj);
						jj--;
					}
				}
				if (iDIMIndex < 0) { break; }

				tnameDIM = "Census$" + vvsCata[ii][0] + "$" + vvsCata[ii][jj];
				tnameDIM += "$DIM$" + to_string(iDIMIndex);
				vsResult.clear();
				iSize = sf.selectOrderBy(search, tnameDIM, vsResult, orderby);
				if (iSize < 1) { jf.err("No DIM members-SCDAserver.getCatalogue"); }
				for (int ll = 0; ll < iSize; ll++)
				{
					if (vvsVariable[kk][1] == vsResult[ll]) { break; }
					else if (ll == iSize - 1)
					{
						vvsCata[ii].erase(vvsCata[ii].begin() + jj);
						jj--;
						iDIMIndex = -1;
					}
				}
				if (iDIMIndex < 0) { break; }
			}
		}
		if (vvsCata[ii].size() < 2)
		{
			vvsCata.erase(vvsCata.begin() + ii);
			ii--;
		}
	}
	if (vvsCata.size() < 1) { jf.err("No surviving catalogues-SCDAserver.getCatalogue"); }
	return vvsCata;
}
void SCDAserver::getGeoFamily(vector<vector<string>>& geo, vector<string>& vsGeoCode, vector<string>& vsRegionName)
{
	// vsGeoCode return form [sGeoCodeParent, sGeoCodeChild0, ...].
	// vsRegionName return form [sRegionNameParent, sRegionNameChild0, ...].
	if (geo.size() < 1 || vsGeoCode.size() < 1 || vsRegionName.size() < 1) { jf.err("Missing input-SCDAserver.getGeoFamily"); }
	for (int ii = 0; ii < geo.size(); ii++)
	{
		if (geo[ii].back() == vsGeoCode[0])
		{
			vsGeoCode.push_back(geo[ii][0]);
			vsRegionName.push_back(geo[ii][1]);
		}
	}
}
vector<string> SCDAserver::getColTitle(string sYear, string sCata)
{
	vector<string> vsResult;
	vector<string> search = { "DIMIndex" };
	string tname = "Census$" + sYear + "$" + sCata + "$DIMIndex";
	int iSize = sf.select(search, tname, vsResult);
	if (iSize < 2) { jf.err("Insufficient DIMIndex-SCDAserver.getColTitle"); }
	int index = iSize - 1;
	vector<string> vsDim;
	search = { "Dim" };
	tname = "Census$" + sYear + "$" + sCata + "$Dim";
	string orderby = "MID ASC";
	sf.selectOrderBy(search, tname, vsDim, orderby);
	if (vsDim.size() < 1) { jf.err("No Dim found-SCDAserver.getColTitle"); }
	vsDim.push_back(vsResult[index]);
	return vsDim;
}
vector<double> SCDAserver::getDataFamily(string sYear, string sCata, vector<string> vsIndex, vector<string> vsGeoCode)
{
	// Return each region's data value, in the same order as vsGeoCode.
	// vsIndex has form [uniqueDataIndex, dimIndex] where dimIndex begins at 1.
	vector<double> data(vsGeoCode.size());
	string tname, result;
	vector<string> search = { vsIndex[1] };
	vector<string> conditions = { "DataIndex = " + vsIndex[0] };
	string tname0 = "Data$" + sYear + "$" + sCata + "$";
	for (int ii = 0; ii < vsGeoCode.size(); ii++)
	{
		tname = tname0 + vsGeoCode[ii];
		result.clear();
		sf.select(search, tname, result, conditions);
		data[ii] = stod(result);
	}
	return data;
}
vector<string> SCDAserver::getDataIndex(string sYear, string sCata, vector<vector<string>>& vvsDIM)
{
	// Returns the unique DataIndices for the given combination of DIMs.
	string temp;
	int inum;
	vector<string> vsDataIndex, vsDIMTitle, conditions, vsMID;
	vector<string> search = { "DIM" };
	string tname = "Census$" + sYear + "$" + sCata + "$DIMIndex";
	string orderby = "DIMIndex ASC";
	int iSize = sf.selectOrderBy(search, tname, vsDIMTitle, orderby);
	if (iSize < 1) { jf.err("No DIM titles found-SCDAserver.getDataIndex"); }
	else if (iSize == 1)  // noDIM table
	{
		vsDataIndex = { "0" };
		return vsDataIndex;
	}
	else if (iSize == 2)  // No variables, only one table possible.
	{
		vsDataIndex = { "all" };
		return vsDataIndex;
	}
	
	search = { "MID" };
	if (vvsDIM.size() >= vsDIMTitle.size()) { jf.err("Invalid vvsDIM-SCDAserver.getDataIndex"); }
	int iSearch = max(vsDIMTitle.size() - 2, vvsDIM.size());
	for (int ii = 0; ii < iSearch; ii++)
	{
		for (int jj = 0; jj < vvsDIM.size(); jj++)
		{
			if (vvsDIM[jj][0] == vsDIMTitle[ii])
			{
				temp.clear();
				conditions = { "DIM LIKE " + vvsDIM[jj][1] };
				tname = "Census$" + sYear + "$" + sCata + "$DIM$" + to_string(ii);
				iSize = sf.select(search, tname, temp, conditions);
				if (iSize < 1) { jf.err("MID not found-SCDAserver.getDataIndex"); }
				inum = stoi(temp);
				inum--;
				vsMID.push_back(to_string(inum));
				break;
			}
			else if (jj == vvsDIM.size() - 1)
			{
				jf.err("Unresolved DIM-SCDAserver.getDataIndex");
			}
		}
	}

	search = { "DataIndex" };
	tname = "DataIndex$" + sYear + "$" + sCata;
	conditions.clear();
	for (int ii = 0; ii < vsMID.size(); ii++)
	{
		temp.clear();
		if (ii > 0) { temp += "AND "; }
		temp += "DIM" + to_string(ii) + " = " + vsMID[ii];
		conditions.push_back(temp);
	}
	sf.select(search, tname, vsDataIndex, conditions);

	if (vsDataIndex.size() < 1) { jf.err("No DataIndex found-SCDAserver.getDataIndex"); }
	return vsDataIndex;
}
vector<string> SCDAserver::getDIMIndex(vector<vector<string>>& vvsCata)
{
	// If only one catalogue exists in the list, return its DIMIndex. Else, return null.
	vector<string> DIMIndex;
	int numCata = 0;
	for (int ii = 0; ii < vvsCata.size(); ii++)
	{
		numCata += vvsCata[ii].size() - 1;
	}
	if (numCata <= 0) { jf.err("Zero catalogues in list-SCDAserver.getDIMIndex"); }
	else if (numCata > 1) { return DIMIndex; }
	string tname = "Census$" + vvsCata[0][0] + "$" + vvsCata[0][1] + "$DIMIndex";
	vector<string> search = { "DIM" };
	string orderby = "DIMIndex ASC";
	sf.selectOrderBy(search, tname, DIMIndex, orderby);
	if (DIMIndex.size() < 1) { jf.err("No DIMIndex found-SCDAserver.getDIMIndex"); }
	return DIMIndex;
}
vector<vector<string>> SCDAserver::getGeo(string sYear, string sCata)
{
	// Returns a Geo table, sorted by ascending GEO_LEVEL.
	vector<vector<string>> geo;
	vector<string> search = { "*" };
	string tname = "Geo$" + sYear + "$" + sCata;
	string orderby = "GEO_LEVEL ASC";
	int iSize = sf.selectOrderBy(search, tname, geo, orderby);
	if (iSize < 1) { jf.err("Empty geo table-SCDAserver.getGeo"); }
	return geo;
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
vector<string> SCDAserver::getRowTitle(string sYear, string sCata)
{
	vector<string> vsResult;
	vector<string> search = { "DIM" };
	string tname = "Census$" + sYear + "$" + sCata + "$DIMIndex";
	string orderby = "DIMIndex ASC";
	int iSize = sf.selectOrderBy(search, tname, vsResult, orderby);
	if (iSize < 2) { jf.err("Insufficient DIMIndex-SCDAserver.getRowTitle"); }
	int index = iSize - 2;
	string DIMIndexTitle = vsResult[index];
	tname = "Census$" + sYear + "$" + sCata + "$DIM$" + to_string(index);
	search = { "DIM" };
	vsResult.clear();
	orderby = "MID ASC";
	iSize = sf.selectOrderBy(search, tname, vsResult, orderby);
	if (iSize < 2) { jf.err("No row titles found-SCDAserver.getRowTitle"); }
	vsResult.push_back(DIMIndexTitle);
	return vsResult;
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
vector<vector<string>> SCDAserver::getVariableCandidate(vector<vector<string>>& vvsCata, vector<vector<string>>& vvsVariable)
{
	// Return form [candidate index][MID0, MID1, ..., Title].
	vector<vector<string>> vvsCandidate;  
	vector<string> vsTitle, vsMID;
	string tname, tnameMID, sTitle;
	int iSize, iFail;
	vector<string> search = { "DIM" };
	string orderby = "DIMIndex ASC";
	string orderbyMID = "MID ASC";
	for (int ii = 0; ii < vvsCata.size(); ii++)
	{
		for (int jj = 1; jj < vvsCata[ii].size(); jj++)
		{
			tname = "Census$" + vvsCata[ii][0] + "$" + vvsCata[ii][jj] + "$DIMIndex";
			vsTitle.clear();
			iSize = sf.selectOrderBy(search, tname, vsTitle, orderby);
			if (iSize < 3) { continue; }
			for (int kk = 0; kk < vsTitle.size() - 2; kk++)
			{
				iFail = 0;
				for (int ll = 0; ll < vvsVariable.size(); ll++)
				{
					if (vsTitle[kk] == vvsVariable[ll][0]) 
					{ 
						vsTitle.erase(vsTitle.begin() + kk);
						kk--;
						iFail = 1;
						break; 
					}
				}
				if (!iFail)
				{
					vsMID.clear();
					tnameMID = "Census$" + vvsCata[ii][0] + "$" + vvsCata[ii][jj];
					tnameMID += "$DIM$" + to_string(kk);
					iSize = sf.selectOrderBy(search, tnameMID, vsMID, orderbyMID);
					if (iSize < 1) { jf.err("No MID found-SCDAserver.getVariableCandidate"); }
					vsMID.push_back(vsTitle[kk]);
					vvsCandidate.push_back(vsMID);
				}
			}
		}
	}
	
	// If more than one catalogue survived, ensure that the first suggested variable
	// will differentiate between some of them.
	int numCata = 0;
	for (int ii = 0; ii < vvsCata.size(); ii++)
	{
		numCata += vvsCata[ii].size() - 1;
	}
	if (numCata > 1)  // RESUME HERE. Should return only differentiating parameters!
	{
		vector<vector<string>> vvsDIMIndex(numCata, vector<string>());
		int index = 0;
		search = { "DIM" };
		orderby = "DIMIndex ASC";
		for (int ii = 0; ii < vvsCata.size(); ii++)
		{
			for (int jj = 1; jj < vvsCata[ii].size(); jj++)
			{
				tname = "Census$" + vvsCata[ii][0] + "$" + vvsCata[ii][jj] + "$DIMIndex";
				sf.selectOrderBy(search, tname, vvsDIMIndex[index], orderby);
				index++;
			}
		}
		int iPos = vvsDIMIndex.size();
		for (int ii = 0; ii < iPos; ii++)
		{
			vvsDIMIndex[ii].resize(vvsDIMIndex[ii].size() - 2);
		}
		int count;
		for (int ii = 0; ii < vvsCandidate.size(); ii++)
		{
			count = 0;
			sTitle = vvsCandidate[ii].back();
			for (int jj = 0; jj < iPos; jj++)
			{
				for (int kk = 0; kk < vvsDIMIndex[jj].size(); kk++)
				{
					if (vvsDIMIndex[jj][kk] == sTitle)
					{
						count++;
						break;
					}
				}
			}
			if (count < iPos)  // At least one catalogue does not have this variable.
			{
				if (ii > 0)
				{
					vvsCandidate[ii].swap(vvsCandidate[0]);
				}
				break;
			}
			else if (ii == vvsCandidate.size() - 1)  // The catalogues have the same variables...
			{
				// Keep the catalogue with the greatest geographical articulation.
				int maxCount = -1;
				vector<string> maxCata;
				for (int jj = 0; jj < vvsCata.size(); jj++)
				{
					for (int kk = 1; kk < vvsCata[jj].size(); kk++)
					{
						tname = "Geo$" + vvsCata[jj][0] + "$" + vvsCata[jj][kk];
						count = sf.count(tname);
						if (count > maxCount)
						{
							maxCount = count;
							maxCata = { vvsCata[jj][kk] };
						}
						else if (count == maxCount)
						{
							maxCata.push_back(vvsCata[jj][kk]);
						}
					}
				}
				if (maxCata.size() != 1) { jf.err("Unable to differentiate catalogues-SCDAserver.getVariableCandidate"); }
				for (int jj = 0; jj < vvsCata.size(); jj++)
				{
					for (int kk = 1; kk < vvsCata[jj].size(); kk++)
					{
						if (vvsCata[jj][kk] != maxCata[0])
						{
							vvsCata[jj].erase(vvsCata[jj].begin() + kk);
							kk--;
						}
					}
					if (vvsCata[jj].size() < 2)
					{
						vvsCata.erase(vvsCata.begin() + jj);
						jj--;
					}
				}

				// Refresh vvsCandidate for the single catalogue survivor.
				vvsCandidate = getVariableCandidate(vvsCata, vvsVariable);
			}
		}
	}

	if (vvsCandidate.size() < 1) { jf.err("Zero candidate variables remain-SCDAserver.getVariableCandidate"); }
	return vvsCandidate;
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
void SCDAserver::pullMap(vector<string> prompt, vector<vector<string>> vvsDIM)
{
	// Prompt has form [id, year, cata, sParent].
	// vvsDIM has form [some index][DIM title, DIM value] (all DIMs must be represented).
	string result, tnameData;
	vector<string> vsResult;
	vector<vector<string>> vvsResult;
	string tnameGeo = "Geo$" + prompt[1] + "$" + prompt[2];
	vector<string> search = { "GEO_CODE" };
	vector<string> conditions = { "\"Region Name\" LIKE " + prompt[3] };
	sf.select(search, tnameGeo, result, conditions);
	if (result.size() < 1) { jf.err("No parent geoCode-SCDAserver.pullMap"); }

	// Get the list of regions to display.
	vector<string> vsGeoCode = { result };
	vector<string> vsRegionName = { prompt[3] };
	vector<vector<string>> geo = getGeo(prompt[1], prompt[2]);
	jf.removeBlanks(geo);
	getGeoFamily(geo, vsGeoCode, vsRegionName);

	// Get all the border coordinates, form [region index][border point index][xCoord, yCoord].
	vector<vector<vector<double>>> areas = getBorderKM(vsGeoCode, prompt[1]);
	addFrameKM(areas, vsGeoCode, prompt[1]);  // Final two coordinates (for the parent region) are the frame TLBR.

	// Retrieve the unique DataIndex needed for this row of data. 
	vector<string> vsDim = vvsDIM.back();  // Form [column DIMIndex title, dimIndex(1, ...)].
	vvsDIM.pop_back();
	vector<string> vsDataIndex = getDataIndex(prompt[1], prompt[2], vvsDIM);
	if (vsDataIndex.size() != 1) { jf.err("Failed to return one DataIndex-SCDAserver.pullMap"); }
	vsDataIndex.push_back("dim" + vsDim[1]);  // Now has form [dataIndex, dimTitle].

	// Get all the table data cells, form [region index][numeric value].
	vector<double> data = getDataFamily(prompt[1], prompt[2], vsDataIndex, vsGeoCode);

	postDataEvent(DataEvent(DataEvent::Map, prompt[0], vsRegionName, areas, data), prompt[0]);
}
void SCDAserver::pullTable(vector<string> prompt, vector<vector<string>> vvsDIM)
{
	// Prompt has form [id, year, cata, GEO_CODE, Region Name].
	// vvsDIM has form [some index][DIM title, DIM value].
	vector<vector<string>> vvsTable;
	vector<string> vsCol, vsRow, vsResult, conditions;
	string tnameDIM, tnameDim, tnameData;
	int iCol, iRow, index, iSize;
	vector<string> search = { "*" };
	string tname = "Data$" + prompt[1] + "$" + prompt[2] + "$" + prompt[3];
	vector<string> vsDataIndex = getDataIndex(prompt[1], prompt[2], vvsDIM);
	if (vsDataIndex.size() == 1)
	{
		if (vsDataIndex[0] == "0")
		{
			conditions = { "DataIndex = 0" };
			iSize = sf.select(search, tname, vsResult, conditions);
			if (iSize < 2) { jf.err("Empty data row-SCDAserver.pullTable"); }
			for (int ii = 0; ii < iSize; ii++)
			{
				vvsTable.push_back({ vsResult[ii] });
			}
			vsCol = { prompt[4], "Value" };
			vsRow = getColTitle(prompt[1], prompt[2]);  // This is not a typo.
		}
		else if (vsDataIndex[0] == "all")
		{
			iSize = sf.select(search, tname, vvsTable);
			if (iSize < 2) { jf.err("Empty data row-SCDAserver.pullTable"); }
			for (int ii = 0; ii < iSize; ii++)
			{
				vvsTable[ii].erase(vvsTable[ii].begin());
			}
			vsCol = getColTitle(prompt[1], prompt[2]);
			vsCol.insert(vsCol.begin(), prompt[4]);
			vsRow = getRowTitle(prompt[1], prompt[2]);
		}
		else { jf.err("Unknown vsDataIndex-SCDAserver.pullTable"); }
	}
	else
	{
		for (int ii = 0; ii < vsDataIndex.size(); ii++)
		{
			conditions = { "DataIndex = " + vsDataIndex[ii] };
			vsResult.clear();
			iSize = sf.select(search, tname, vsResult, conditions);
			if (iSize < 2) { continue; }
			index = vvsTable.size();
			vvsTable.push_back(vector<string>());
			vvsTable[index].assign(vsResult.begin() + 1, vsResult.end());
		}
		vsCol = getColTitle(prompt[1], prompt[2]);
		vsCol.insert(vsCol.begin(), prompt[4]);
		vsRow = getRowTitle(prompt[1], prompt[2]);
	}
	postDataEvent(DataEvent(DataEvent::Table, prompt[0], vvsTable, vsCol, vsRow), prompt[0]);
}
void SCDAserver::pullTopic(vector<string> prompt)
{
	// Posts an event containing the list of options for the column topic and
	// the row topic. The prompt has form [id, year, category, colTopic, rowTopic].
	if (prompt.size() != 5) { jf.err("Missing prompt-SCDAserver.pullTopic"); }
	string sID = prompt[0];
	prompt.erase(prompt.begin());
	vector<vector<string>> vvsCata = getCatalogue(prompt);
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
				if (prompt[2] == "*" || prompt[2] == sCol)
				{
					if (!setRowTopic.count(sRow))
					{
						vsRowTopic.push_back(sRow);
						setRowTopic.emplace(sRow);
					}
				}
				if (prompt[3] == "*" || prompt[3] == sRow)
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
				if (prompt[2] == "*")
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
	postDataEvent(DataEvent(DataEvent::Topic, sID, vsColTopic, vsRowTopic), sID);
}
void SCDAserver::pullTree(vector<string> prompt)
{
	// Creates a "tree" event containing a JTREE object that describes the geographical regions.
	// Prompt has form [id, year, cata].
	if (prompt.size() < 3) { jf.err("Missing prompt-SCDAserver.pullTree"); }
	JTREE jt;
	vector<vector<string>> geo = getGeo(prompt[1], prompt[2]);
	jf.removeBlanks(geo);
	jt.inputTreeDB(geo);
	postDataEvent(DataEvent(DataEvent::Tree, prompt[0], jt), prompt[0]);
}
void SCDAserver::pullVariable(vector<string> prompt, vector<vector<string>> vvsVariable)
{
	// Creates a "variable" event for the next unspecified DIM. 
	// vvsVariable refers to previously fixed DIM titles and MIDs (restrictions).
	// Prompt has form [id, year, category, colTopic, rowTopic].
	// vvsVariable has form [variable index][variable title, variable MID].
	if (prompt.size() < 5) { jf.err("Missing prompt-SCDAserver.pullVariable"); }
	string sID = prompt[0];
	prompt.erase(prompt.begin());
	if (prompt[2] == "*" || prompt[3] == "*") { jf.err("No wildcards allowed-SCDAserver.pullVariable"); }
	vector<vector<string>> vvsCata = getCatalogue(prompt, vvsVariable);  
	vector<vector<string>> vvsCandidate = getVariableCandidate(vvsCata, vvsVariable);
	vector<string> DIMIndex = getDIMIndex(vvsCata);
	postDataEvent(DataEvent(DataEvent::Variable, sID, vvsCandidate, vvsCata, DIMIndex), sID);
}
