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
vector<string> SCDAserver::getColTitle(string sYear, string sCata)
{
	vector<string> vsDim;
	vector<string> search = { "Dim" };
	string tname = "Census$" + sYear + "$" + sCata + "$Dim";
	string orderby = "MID ASC";
	sf.selectOrderBy(search, tname, vsDim, orderby);
	if (vsDim.size() < 1) { jf.err("No Dim found-SCDAserver.getColTitle"); }
	return vsDim;
}
vector<string> SCDAserver::getDataIndex(string sYear, string sCata, vector<vector<string>>& vvsDIM)
{
	// Returns the unique DataIndices for the given combination of DIMs.
	string temp;
	int inum;
	vector<string> vsDataIndex, vsDIMTitle, conditions, vsMID;
	vector<string> search = { "DIM" };
	string tname = "DataIndex$" + sYear + "$" + sCata;
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
	for (int ii = 0; ii < vsDIMTitle.size() - 2; ii++)
	{
		for (int jj = 0; jj < vvsDIM.size(); jj++)
		{
			if (vvsDIM[jj][0] == vsDIMTitle[ii])
			{
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
	vector<string> search = { "DIMIndex" };
	string tname = "Census$" + sYear + "$" + sCata + "$DIMIndex";
	int iSize = sf.select(search, tname, vsResult);
	if (iSize < 2) { jf.err("Insufficient DIMIndex-SCDAserver.getRowTitle"); }
	int index = iSize - 2;
	tname = "Census$" + sYear + "$" + sCata + "$DIM$" + to_string(index);
	search = { "DIM" };
	vsResult.clear();
	string orderby = "MID ASC";
	iSize = sf.selectOrderBy(search, tname, vsResult, orderby);
	if (iSize < 2) { jf.err("No row titles found-SCDAserver.getRowTitle"); }
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
	// Return form [candidate index][DIM title, DIM first MID].
	vector<vector<string>> vvsCandidate;
	vector<string> vsTitle, vsMID;
	string tname, tnameMID;
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
void SCDAserver::pullTable(vector<string> prompt, vector<vector<string>> vvsDIM)
{
	// Prompt has form [id, year, cata, GEO_CODE].
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
			vsCol = { "Value" };
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
			if (iSize < 2) { jf.err("Empty data row-SCDAserver.pullTable"); }
			index = vvsTable.size();
			vvsTable.push_back(vector<string>());
			vvsTable[index].assign(vsResult.begin() + 1, vsResult.end());
		}
		vsCol = getColTitle(prompt[1], prompt[2]);
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
	// Creates a "variable" event for the next unspecified DIM. If all DIMs are set,
	// then this function will create a "table" event instead. 
	// Prompt has form [id, year, category, colTopic, rowTopic].
	// vvsVariable has form [variable index][variable title, variable MID].
	if (prompt.size() < 5) { jf.err("Missing prompt-SCDAserver.pullVariable"); }
	string sID = prompt[0];
	prompt.erase(prompt.begin());
	if (prompt[2] == "*" || prompt[3] == "*") { jf.err("No wildcards allowed-SCDAserver.pullVariable"); }
	vector<vector<string>> vvsCata = getCatalogue(prompt, vvsVariable);  
	vector<vector<string>> vvsCandidate = getVariableCandidate(vvsCata, vvsVariable);
	postDataEvent(DataEvent(DataEvent::Variable, sID, vvsCandidate, vvsCata), sID);
}
