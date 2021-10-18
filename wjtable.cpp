#include "wjtable.h"

void WJTABLE::cellSelect(int iRow, int iCol)
{
	Wt::WStandardItem* wsiCell = model->item(iRow, iCol);
	Wt::WModelIndex wmIndex = model->indexFromItem(wsiCell);
	cellSelect(wmIndex, iRow, iCol);
}
void WJTABLE::cellSelect(Wt::WModelIndex wmIndex)
{
	int iRow = wmIndex.row();
	int iCol = wmIndex.column();
	cellSelect(wmIndex, iRow, iCol);
}
void WJTABLE::cellSelect(Wt::WModelIndex wmIndex, int iRow, int iCol)
{
	// Un-highlight and un-border the previous selection, highlight and border this col/row, 
	// and then load a map using the col/row headers. 
	if (!wmIndex.isValid()) { jf.err("Invalid model index-wjtable.cellSelect"); }
	if (iRow >= model->rowCount() || iCol >= model->columnCount()) { jf.err("Index beyond maximum-wjtable.cellClicked"); }
	if (iRow < 0 || iCol < 0) { jf.err("Index below minimum-wjtable.cellClicked"); }

	string sID;
	string sProperty ="margin-left";
	string sValue0 = "0px";
	string sValue6 = "6px";
	Wt::WContainerWidget *wBox = nullptr;
	Wt::WWidget *wHeader = nullptr;
	Wt::WText* wText = nullptr;
	Wt::WModelIndex wmiTemp;
	Wt::WStandardItem* wsiTemp = nullptr;
	Wt::WFlags<Wt::ViewItemRenderFlag> wFlags(Wt::None);
	int numRow = model->rowCount();
	int numCol = model->columnCount();
	bool oneD = 0, first = 0;
	int selectedRow, selectedCol;
	if (selectedIndex.isValid()) 
	{ 
		selectedRow = selectedIndex.row();
		selectedCol = selectedIndex.column();
		if (iCol == 0) 
		{ 
			iCol = selectedCol;
			wsiTemp = model->item(iRow, iCol);
			wmIndex = model->indexFromItem(wsiTemp);
		}
	}
	else
	{
		first = 1;
		selectedRow = -1;
		selectedCol = -1;
	}

	// Ignore a new click on the same cell. 
	if (iRow == selectedRow && iCol == selectedCol) { return; }
	else if (iRow == selectedRow || iCol == selectedCol) { oneD = 1; }

	// Remove the previous border box.
	if (!first)
	{
		wText = (Wt::WText*)itemWidget(selectedIndex);
		wText->decorationStyle().setBorder(wbNone);
		wjDel->update(wText, selectedIndex, wFlags);

		wBox = dynamic_cast<Wt::WContainerWidget*>(wText->parent());
		wBox->setPadding(0.0, Wt::Side::Left);  // Remove the previous artificial padding.
		if (selectedCol < numCol - 1)
		{
			wsiTemp = model->item(selectedRow, selectedCol + 1);
			wmiTemp = model->indexFromItem(wsiTemp);
			wText = (Wt::WText*)itemWidget(wmiTemp);
			wBox = dynamic_cast<Wt::WContainerWidget*>(wText->parent());
			wBox->setPadding(0.0, Wt::Side::Left);
		}
	}

	// Apply a border box to the selected cell.
	wText = (Wt::WText*)itemWidget(wmIndex);
	wText->decorationStyle().setBorder(wbSelected);
	wjDel->update(wText, wmIndex, wFlags);

	// Apply fresh artificial padding.
	wBox = dynamic_cast<Wt::WContainerWidget*>(wText->parent());
	wBox->setPadding(6.0, Wt::Side::Left);  
	if (iCol < numCol - 1)
	{
		wsiTemp = model->item(iRow, iCol + 1);
		wmiTemp = model->indexFromItem(wsiTemp);
		wText = (Wt::WText*)itemWidget(wmiTemp);
		wBox = dynamic_cast<Wt::WContainerWidget*>(wText->parent());
		wBox->setPadding(6.0, Wt::Side::Left);
	}

	// Remove the previous row/column highlighting.
	if (iRow != selectedRow && selectedRow >= 0)
	{
		for (int ii = 0; ii < numCol; ii++)
		{
			if (oneD && ii == iCol) { continue; }
			wsiTemp = model->item(selectedRow, ii);
			wmiTemp = model->indexFromItem(wsiTemp);
			wText = (Wt::WText*)itemWidget(wmiTemp);
			wText->decorationStyle().setBackgroundColor(wcWhite);
			wjDel->update(wText, wmiTemp, wFlags);
		}
	}
	if (iCol != selectedCol && selectedCol >= 0)
	{
		for (int ii = 0; ii < numRow; ii++)
		{
			if (oneD && ii == iRow) { continue; }
			wsiTemp = model->item(ii, selectedCol);
			wmiTemp = model->indexFromItem(wsiTemp);
			wText = (Wt::WText*)itemWidget(wmiTemp);
			wText->decorationStyle().setBackgroundColor(wcWhite);
			wjDel->update(wText, wmiTemp, wFlags);
		}
		sID = mapHeaderID.at(selectedCol);
		wHeader = findById(sID);
		wBox = (Wt::WContainerWidget*)wHeader->parent()->parent()->parent();
		wBox->decorationStyle().setBackgroundColor(wcWhite);
		wBox->setPadding(0.0, Wt::Side::Left);
	}

	// Apply new row/column highlighting. 
	if (iRow != selectedRow)
	{ 
		for (int ii = 0; ii < numCol; ii++)
		{
			wsiTemp = model->item(iRow, ii);
			wmiTemp = model->indexFromItem(wsiTemp);
			wText = (Wt::WText*)itemWidget(wmiTemp);
			wText->decorationStyle().setBackgroundColor(wcSelectedWeak);
			wjDel->update(wText, wmiTemp, wFlags);
		}
	}
	if (iCol != selectedCol)
	{
		for (int ii = 0; ii < numRow; ii++)
		{
			wsiTemp = model->item(ii, iCol);
			wmiTemp = model->indexFromItem(wsiTemp);
			wText = (Wt::WText*)itemWidget(wmiTemp);
			wText->decorationStyle().setBackgroundColor(wcSelectedWeak);
			wjDel->update(wText, wmiTemp, wFlags);
		}
		sID = mapHeaderID.at(iCol);
		wHeader = findById(sID);
		wBox = (Wt::WContainerWidget*)wHeader->parent()->parent()->parent();
		wBox->decorationStyle().setBackgroundColor(wcSelectedWeak);
		wBox->setPadding(0.0, Wt::Side::Left);
	}

	// Update the saved selectedIndex.
	selectedIndex = wmIndex;
	scrollTo(wmIndex);
}
string WJTABLE::getCell(int iRow, int iCol)
{
	// For ease of access, an iRow value of "-1" will be interpreted as being the header's index.
	if (iRow < -1 || iCol < 0) { jf.err("Negative index-wjtable.getCell"); }
	if (iRow >= model->rowCount() || iCol >= model->columnCount()) { jf.err("Index beyond maximum-wjtable.getCell"); }	
	
	Wt::WString wsTemp;
	if (iRow == -1)
	{
		string headerID = mapHeaderID.at(iCol);
		Wt::WText* wText = dynamic_cast<Wt::WText*>(findById(headerID));
		wsTemp = wText->text();		
	}
	else
	{
		Wt::WStandardItem* wsiCell = model->item(iRow, iCol);
		wsTemp = wsiCell->text();	
	}
	string sCell = wsTemp.toUTF8();
	return sCell;
}
int WJTABLE::getColIndex(string sHeader)
{
	int index = -1;
	if (mapColIndex.count(sHeader))
	{
		index = mapColIndex.at(sHeader);
	}
	return index;
}
vector<int> WJTABLE::getRowColSel()
{
	// Return form [selectedRow, selectedCol] using MODEL indices.
	if (!selectedIndex.isValid()) { jf.err("Invalid selection-wjtable.getRowColSel"); }
	vector<int> viSel(2);
	viSel[0] = selectedIndex.row();
	viSel[1] = selectedIndex.column();
	return viSel;
}
int WJTABLE::getRowIndex(string sHeader)
{
	int index = -1;
	if (mapRowIndex.count(sHeader))
	{
		index = mapRowIndex.at(sHeader);
	}
	return index;
}
string WJTABLE::getUnit()
{
	string unit = getUnit(selectedIndex.row(), selectedIndex.column());
	return unit;
}
string WJTABLE::getUnit(int iRow, int iCol)
{
	string unit;
	if (mapColUnit.count(iCol))
	{
		unit = mapColUnit.at(iCol);
		return unit;
	}
	else if (mapRowUnit.count(iRow))
	{
		unit = mapRowUnit.at(iRow);
		return unit;
	}
	unit = "";
	return unit;
}
string WJTABLE::getUnit(string header)
{
	string unit;
	size_t pos1, pos2;
	if (header.back() == ')')
	{
		pos2 = header.size() - 1;
		pos1 = header.rfind('(', pos2 - 1) + 1;
		unit = header.substr(pos1, pos2 - pos1);
		if (pos2 - pos1 == 1) { return unit; }
	}
	unit = getUnitParser(header);
	return unit;
}
string WJTABLE::getUnitParser(string header)
{
	if (setUnitPercent.count(header)) { return "%"; }
	return "";
}
void WJTABLE::headerSelect(int iCol)
{
	// If the top-left cell was selected, then do nothing (functionality TBD). 
	// Otherwise, un-highlight a previous selection and highlight this column. 
	// Then, load a map using the col/row headers. 
	if (iCol == 0) { return; }
	if (!selectedIndex.isValid()) { jf.err("Invalid model index-wjtable.headerSelect"); }

	string sID;
	int selectedRow = selectedIndex.row();
	int selectedCol = selectedIndex.column();
	int numRow = model->rowCount();
	int numCol = model->columnCount();
	Wt::WContainerWidget* wBox = nullptr;
	Wt::WWidget* wHeader = nullptr, *wGreatgrandparent = nullptr;
	Wt::WText* wText = nullptr;
	Wt::WStandardItem* wsiTemp = nullptr;
	Wt::WFlags<Wt::ViewItemRenderFlag> wFlags(Wt::None);
	Wt::WModelIndex wmiTemp;

	// Ignore a new click on the same column. 
	if (iCol == selectedCol) { return; }

	// Remove the previous border box.
	if (selectedCol > 0)
	{
		wText = (Wt::WText*)itemWidget(selectedIndex);
		wText->decorationStyle().setBorder(wbNone);
		wjDel->update(wText, selectedIndex, wFlags);

		wBox = dynamic_cast<Wt::WContainerWidget*>(wText->parent());
		wBox->setPadding(0.0, Wt::Side::Left);  // Remove the previous artificial padding.
		if (selectedCol < numCol - 1)
		{
			wsiTemp = model->item(selectedRow, selectedCol + 1);
			wmiTemp = model->indexFromItem(wsiTemp);
			wText = (Wt::WText*)itemWidget(wmiTemp);
			wBox = dynamic_cast<Wt::WContainerWidget*>(wText->parent());
			wBox->setPadding(0.0, Wt::Side::Left);
		}
	}

	// Remove the previous column highlighting.
	if (selectedCol > 0)
	{
		for (int ii = 0; ii < numRow; ii++)
		{
			if (ii == selectedRow) { continue; }
			wsiTemp = model->item(ii, selectedCol);
			wmiTemp = model->indexFromItem(wsiTemp);
			wText = (Wt::WText*)itemWidget(wmiTemp);
			wText->decorationStyle().setBackgroundColor(wcWhite);
			wjDel->update(wText, wmiTemp, wFlags);
		}
		sID = mapHeaderID.at(selectedCol);
		wHeader = findById(sID);
		wGreatgrandparent = wHeader->parent()->parent()->parent();
		wGreatgrandparent->decorationStyle().setBackgroundColor(wcWhite);
	}

	// Apply new column highlighting.
	for (int ii = 0; ii < numRow; ii++)
	{
		wsiTemp = model->item(ii, iCol);
		wmiTemp = model->indexFromItem(wsiTemp);
		wText = (Wt::WText*)itemWidget(wmiTemp);
		wText->decorationStyle().setBackgroundColor(wcSelectedWeak);
		wjDel->update(wText, wmiTemp, wFlags);
	}
	sID = mapHeaderID.at(iCol);
	wHeader = findById(sID);
	wGreatgrandparent = wHeader->parent()->parent()->parent();
	wGreatgrandparent->decorationStyle().setBackgroundColor(wcSelectedWeak);

	// Apply a border box to the selected cell at the new row/col intersection.
	wsiTemp = model->item(selectedRow, iCol);
	wmiTemp = model->indexFromItem(wsiTemp);
	wText = (Wt::WText*)itemWidget(wmiTemp);
	wText->decorationStyle().setBorder(wbSelected);
	wjDel->update(wText, wmiTemp, wFlags);

	// Update the saved selectedIndex.
	selectedIndex = wmiTemp;

	// Apply fresh artificial padding.
	wBox = dynamic_cast<Wt::WContainerWidget*>(wText->parent());
	wBox->setPadding(6.0, Wt::Side::Left);
	if (iCol < numCol - 1)
	{
		wsiTemp = model->item(selectedRow, iCol + 1);
		wmiTemp = model->indexFromItem(wsiTemp);
		wText = (Wt::WText*)itemWidget(wmiTemp);
		wBox = dynamic_cast<Wt::WContainerWidget*>(wText->parent());
		wBox->setPadding(6.0, Wt::Side::Left);
	}
}
void WJTABLE::init(vector<vector<string>>& vvsCore, vector<vector<string>>& vvsCol, vector<vector<string>>& vvsRow, string& sRegion)
{
	initValues();
	initModel(vvsCore.size(), vvsCore[0].size() + 1);
	modelSetTopLeft(sRegion);
	modelSetTop(vvsCol);
	modelSetLeft(vvsRow);
	modelSetCore(vvsCore);

	// Signal/function connection.
	this->clicked().connect(this, std::bind(&WJTABLE::tableClicked, this, std::placeholders::_1, std::placeholders::_2));
	this->headerClicked().connect(this, std::bind(&WJTABLE::tableHeaderClicked, this, std::placeholders::_1, std::placeholders::_2));
	wjhDel->sigHeaderID().connect(this, std::bind(&WJTABLE::saveHeader, this, std::placeholders::_1, std::placeholders::_2));
}
void WJTABLE::initBlank()
{
	// For the server object.
	vector<vector<string>> vvsDummy = { {"0", "1"}};
	initValues();
	initModel(1, 2);
	modelSetTopLeft(vvsDummy[0][0]);
	modelSetTop(vvsDummy);
	modelSetLeft(vvsDummy);
	modelSetCore(vvsDummy);
}
void WJTABLE::initModel(int numRow, int numCol)
{
	wjDel = make_shared<WJDELEGATE>(heightCell);
	setItemDelegate(wjDel);
	wjhDel = make_shared<WJHDELEGATE>(heightHeader);
	setHeaderItemDelegate(wjhDel);
	model = make_shared<Wt::WStandardItemModel>(numRow, numCol);
	setModel(model);
	setSortingEnabled(0);
	setColumnResizeEnabled(1);
	setRowHeight(heightCell);
	setHeaderHeight(heightHeader);
	setPreloadMargin(60000.0);
	setOverflow(Wt::Overflow::Auto);
	for (int ii = 0; ii < numCol; ii++)
	{
		setHeaderWordWrap(ii, 1);
		setHeaderAlignment(ii, Wt::AlignmentFlag::Top);
		setColumnAlignment(ii, Wt::AlignmentFlag::Top);
		if (ii == 0) { setColumnWidth(ii, 200.0); }
		else { setColumnWidth(ii, 170.0); }
	}
}
void WJTABLE::initValues()
{
	// WColors used.
	wcSelectedWeak.setRgb(200, 200, 255);
	wcSelectedStrong.setRgb(150, 150, 192);
	wcWhite.setRgb(255, 255, 255);

	// CSS colours used.
	scSelectedWeak = "rgb(200, 200, 255)";
	scSelectedStrong = "rgb(150, 150, 192)";
	scWhite = "rgb(255, 255, 255)";

	// WLengths used.
	wlAuto = Wt::WLength::Auto;

	// WBorders used.
	wbNone = Wt::WBorder();
	wbSelected = Wt::WBorder(Wt::BorderStyle::Inset, Wt::BorderWidth::Medium, wcSelectedStrong);

	// Populate string list sets.
	setUnitBreaker.emplace(" ");
	setUnitBreaker.emplace("GIS");  // Guaranteed Income Supplement
	setUnitPercent.emplace("Percentage ");
	setUnitPercent.emplace(" percentage");
	setUnitPercent.emplace("Rate ");
	setUnitPercent.emplace(" rate");
}
void WJTABLE::modelSetCore(vector<vector<string>>& vvsCore)
{
	bool rowUnit;
	string unit, temp;
	for (int ii = 0; ii < vvsCore.size(); ii++)
	{
		unit.clear();
		if (mapRowUnit.count(ii)) 
		{ 
			unit = mapRowUnit.at(ii);
			rowUnit = 1;
		}
		else { rowUnit = 0; }
		for (int jj = 0; jj < vvsCore[ii].size(); jj++)
		{
			if (!rowUnit && mapColUnit.count(jj + 1)) { unit = mapColUnit.at(jj + 1); }
			else if (!rowUnit)  { unit = "# of persons"; }

			if (unit == "%") 
			{ 
				temp = jf.numericToCommaString(vvsCore[ii][jj], 1) + "\n(" + unit + ")";
			}
			else
			{
				temp = jf.numericToCommaString(vvsCore[ii][jj], 0) + "\n(" + unit + ")";
			}

			model->setData(ii, jj + 1, temp, Wt::ItemDataRole::Display);
		}
	}
}
void WJTABLE::modelSetLeft(vector<vector<string>>& vvsRow)
{
	// vvsRow has form [MID1, MID2, ...][MID, sVal, Ancestor0, ...]
	int count;
	for (int ii = 0; ii < vvsRow.size(); ii++)
	{
		mapRowIndex.emplace(vvsRow[ii][1], ii);
		setRowUnit(vvsRow[ii][1], ii);
		model->setData(ii, 0, vvsRow[ii][1], Wt::ItemDataRole::Display);
		count = 0;
		while (vvsRow[ii][1][count] == '+') { count++; }
		mapIndentRow.emplace(ii, count);
	}
}
void WJTABLE::modelSetTop(vector<vector<string>>& vvsCol)
{
	// vvsCol has form [MID1, MID2, ...][MID, sVal, Ancestor0, ...]
	int count;
	for (int ii = 0; ii < vvsCol.size(); ii++)
	{
		mapColIndex.emplace(vvsCol[ii][1], ii + 1);
		mapColValue.emplace(ii + 1, vvsCol[ii][1]);
		setColUnit(vvsCol[ii][1], ii + 1);
		model->setHeaderData(ii + 1, Wt::Orientation::Horizontal, vvsCol[ii][1], Wt::ItemDataRole::Display);
		count = 0;
		while (vvsCol[ii][1][count] == '+') { count++; }
		mapIndentCol.emplace(ii + 1, count);
	}
}
void WJTABLE::modelSetTopLeft(string sRegion)
{
	mapColValue.emplace(0, sRegion);
	model->setHeaderData(0, Wt::Orientation::Horizontal, sRegion, Wt::ItemDataRole::Display);
}
void WJTABLE::saveHeader(const int& iCol, const string& sID)
{
	mapHeaderID.emplace(iCol, sID);
}
void WJTABLE::setColUnit(string& colHeader, int index)
{
	// Map non-default units by index. 
	string unit;
	size_t pos1, pos2;
	if (colHeader.back() == ')')
	{
		pos2 = colHeader.size() - 1;
		pos1 = colHeader.rfind('(') + 1;
		unit = colHeader.substr(pos1, pos2 - pos1);
		if (!setUnitBreaker.count(unit)) { mapColUnit.emplace(index, unit); }
	}
	else
	{
		unit = getUnitParser(colHeader);
		if (unit.size() > 0) { mapColUnit.emplace(index, unit); }
	}
}
void WJTABLE::setProperty(Wt::WWidget* widget, string property, string value)
{
	// Load a widget's CSS style property list, and add/edit the given property to the value.
	Wt::WString wsStyle = widget->attributeValue("style");
	string sStyle = wsStyle.toUTF8();
	size_t pos1 = sStyle.find(property), pos2;
	if (pos1 > sStyle.size())
	{
		sStyle += " " + property + ": " + value + ";";
	}
	else
	{
		pos1 += property.size() + 2;
		pos2 = sStyle.find(';', pos1);
		sStyle.replace(pos1, pos2 - pos1, value);
	}
	wsStyle = Wt::WString::fromUTF8(sStyle);
	widget->setAttributeValue("style", wsStyle);
}
void WJTABLE::setProperty(Wt::WWidget* widget, vector<string> vsProperty, vector<string> vsValue)
{
	// Load a widget's CSS style property list, and add/edit the given properties 
	// to the values.
	if (vsProperty.size() != vsValue.size()) { jf.err("Parameter size mismatch-wjtable.setProperty"); }
	Wt::WString wsStyle = widget->attributeValue("style");
	string sStyle = wsStyle.toUTF8();
	size_t pos1, pos2;
	for (int ii = 0; ii < vsProperty.size(); ii++)
	{
		pos1 = sStyle.find(vsProperty[ii]);
		if (pos1 > sStyle.size())
		{
			sStyle += " " + vsProperty[ii] + ": " + vsValue[ii] + ";";
		}
		else
		{
			pos1 += vsProperty[ii].size() + 2;
			pos2 = sStyle.find(';', pos1);
			sStyle.replace(pos1, pos2 - pos1, vsValue[ii]);
		}
	}
	wsStyle = Wt::WString::fromUTF8(sStyle);
	widget->setAttributeValue("style", wsStyle);
}
void WJTABLE::setRowUnit(string& rowHeader, int index)
{
	// Map non-default units by index. 
	string unit;
	size_t pos1, pos2;
	if (rowHeader.back() == ')')
	{
		pos2 = rowHeader.size() - 1;
		pos1 = rowHeader.rfind('(') + 1;
		unit = rowHeader.substr(pos1, pos2 - pos1);
		pos1 = unit.find(' ');
		if (pos1 < unit.size()) { return; }
		if (!setUnitBreaker.count(unit)) { mapRowUnit.emplace(index, unit); }
	}
	else
	{
		unit = getUnitParser(rowHeader);
		if (unit.size() > 0) { mapRowUnit.emplace(index, unit); }
	}
}
void WJTABLE::tableClicked(const Wt::WModelIndex& wmIndex, const Wt::WMouseEvent& wmEvent)
{
	// Triggered in response to a user click.
	if (!wmIndex.isValid()) { jf.err("Invalid index from table click-wjtable.tableClicked"); }
	jf.timerStart();
	cellSelect(wmIndex);
	int iRow = wmIndex.row();
	int iCol = wmIndex.column();
	headerSignal_.emit(iRow, iCol);
}
void WJTABLE::tableHeaderClicked(const int& iCol, const Wt::WMouseEvent& wmEvent)
{
	// Triggered in response to a user click.
	jf.timerStart();
	headerSelect(iCol);
	int iRow = selectedIndex.row();
	headerSignal_.emit(iRow, iCol);
}

