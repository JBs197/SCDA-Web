#pragma once
#include <Wt/WPainter.h>
#include "jtree.h"
#include "wjcata.h"

class DataEvent
{
public:
	enum eType { Catalogue, Category, Connection, Demographic, Differentiation, Map, Parameter, Table, Topic, Tree, CatalogueList };

	const int numCata;
	const std::vector<std::string> vsCata;
	const std::vector<WJCATA> vwjCata;

	std::vector<WJCATA> getCataList() const { return vwjCata; }

	const JTREE tree;
	const std::vector<std::string> list, listCol, listRow, vsNamePop;
	const std::vector<std::vector<std::string>> catalogue, table, treeCol, treeRow, variable;
	const std::vector<std::string> ancestry;
	const std::vector<std::vector<std::vector<std::string>>> parameter;
	const std::string sYear, sCata, sRegion, title;
	const std::vector<Wt::WPainterPath> wpPaths;
	const std::vector<std::vector<std::vector<double>>> areas, frames;
	const std::vector<std::vector<double>> regionData;

	std::string getSessionID() { return sessionID; }
	std::vector<std::string> get_ancestry() const { return ancestry; }
	std::vector<std::vector<std::vector<double>>> getAreas() const { return areas; }
	std::vector<std::vector<std::string>> getCata() const { return catalogue; }
	std::vector<std::vector<std::string>> getCol() const { return treeCol; }
	std::vector<std::vector<std::vector<double>>> getFrames() const { return frames; }
	std::vector<std::string> get_list() const { return list; }
	std::vector<std::string> getListCol() const { return listCol; }
	std::vector<std::string> getListRow() const { return listRow; }
	std::vector<std::string> getNamePop() const { return vsNamePop; }
	int getNumCata() const { return numCata; }
	std::vector<std::vector<std::vector<std::string>>> getParameter() const { return parameter; }
	std::vector<std::vector<double>> getRegionData() const { return regionData; }
	std::vector<std::vector<std::string>> getRow() const { return treeRow; }
	std::string getSCata() const { return sCata; }
	std::string getSRegion() const { return sRegion; }
	std::string getSYear() const { return sYear; }
	std::vector<std::vector<std::string>> getTable() const { return table; }
	std::string getTitle() const { return title; }
	const JTREE getTree() const { return tree; }
	std::vector<std::vector<std::string>> getVariable() const { return variable; }
	std::vector<Wt::WPainterPath> get_wpPaths() const { return wpPaths; }
	int type() const { return etype; }

private:
	eType etype;
	std::string sessionID;

	// Constructor for eType Catalogue.
	DataEvent(eType et, const int& NumCata, const std::vector<std::string>& cata)
		: etype(et), numCata(NumCata), vsCata(cata) {}

	// Constructore for eType CatalogueList.
	DataEvent(eType et, const std::vector<WJCATA>& vCata)
		: etype(et), vwjCata(vCata), numCata(-1) {}

	// Constructor for eType Category.
	DataEvent(eType et, const std::string& sID, const int& nC, const std::vector<std::string>& vsList)
		: etype(et), sessionID(sID), numCata(nC), list(vsList) {}

	// Constructor for eType Connection.
	DataEvent(eType et, const std::string& sID)
		: etype(et), sessionID(sID), numCata(1) {}

	// Constructor for eType Demographic.
	DataEvent(eType et, const std::string& sID, const int& nC, const std::vector<std::vector<std::string>>& vvs)
		: etype(et), sessionID(sID), numCata(nC), variable(vvs) {}

	// Constructor for eType Differentiator.
	DataEvent(eType et, const std::string& sID, const int& nC, const std::vector<std::string>& vsList, const std::string& sTitle)
		: etype(et), sessionID(sID), numCata(nC), list(vsList), title(sTitle) {}

	// Constructor for eType Map.
	DataEvent(eType et, const std::string& sID, const std::vector<std::string>& vsRegion, const std::vector<std::vector<std::vector<double>>>& frame, const std::vector<std::vector<std::vector<double>>>& area, const std::vector<std::vector<double>>& rData)
		: etype(et), sessionID(sID), list(vsRegion), frames(frame), areas(area), regionData(rData), numCata(1) {}

	// Constructor for eType Parameter.
	DataEvent(eType et, const std::string& sID, const int& nC, const std::vector<std::vector<std::vector<std::string>>>& param, const std::vector<std::vector<std::string>>& cata)
		: etype(et), sessionID(sID), numCata(nC), parameter(param), catalogue(cata) {}

	// Constructor for eType Table.
	DataEvent(eType et, const std::string& sID, const std::vector<std::vector<std::string>>& vvsTable, const std::vector<std::vector<std::string>>& vvsCol, const std::vector<std::vector<std::string>>& vvsRow, const std::vector<std::string>& regionNamePop)
		: etype(et), sessionID(sID), table(vvsTable), treeCol(vvsCol), treeRow(vvsRow), vsNamePop(regionNamePop), numCata(1) {}

	// Constructor for eType Topic.
	DataEvent(eType et, const std::string& sID, const int& nC, const std::vector<std::string>& vsCol, const std::vector<std::string>& vsRow)
		: etype(et), sessionID(sID), numCata(nC), listCol(vsCol), listRow(vsRow) {}

	// Constructor for eType Tree.
	DataEvent(eType et, const std::string& sID, const JTREE& jt)
		: etype(et), sessionID(sID), numCata(1), tree(jt) {}

	friend class SCDAserver;
};

typedef std::function<void(const DataEvent&)> DataEventCallback;