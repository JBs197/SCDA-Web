#include "wjtable.h"

void WJCELL::initText(string text, JFUNC& jf, Wt::WFont& wfCell)
{
	sText = text;
	vector<string> dirt = { " " }, soap = { "\n" };
	jf.clean(text, dirt, soap);
	Wt::WString wsTemp = Wt::WString::fromUTF8(text);
	Wt::WSvgImage wsiScratchpad(200, 200);
	Wt::WPainter wPainter(&wsiScratchpad);
	auto wTextItem = wsiScratchpad.measureText(wsTemp);
	minWidth = int(ceil(wTextItem.width()));
}
void WJCELL::paintEvent(Wt::WPaintDevice* paintDevice) {}
void WJCELL::setTL(int x, int y)
{
	xTL = x;
	yTL = y;
}

void WJTABLE::cellClicked(int iRow, int iCol)
{
	// Triggered in response to a user click. Note that the col/row headers have indices "-1".
	jf.timerStart();

	// Highlighting/border boxes.
	if (iCol >= 0 && iRow >= 0)
	{
		if (selectedCol >= 0 && selectedRow >= 0)
		{
			tableData->elementAt(selectedRow, selectedCol)->decorationStyle().setBorder(wbNone);
		}
		tableData->elementAt(iRow, iCol)->decorationStyle().setBorder(wbSelectedCell);
	}
	if (iCol >= 0) { colSelect(iCol); }
	if (iRow >= 0) { rowSelect(iRow); }

	// SCDAwidget combobox update and map creation.
	wsigCellSel_.emit(iRow, iCol, activeRegion);
}
void WJTABLE::clear()
{
	activeColTitle.clear();
	activeRegion.clear();
	activeRowTitle.clear();

	mapColIndex.clear();
	mapRowIndex.clear();
	mapColUnit.clear();
	mapRowUnit.clear();

	numCol = -2;
	numRow = -2;
	selectedCol = -2;
	selectedRow = -2;
	
	vvcTable.clear();

	/*
	if (textRegion != nullptr)
	{
		textRegion->~WText();
		textRegion = nullptr;
	}
	if (tableColHeader != nullptr)
	{
		tableColHeader->clear();
		tableColHeader = nullptr;
	}
	if (tableData != nullptr)
	{
		tableData->clear();
		tableData = nullptr;
	}
	if (tableRowHeader != nullptr)
	{
		tableRowHeader->clear();
		tableRowHeader = nullptr;
	}
	*/

}
void WJTABLE::colDeselect()
{
	if (selectedCol < 0) { return; }
	if (numRow < 0) { jf.err("Rows not initialized-wjtable.colDeselect"); }
	tableColHeader->elementAt(0, selectedCol)->decorationStyle().setBackgroundColor(wcWhite);
	for (int ii = 0; ii < numRow; ii++)
	{
		tableColHeader->elementAt(ii, selectedCol)->decorationStyle().setBackgroundColor(wcWhite);
	}
	selectedCol = -2;
}
void WJTABLE::colSelect(int colIndex)
{
	if (numRow < 0) { jf.err("Rows not initialized-wjtable.colSelect"); }
	if (colIndex >= numCol) { jf.err("Invalid column index-wjtable.colSelect"); }

	// If a different column had been previously selected, deselect it. 
	if (colIndex != selectedCol) { colDeselect(); }

	// Highlight the newly-selected column and save its index.
	if (colIndex >= 0)
	{
		tableColHeader->elementAt(0, colIndex)->decorationStyle().setBackgroundColor(wcSelectedWeak);
		for (int ii = 0; ii < numRow; ii++)
		{
			tableData->elementAt(ii, colIndex)->decorationStyle().setBackgroundColor(wcSelectedWeak);
		}
	}

	selectedCol = colIndex;
}
int WJTABLE::columnCount()
{
	return numCol;
}
void WJTABLE::display()
{

}
vector<string> WJTABLE::elementAt(int rowIndex, int colIndex)
{
	// Return form [cell value, cell unit]. 
	if (rowIndex >= numRow || colIndex >= numCol) { jf.err("Invalid row/col input-wjtable.elementAt"); }
	vector<string> vsCell(2);
	Wt::WText* wText = (Wt::WText*)tableData->elementAt(rowIndex, colIndex)->widget(0);
	Wt::WString wsTemp = wText->text();
	string temp = wsTemp.toUTF8();
	size_t pos1 = temp.find('\n');
	if (pos1 > temp.size())
	{
		vsCell[0] = temp;
		vsCell.resize(1);
	}
	else
	{
		vsCell[0] = temp.substr(0, pos1);
		vsCell[1] = temp.substr(pos1 + 1);
	}
	return vsCell;
}

