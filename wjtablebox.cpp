#include "wjtablebox.h"

using namespace std;

WJTABLEBOX::WJTABLEBOX() : WContainerWidget()
{
	modelTable = make_shared<Wt::WStandardItemModel>();
	vvvsTable = make_shared<vector<vector<vector<string>>>>();
	colHeader = make_shared<JTREE>();
	rowHeader = make_shared<JTREE>();

	initGUI();
}

void WJTABLEBOX::configure(string& configXML)
{
	unordered_map<string, string> mapTag;
	vector<string> vsTag{ "settings", "table" };
	jparse.getXML(mapTag, configXML, vsTag);
	string cssCell, cssColHeader, cssRowHeader, cssTopLeft, temp;
	
	auto mainLayout = (Wt::WVBoxLayout*)this->layout();
	auto wlItem = mainLayout->itemAt(indexMain::Table);
	auto box = (Wt::WContainerWidget*)wlItem->widget();
	auto boxLayout = (Wt::WVBoxLayout*)box->layout();
	wlItem = boxLayout->itemAt(0);
	auto table = (WJTABLEVIEW*)wlItem->widget();
	
	try {
		cssCell = mapTag.at("cell");
		cssColHeader = mapTag.at("col_header");
		cssRowHeader = mapTag.at("row_header");
		cssTopLeft = mapTag.at("top_left");
		temp = mapTag.at("row_height");
		table->rowHeight = stod(temp);
		temp = mapTag.at("header_height");
		table->headerHeight = stod(temp);
		temp = mapTag.at("col_width");
		table->colWidth = stod(temp);
		temp = mapTag.at("header_width");
		table->headerWidth = stod(temp);
		temp = mapTag.at("preload_margin");
		table->preloadMargin = stod(temp);
	}
	catch (out_of_range) { err("mapTag-configure"); }
	catch (invalid_argument) { err("height stod-configure"); }

	table->initCellDelegate(table->rowHeight, cssCell, cssRowHeader, cssColHeader, cssTopLeft);
	table->setCellSize();
}
void WJTABLEBOX::err(string message)
{
	string errorMessage = "WJTABLEBOX error:\n" + message;
	JLOG::getInstance()->err(errorMessage);
}
void WJTABLEBOX::initGUI()
{
	Wt::WLength wlAuto = Wt::WLength::WLength();

	auto vLayoutUnique = make_unique<Wt::WVBoxLayout>();
	auto vLayout = this->setLayout(std::move(vLayoutUnique));

	auto boxUnitPinUnique = make_unique<Wt::WContainerWidget>();
	auto boxUnitPin = vLayout->insertWidget(indexMain::UnitPin, std::move(boxUnitPinUnique));
	boxUnitPin->setStyleClass("boxpin");
	auto hLayoutUnique = make_unique<Wt::WHBoxLayout>();
	auto hLayout = boxUnitPin->setLayout(std::move(hLayoutUnique));

	auto unitLabelUnique = make_unique<Wt::WText>("None");
	auto unitLabel = hLayout->insertWidget(indexUnitPin::UnitLabel, std::move(unitLabelUnique));

	auto pbUnitUnique = make_unique<Wt::WPushButton>();
	auto pbUnit = hLayout->insertWidget(indexUnitPin::UnitButton, std::move(pbUnitUnique));
	pbUnit->setStyleClass("pbunit");

	hLayout->insertStretch(indexUnitPin::Stretch, 1);

	auto pbPinRowUnique = make_unique<Wt::WPushButton>("Pin Row Data To Bar Graph");
	auto pbPinRow = hLayout->insertWidget(indexUnitPin::PinRow, std::move(pbPinRowUnique));
	pbPinRow->setStyleClass("pbpin");

	auto pbPinColUnique = make_unique<Wt::WPushButton>("Pin Column Data To Bar Graph");
	auto pbPinCol = hLayout->insertWidget(indexUnitPin::PinColumn, std::move(pbPinColUnique));
	pbPinCol->setStyleClass("pbpin");

	auto pbResetUnique = make_unique<Wt::WPushButton>("Reset Bar Graph");
	auto pbReset = hLayout->insertWidget(indexUnitPin::Reset, std::move(pbResetUnique));
	pbReset->setStyleClass("pbpin");

	auto boxTableUnique = make_unique<Wt::WContainerWidget>();
	auto boxTable = vLayout->insertWidget(indexMain::Table, std::move(boxTableUnique));
	boxTable->setMaximumSize(wlAuto, wlAuto);
	auto tableLayoutUnique = make_unique<Wt::WVBoxLayout>();
	auto tableLayout = boxTable->setLayout(std::move(tableLayoutUnique));
	auto wjTableUnique = make_unique<WJTABLEVIEW>();
	auto wjTable = tableLayout->addWidget(std::move(wjTableUnique));
	wjTable->setModel(modelTable);

	auto boxTipUnique = make_unique<Wt::WContainerWidget>();
	auto boxTip = vLayout->insertWidget(indexMain::Tip, std::move(boxTipUnique));
	boxTip->setStyleClass("boxtip");
	auto gLayoutUnique = make_unique<WJGRIDLAYOUT>();
	auto gLayout = boxTip->setLayout(std::move(gLayoutUnique));

	auto tipUnique = make_unique<Wt::WText>("Tip: Adjust any column width by dragging its column header edge left or right.");	
	auto tip = gLayout->addWidget(std::move(tipUnique), 0, 0, 2, 1);

	auto pbTipUnique = make_unique<Wt::WPushButton>();
	auto pbTip = gLayout->addWidget(std::move(pbTipUnique), 0, 1);

	gLayout->setColumnStretch(0, 1);
	gLayout->setRowStretch(1, 1);

}
void WJTABLEBOX::initTableAll(const vector<string>& vsGeoCode, const vector<vector<vector<string>>>& table)
{
	// Receive table data from the server, for the parent region and its immediate children.
	int numRegion = (int)vsGeoCode.size();
	if (numRegion != table.size()) { err("Parameter size mismatch-setTableAll"); }

	// Move the raw data from the server event, building the GeoCode map as we go. 
	int geoCode;
	mapGeoCode.clear();
	vvvsTable.reset();
	vvvsTable = make_shared<vector<vector<vector<string>>>>();
	vvvsTable->resize(numRegion, vector<vector<string>>());
	for (int ii = 0; ii < numRegion; ii++) {
		try { geoCode = stoi(vsGeoCode[ii]); }
		catch (invalid_argument) { err("GeoCode stoi-setTableAll"); }
		mapGeoCode.emplace(geoCode, ii);

		vvvsTable->at(ii) = std::move(table[ii]);
	}
}
void WJTABLEBOX::populateHeader()
{
	// Use the row/column JTREEs to populate the model. 

	int index, nodeID, numRow;
	int numCol = colHeader->numNode();
	if (modelTable->columnCount() != numCol) { err("numCol mismatch-populateHeader"); }
	for (int ii = 1; ii < numCol; ii++) {
		nodeID = colHeader->getID(ii);
		JNODE& jn = colHeader->getNode(nodeID);
		try { index = stoi(jn.vsData[1]); }
		catch (invalid_argument) { err("col index stoi-populateHeader"); }
		modelTable->setHeaderData(index, Wt::Orientation::Horizontal, jn.vsData[0], Wt::ItemDataRole::Display);
	}

	if (rowTopic.size() > 0) {
		numRow = rowHeader->numNode();
		for (int ii = 1; ii < numRow; ii++) {
			nodeID = rowHeader->getID(ii);
			JNODE& jn = rowHeader->getNode(nodeID);
			try { index = stoi(jn.vsData[1]); }
			catch (invalid_argument) { err("row index stoi-populateHeader"); }
			modelTable->setData(index - 1, 0, jn.vsData[0], Wt::ItemDataRole::Display);
		}
	}


}
void WJTABLEBOX::populateTable(int index)
{
	// Use the table at the given index to populate the model.

	unique_ptr<Wt::WStandardItem> wsiCell;
	int numRow = (int)vvvsTable->at(index).size();
	if (numRow == 0) { err("No rows in vvvsTable-initModel"); }
	int numCol = (int)vvvsTable->at(index)[0].size();
	if (numCol == 0) { err("No columns in vvvsTable-initModel"); }
	int headerShift{ 0 };
	if (rowTopic.size() > 0) { headerShift = 1; }

	modelTable->clear();
	for (int ii = 0; ii < numRow; ii++) {
		for (int jj = 0; jj < numCol; jj++) {
			wsiCell = make_unique<Wt::WStandardItem>(vvvsTable->at(index)[ii][jj]);

			modelTable->setItem(ii, headerShift + jj, std::move(wsiCell));
		}
	}
	
	auto mainLayout = (Wt::WVBoxLayout*)this->layout();
	auto wlItem = mainLayout->itemAt(indexMain::Table);
	auto box = (Wt::WContainerWidget*)wlItem->widget();
	auto boxLayout = (Wt::WVBoxLayout*)box->layout();
	wlItem = boxLayout->itemAt(0);
	auto table = (WJTABLEVIEW*)wlItem->widget();
	table->setCellSize();
}
void WJTABLEBOX::selectRegion(pair<int, string> selRegion)
{
	// Load the selected region's table data into the model.
	int indexTable{ -1 };
	int geoCode = get<0>(selRegion);
	if (geoCode < 0) { indexTable = 0; }
	else if (mapGeoCode.count(geoCode)) { indexTable = mapGeoCode.at(geoCode); }
	else { err("mapGeoCode-selectRegion"); }
	populateTable(indexTable);

	// Display the region's name in the top-left table cell.
	modelTable->setHeaderData(0, Wt::Orientation::Horizontal, get<1>(selRegion), Wt::ItemDataRole::Display);
}
void WJTABLEBOX::setHeader(const vector<vector<vector<string>>>& vvvsMID)
{
	// Receive raw MID tables from the database, and use them to build JTREEs describing
	// the hierarchy of row/column header titles for the table.
	int numMID = (int)vvvsMID.size();
	if (numMID == 0) { err("No MIDs given-setHeader"); }
	
	unordered_map<int, int> mapColumn;
	mapColumn.emplace(0, 1);
	mapColumn.emplace(1, 0);
	
	colHeader.reset();
	colHeader = make_shared<JTREE>();
	int numFailure = colHeader->populateFromSQL(vvvsMID[numMID - 1], mapColumn);
	if (numFailure > 0) { err("colHeader populateFromSQL-setHeader"); }

	rowHeader.reset();
	rowHeader = make_shared<JTREE>();
	if (numMID > 1) {
		numFailure = rowHeader->populateFromSQL(vvvsMID[numMID - 2], mapColumn);
		if (numFailure > 0) { err("rowHeader populateFromSQL-setHeader"); }
	}

	populateHeader();
}
void WJTABLEBOX::setTopic(const vector<vector<string>>& vvsDIM)
{
	// Receive the row/column topics from the database.
	int numDIM = (int)vvsDIM.size();
	if (numDIM == 0) { err("No DIMs given-setTopic"); }

	colTopic = vvsDIM[numDIM - 1][1];
	if (numDIM > 1) { rowTopic = vvsDIM[numDIM - 2][1]; }
	else { rowTopic = ""; }
}
