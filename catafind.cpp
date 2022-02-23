#include "catafind.h"

using namespace std;

CataFind::CataFind(string& dbpath) : dbPath(dbpath) 
{
	sf.init(dbPath);
	initList();
}

void CataFind::applyFilter(vector<string>& vsCata, CataFilter& cf)
{
	// Return a list of catalogues which satisfy the given filter.
	list<string> listCata(allCata);
	string orderby, tname;
	vector<string> conditions, search, vsResult, vsYear;
	unordered_set<string> setResult;

	// Obtain a list of available census years. 
	sf.select({ "*" }, "Census", vsYear);  

	// Apply as many filters as have been specified.
	if (cf.year != "*") {
		filterYear(listCata, cf);
		vsYear = { cf.year };
	}
	if (cf.category != "*") {
		filterCategory(listCata, cf);
	}
	if (cf.colTopic != "*" || cf.rowTopic != "*") {
		filterTopic(listCata, cf, vsYear);
	}
	if (cf.vParameter.size() > 0) {
		filterParameter(listCata, cf, vsYear);
	}
	if (cf.differentiator != "*") {
		filterDifferentiator(listCata, cf, vsYear);
	}

	// Output as a vector.
	int index{ 0 };
	vsCata.resize(listCata.size());
	for (auto it = listCata.begin(); it != listCata.end(); ++it) {
		vsCata[index] = std::move(*it);
		index++;
	}
}
void CataFind::err(string message)
{
	string errorMessage = "CataFind error:\n" + message;
	JLOG::getInstance()->err(errorMessage);
}
void CataFind::filterCategory(list<string>& listCata, CataFilter& cf)
{
	unordered_set<string> setResult;
	auto failFilter = [&](string& sCata) {
		return (bool)!setResult.count(sCata);
	};

	vector<string> search{ "Catalogue" };
	string tname{ "Census$" + cf.year };
	vector<string> conditions{ "Topic LIKE '" + cf.category + "'" };
	sf.select(search, tname, setResult);
	listCata.remove_if(failFilter);
}
void CataFind::filterDifferentiator(list<string>& listCata, CataFilter& cf, vector<string>& vsYear)
{
	int numResult, pass;
	string tname;
	unordered_set<string> setResult;
	vector<string> search{ "Catalogue" };
	vector<string> conditions{ "ForWhom LIKE '" + cf.differentiator + "'" };
	int numYear = (int)vsYear.size();
	for (auto it = listCata.begin(); it != listCata.end();) {
		pass = 0;
		for (int ii = 0; ii < numYear; ii++) {
			tname = "ForWhom$" + vsYear[ii];
			if (sf.tableExist(tname)) {
				pass = 1;
				setResult.clear();
				numResult = sf.select(search, tname, setResult, conditions);
				if (numResult == 0) { err("Failed to locate any catalogues-filterDifferentiator"); }

				if (!setResult.count(*it)) {
					it = listCata.erase(it);
					pass = -1;
				}
				break;
			}
		}
		if (pass == 0) { it = listCata.erase(it); }
		else if (pass > 0) { ++it; }
	}
}
void CataFind::filterParameter(list<string>& listCata, CataFilter& cf, vector<string>& vsYear)
{
	int numDIM, pass;
	string tname;
	vector<string> vsResult;
	vector<string> search{ "DIM" };
	string orderby{ "DIMIndex DESC" };
	int numParam = (int)cf.vParameter.size();
	int numYear = (int)vsYear.size();
	for (auto it = listCata.begin(); it != listCata.end();) {
		pass = 0;
		for (int ii = 0; ii < numYear; ii++) {
			tname = "Census$" + vsYear[ii] + "$" + *it + "$DIMIndex";
			if (sf.tableExist(tname)) {
				vsResult.clear();
				numDIM = sf.selectOrderBy(search, tname, vsResult, orderby);
				if (numDIM <= 2) { 
					it = listCata.erase(it);
					pass = -1; 
					break;
				}

				pass = 1;
				for (int jj = 0; jj < numParam; jj++) {
					// Every parameter must pass.
					for (int kk = vsResult.size() - 1; kk >= 2; kk--) {
						if (vsResult[kk] == cf.vParameter[jj]) { break; }
						else if (kk < 2) {
							it = listCata.erase(it);
							pass = -1;
						}
					}
					if (pass < 0) { break; }
				}

				break;
			}
		}
		if (pass == 0) { it = listCata.erase(it); }
		else if (pass > 0) { ++it; }
	}
}
void CataFind::filterTopic(list<string>& listCata, CataFilter& cf, vector<string>& vsYear)
{
	int colIndex, pass;
	string tname;
	vector<string> vsResult;
	vector<string> search{ "DIM" };
	string orderby{ "DIMIndex DESC" };
	int numYear = (int)vsYear.size();
	for (auto it = listCata.begin(); it != listCata.end();) {
		pass = 0;
		for (int ii = 0; ii < numYear; ii++) {
			tname = "Census$" + vsYear[ii] + "$" + *it + "$DIMIndex";
			if (sf.tableExist(tname)) {
				pass = 1;
				vsResult.clear();
				colIndex = sf.selectOrderBy(search, tname, vsResult, orderby) - 1;

				if (cf.colTopic != "*") {
					// A column topic has been specified.
					if (vsResult[0] != cf.colTopic) {
						it = listCata.erase(it);
						pass = -1;
						break;
					}
				}
				if (cf.rowTopic != "*" && colIndex > 0) {
					// A row topic has been specified.
					if (vsResult[1] != cf.rowTopic) {
						it = listCata.erase(it);
						pass = -1;
						break;
					}
				}

				break;
			}
		}
		if (pass == 0) { it = listCata.erase(it); }
		else if (pass > 0) { ++it; }
	}
}
void CataFind::filterYear(list<string>& listCata, CataFilter& cf)
{
	unordered_set<string> setResult;
	auto failFilter = [&](string& sCata) {
		return (bool)!setResult.count(sCata);
	};

	vector<string> search{ "Catalogue" };
	string tname{ "Census$" + cf.year };
	sf.select(search, tname, setResult);
	listCata.remove_if(failFilter);
}
void CataFind::initList()
{
	vector<string> vsCata, vsYear;
	vector<string> search{ "Year" };
	string tname{ "Census" };
	string orderby{ "Year DESC" };
	int numYear = sf.selectOrderBy(search, tname, vsYear, orderby);
	if (numYear == 0) { err("No years found in Census-init"); }
	
	int numCata;
	search = { "Catalogue" };
	for (int ii = 0; ii < numYear; ii++) {
		tname = "Census$" + vsYear[ii];
		vsCata.clear();
		numCata = sf.select(search, tname, vsCata);
		for (int jj = 0; jj < numCata; jj++) {
			allCata.push_back(std::move(vsCata[jj]));
		}
	}
}