vector<string> WJTABLE::getCol(int iCol)
{
	// Returns the entire column, including the header. 
	if (numCol < 0 || numRow < 0) { jf.err("Columns/rows not initialized-wjtable.getCol"); }
	if (iCol >= numCol || iCol < -1) { jf.err("Invalid column index-wjtable.getCol"); }
	vector<string> vsCol(numRow + 1);
	Wt::WText* wText = nullptr;
	Wt::WString wsTemp;
	if (iCol == -1)
	{
		vsCol[0] = activeRegion;
		for (int ii = 0; ii < numRow; ii++)
		{
			wText = (Wt::WText*)tableRowHeader->elementAt(ii, 0)->widget(0);
			wsTemp = wText->text();
			vsCol[ii + 1] = wsTemp.toUTF8();
		}
	}
	else
	{
		wText = (Wt::WText*)tableColHeader->elementAt(0, iCol)->widget(0);
		wsTemp = wText->text();
		vsCol[0] = wsTemp.toUTF8();
		for (int ii = 0; ii < numRow; ii++)
		{
			wText = (Wt::WText*)tableData->elementAt(ii, iCol)->widget(0);
			wsTemp = wText->text();
			vsCol[ii + 1] = wsTemp.toUTF8();
		}
	}
	return vsCol;
}
string WJTABLE::getColHeader(int iCol)
{
	Wt::WText* wText = (Wt::WText*)tableColHeader->elementAt(0, iCol)->widget(0);
	Wt::WString wTemp = wText->text();
	string sCol = wTemp.toUTF8();
	if (sCol.size() < 1) { jf.err("No column header found-wjtable.getColHeader"); }
	return sCol;
}
int WJTABLE::getColIndex(string colHeader)
{
	if (!mapColIndex.count(colHeader)) { return -1; }
	int colIndex = mapColIndex.at(colHeader);
	return colIndex;
}
string WJTABLE::getRegion()
{
	if (activeRegion.size() < 1) { jf.err("No region loaded-wjtable.getRegion"); }
	return activeRegion;
}
vector<string> WJTABLE::getRow(int iRow)
{
	// Returns the entire row, including the header. 
	if (numCol < 0 || numRow < 0) { jf.err("Columns/rows not initialized-wjtable.getRow"); }
	if (iRow >= numRow || iRow < -1) { jf.err("Invalid rown index-wjtable.getRow"); }
	vector<string> vsRow(numCol + 1);
	Wt::WText* wText = nullptr;
	Wt::WString wsTemp;
	if (iRow == -1)
	{
		vsRow[0] = activeRegion;
		for (int ii = 0; ii < numCol; ii++)
		{
			wText = (Wt::WText*)tableColHeader->elementAt(0, ii)->widget(0);
			wsTemp = wText->text();
			vsRow[ii + 1] = wsTemp.toUTF8();
		}
	}
	else
	{
		wText = (Wt::WText*)tableRowHeader->elementAt(iRow, 0)->widget(0);
		wsTemp = wText->text();
		vsRow[0] = wsTemp.toUTF8();
		for (int ii = 0; ii < numCol; ii++)
		{
			wText = (Wt::WText*)tableData->elementAt(iRow, ii)->widget(0);
			wsTemp = wText->text();
			vsRow[ii + 1] = wsTemp.toUTF8();
		}
	}
	return vsRow;
}
string WJTABLE::getRowHeader(int iRow)
{
	Wt::WText* wText = (Wt::WText*)tableRowHeader->elementAt(iRow, 0)->widget(0);
	Wt::WString wTemp = wText->text();
	string sRow = wTemp.toUTF8();
	if (sRow.size() < 1) { jf.err("No row header found-wjtable.getRowHeader"); }
	return sRow;
}
int WJTABLE::getRowIndex(string rowHeader)
{
	if (!mapRowIndex.count(rowHeader)) { return -1; }
	int rowIndex = mapRowIndex.at(rowHeader);
	return rowIndex;
}
string WJTABLE::getUnit()
{
	string unit = getUnit(selectedRow, selectedCol);
	return unit;
}
string WJTABLE::getUnit(int iRow, int iCol)
{
	string unit;
	if (iCol > 0 && mapColUnit.count(iCol))
	{
		unit = mapColUnit.at(iCol);
		return unit;
	}
	else if (iRow > 0 && mapRowUnit.count(iRow))
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
		pos1 = unit.find(' ');
		if (pos1 > unit.size()) { return unit; }
	}
	unit = getUnitParser(header);
	return unit;
}
string WJTABLE::getUnitParser(string header)
{
	size_t pos1;
	vector<string> vsPercent = { "Percentage", "percentage", " rate", "Rate " };
	for (int ii = 0; ii < vsPercent.size(); ii++)
	{
		pos1 = header.find(vsPercent[ii]);
		if (pos1 < header.size()) { return "%"; }
	}
	return "";
}