void WJTABLEBOX::addFilterBox()
{
	auto boxFilterUnique = make_unique<Wt::WContainerWidget>();
	auto pbRowUnique = make_unique<Wt::WPushButton>("Apply Row Filter");
	pbFilterRow = pbRowUnique.get();
	auto pbColUnique = make_unique<Wt::WPushButton>("Apply Column Filter");
	pbFilterCol = pbColUnique.get();

	auto hLayout = make_unique<Wt::WHBoxLayout>();
	hLayout->addWidget(move(pbRowUnique));
	hLayout->addWidget(move(pbColUnique));
	boxFilterUnique->setLayout(move(hLayout));

	Wt::WVBoxLayout *vLayout = (Wt::WVBoxLayout*)this->layout();
	vLayout->insertWidget(0, move(boxFilterUnique));
}
void WJTABLEBOX::addTipWidth()
{
	boxTip->clear();
	boxTip->setHidden(0);
	Wt::WFlags<Wt::Orientation> flags;
	Wt::WColor wcTip(255, 255, 180);
	boxTip->decorationStyle().setBackgroundColor(wcTip);
	boxTip->setPadding(0.0);
	auto tipLayout = make_unique<Wt::WGridLayout>();

	auto boxText = make_unique<Wt::WContainerWidget>();
	boxText->setPadding(0.0);
	auto text = make_unique<Wt::WText>();
	text->decorationStyle().font().setSize(Wt::FontSize::Small);
	text->decorationStyle().font().setWeight(Wt::FontWeight::Bold);
	Wt::WString wsTip = "Adjust any column width by dragging its column header edge left or right.";
	text->setText(wsTip);
	boxText->addWidget(move(text));
	tipLayout->addWidget(move(boxText), 0, 0, Wt::AlignmentFlag::Left | Wt::AlignmentFlag::Middle);

	auto button = make_unique<Wt::WPushButton>();
	button->setMinimumSize(25.0, 25.0);
	Wt::WCssDecorationStyle& buttonStyle = button->decorationStyle();
	buttonStyle.setBackgroundColor(Wt::StandardColor::White);
	buttonStyle.setBackgroundImage(linkIconClose, flags, Wt::Side::CenterX | Wt::Side::CenterY);
	button->clicked().connect(this, std::bind(&WJTABLEBOX::removeTipWidth, this));
	tipLayout->addWidget(move(button), 0, 1, Wt::AlignmentFlag::Right | Wt::AlignmentFlag::Middle);

	auto boxDummy = make_unique<Wt::WContainerWidget>();
	tipLayout->addWidget(move(boxDummy), 1, 0, Wt::AlignmentFlag::Left | Wt::AlignmentFlag::Top);
	tipLayout->setColumnStretch(0, 1);
	tipLayout->setRowStretch(1, 1);

	boxTip->setLayout(move(tipLayout));
}
void WJTABLEBOX::init()
{
	vviFilter.resize(2, vector<int>());

	setPadding(2.0);
	auto vLayout = make_unique<Wt::WVBoxLayout>();
	auto boxTableUnique = make_unique<Wt::WContainerWidget>();
	boxTable = boxTableUnique.get();
	vLayout->addWidget(move(boxTableUnique));

	auto boxTipUnique = make_unique<Wt::WContainerWidget>();
	boxTip = boxTipUnique.get();
	boxTip->setHidden(1);
	vLayout->addWidget(move(boxTipUnique));

	vLayout->addStretch(1);
	this->setLayout(move(vLayout));
}
string WJTABLEBOX::makeCSV()
{
	// Return the active data table as a CSV string. Indentations are marked by two blank spaces.
	string sCSV, temp;
	int numRow = vvsRow.size();
	int numCol = vvsCol.size();
	vector<string> dirt = { "+" }, soap = { "  " };

	// Write the column headers. 
	sCSV = "\"" + sRegion + "\"";
	for (int ii = 0; ii < numCol; ii++)
	{
		temp = vvsCol[ii][1];
		jf.clean(temp, dirt, soap);
		sCSV += ",\"" + temp + "\"";
	}
	sCSV += "\n";

	// Write the row headers and row values.
	Wt::WStandardItem* wsiTemp = nullptr;
	Wt::WString wsTemp;
	for (int ii = 0; ii < numRow; ii++)
	{
		temp = vvsRow[ii][1];
		jf.clean(temp, dirt, soap);
		sCSV += "\"" + temp + "\"";
		for (int jj = 0; jj < numCol; jj++)
		{
			sCSV += "," + vvsCore[ii][jj];
		}
		sCSV += "\n";
	}
	return sCSV;
}
void WJTABLEBOX::removeTipWidth()
{
	tipSignal_.emit("tableWidth");
	boxTip->clear();
	boxTip->setHidden(1);
}
WJTABLE* WJTABLEBOX::setTable(vector<vector<string>>& core, vector<vector<string>>& col, vector<vector<string>>& row, string& region)
{
	if (boxTable != nullptr) { boxTable->clear(); }
	vvsCore = core;
	vvsCol = col;
	vvsRow = row;
	sRegion = region;
	auto tableUnique = make_unique<WJTABLE>(vvsCore, vvsCol, vvsRow, sRegion);
	return boxTable->addWidget(move(tableUnique));
}
void WJTABLEBOX::widgetMobile(bool mobile)
{
	if (mobile)
	{
		if (pbFilterRow != nullptr)
		{
			pbFilterRow->decorationStyle().font().setSize(Wt::FontSize::XXLarge);
			pbFilterRow->setMinimumSize(wlAuto, 50.0);
		}
		if (pbFilterCol != nullptr)
		{
			pbFilterCol->decorationStyle().font().setSize(Wt::FontSize::XXLarge);
			pbFilterCol->setMinimumSize(wlAuto, 50.0);
		}
	}
	else
	{
		if (pbFilterRow != nullptr)
		{
			pbFilterRow->decorationStyle().font().setSize(Wt::FontSize::Medium);
			pbFilterRow->setMinimumSize(wlAuto, wlAuto);
		}
		if (pbFilterCol != nullptr)
		{
			pbFilterCol->decorationStyle().font().setSize(Wt::FontSize::Medium);
			pbFilterCol->setMinimumSize(wlAuto, wlAuto);
		}
	}

}