void WJTABLE::init()
{
	// Internal widgets.
	auto boxRegionUnique = make_unique<Wt::WContainerWidget>();
	boxRegionUnique->setPositionScheme(Wt::PositionScheme::Relative);
	boxRegion = boxRegionUnique.get();
	this->addWidget(move(boxRegionUnique));

	auto boxColHeaderUnique = make_unique<Wt::WContainerWidget>();
	boxColHeaderUnique->setPositionScheme(Wt::PositionScheme::Relative);
	boxColHeader = boxColHeaderUnique.get();
	this->addWidget(move(boxColHeaderUnique));

	auto boxRowHeaderUnique = make_unique<Wt::WContainerWidget>();
	boxRowHeaderUnique->setPositionScheme(Wt::PositionScheme::Relative);
	boxRowHeader = boxRowHeaderUnique.get();
	this->addWidget(move(boxRowHeaderUnique));

	auto boxDataUnique = make_unique<Wt::WContainerWidget>();
	boxDataUnique->setPositionScheme(Wt::PositionScheme::Relative);
	boxData = boxDataUnique.get();
	this->addWidget(move(boxDataUnique));

	// WColors used.
	wcSelectedWeak.setRgb(200, 200, 255);
	wcSelectedStrong.setRgb(150, 150, 192);
	wcWhite.setRgb(255, 255, 255);

	// WLengths used.
	wlAuto = Wt::WLength::Auto;
	wlCellPadding = Wt::WLength(4.0, Wt::LengthUnit::Pixel);
	wlRowHeaderMin = Wt::WLength(100.0, Wt::LengthUnit::Pixel);  // Row header's minimum width.

	// WBorders used.
	wbNone = Wt::WBorder();
	wbSelectedCell = Wt::WBorder(Wt::BorderStyle::Inset, Wt::BorderWidth::Medium, wcSelectedStrong);

	// WFonts used.
	wfCell = Wt::WFont(Wt::FontFamily::SansSerif);

	// Initialize this table's lastClick timer.
	jf.timerStart();
}
void WJTABLE::initColHeader(vector<string>& vsCol)
{
	// Note that vsCol[0] is the top-left table cell, showing region. 
	activeColTitle = vsCol.back();
	vsCol.pop_back();
	numCol = vsCol.size();

	vvcTable.push_back(vector<unique_ptr<WJCELL>>(numCol));
	activeRegion = vsCol[0];
	string sHeader = "Region:\n" + vsCol[0];
	auto wjCell = make_unique<WJCELL>();
	wjCell->initText(sHeader, jf, wfCell);
	vvcTable[0][0] = move(wjCell);

	for (int ii = 1; ii < numCol; ii++)
	{
		mapColIndex.emplace(vsCol[ii], ii);
		setColUnit(vsCol[ii], ii);
		wjCell = make_unique<WJCELL>();
		wjCell->initText(vsCol[ii], jf, wfCell);
		function<void()> fnColSel = bind(&WJTABLE::cellClicked, this, 0, ii);
		wjCell->clicked().connect(fnColSel);
		vvcTable[0][ii] = move(wjCell);
	}
}
void WJTABLE::initData(vector<vector<string>>& vvsData)
{
	string sUnit, sValue, temp;
	bool colUnit, rowUnit, success;
	double tolerance = 1.01;
	for (int ii = 1; ii <= vvsData.size(); ii++)
	{		
		if (mapRowUnit.count(ii)) 
		{ 
			sUnit = mapRowUnit.at(ii);
			rowUnit = 1;
		}
		else { rowUnit = 0; }
		for (int jj = 1; jj <= vvsData[ii - 1].size(); jj++)
		{			
			colUnit = 0;
			if (!rowUnit && mapColUnit.count(jj)) 
			{ 
				sUnit = mapColUnit.at(jj);
				colUnit = 1;
			}
			else if (!rowUnit) { sUnit = "# of persons"; }

			if (colUnit && sUnit == "%")
			{
				success = jf.checkPercent(vvsData[ii - 1][jj - 1]);
				if (!success) { jf.err("Failed checkPercent-wjtable.initData"); }
			}

			sValue = jf.numericToCommaString(vvsData[ii - 1][jj - 1]);
			temp = sValue + "\n" + sUnit;
			auto wjCell = make_unique<WJCELL>();
			function<void()> fnDataSel = bind(&WJTABLE::cellClicked, this, ii, jj);
			wjCell->clicked().connect(fnDataSel);
			vvcTable[ii][jj] = move(wjCell);
		}
		if (rowUnit && sUnit == "%")
		{
			success = jf.checkPercent(vvsData[ii - 1], tolerance);
			if (!success) { jf.err("Failed checkPercent-wjtable.initData"); }
		}
	}
}
void WJTABLE::initRowHeader(vector<string>& vsRow)
{
	// Note that vsRow[0] is the first data row's title (it is beneath the "region" cell).
	activeRowTitle = vsRow.back();
	vsRow.pop_back();
	numRow = vsRow.size() + 1;
	vvcTable.resize(numRow);

	for (int ii = 1; ii < numRow; ii++)
	{
		vvcTable[ii].resize(numCol);
		mapRowIndex.emplace(vsRow[ii - 1], ii);
		setRowUnit(vsRow[ii - 1], ii);
		auto wjCell = make_unique<WJCELL>();
		function<void()> fnRowSel = bind(&WJTABLE::cellClicked, this, ii, 0);
		wjCell->clicked().connect(fnRowSel);
		vvcTable[ii][0] = move(wjCell);
	}
}

void WJTABLE::layoutSizeChanged(int iWidth, int iHeight)
{
	width = iWidth;
	height = iHeight;

}

int WJTABLE::rowCount()
{
	return numRow;
}
void WJTABLE::rowDeselect()
{
	if (selectedRow < 0) { return; }
	if (numCol < 0) { jf.err("Columns not initialized-wjtable.rowDeselect"); }
	tableRowHeader->elementAt(0, selectedRow)->decorationStyle().setBackgroundColor(wcWhite);
	for (int ii = 0; ii < numCol; ii++)
	{
		tableRowHeader->elementAt(selectedRow, ii)->decorationStyle().setBackgroundColor(wcWhite);
	}
	selectedRow = -2;
}
void WJTABLE::rowSelect(int rowIndex)
{
	if (numCol < 0) { jf.err("Columns not initialized-wjtable.rowSelect"); }
	if (rowIndex >= numRow) { jf.err("Invalid row index-wjtable.rowSelect"); }

	// If a different row had been previously selected, deselect it. 
	if (rowIndex != selectedRow) { rowDeselect(); }

	// Highlight the newly-selected row and save its index.
	if (rowIndex >= 0)
	{
		tableRowHeader->elementAt(rowIndex, 0)->decorationStyle().setBackgroundColor(wcSelectedWeak);
		for (int ii = 0; ii < numCol; ii++)
		{
			tableData->elementAt(rowIndex, ii)->decorationStyle().setBackgroundColor(wcSelectedWeak);
		}
	}
	
	selectedRow = rowIndex;
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
		pos1 = unit.find(' ');
		if (pos1 > unit.size()) { mapColUnit.emplace(index, unit); }		
	}
	else
	{
		unit = getUnitParser(colHeader);
		if (unit.size() > 0) { mapColUnit.emplace(index, unit); }
	}
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
		if (pos1 > unit.size()) { mapRowUnit.emplace(index, unit); }		
	}
	else
	{
		unit = getUnitParser(rowHeader);
		if (unit.size() > 0) { mapRowUnit.emplace(index, unit); }
	}
}
void WJTABLE::updateColMinWidth()
{
	if (numCol < 2 || numRow < 2) { jf.err("Insufficient rows/columns-wjtable.updateColMinWidth"); }
	colMinWidth.assign(numCol, 0);
	for (int ii = 0; ii < numCol; ii++)
	{
		for (int jj = 0; jj < numRow; jj++)
		{
			if (vvcTable[jj][ii]->minWidth > colMinWidth[ii])
			{
				colMinWidth[ii] = vvcTable[jj][ii]->minWidth;
			}
		}
	}
}
