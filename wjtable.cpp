#include "wjtable.h"

void WJTABLE::cellSelect(int iRow, int iCol)
{
	// Note that the col/row headers have indices "0".
	if (iRow >= model->rowCount() || iCol >= model->columnCount()) { jf.err("Index beyond maximum-wjtable.cellClicked"); }
	if (iRow < 0) { iRow = 0; }
	if (iCol < 0) { iCol = 0; }
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
	// Un-highlight a previous selection, highlight this col/row, apply a border 
	// if the selection is a data cell, and then load a map using the col/row headers. 

	Wt::WText* wText = nullptr;
	Wt::WItemDelegate widCell;
	Wt::WModelIndex wmiTemp;
	Wt::WStandardItem* wsiTemp = nullptr;
	Wt::WFlags<Wt::ViewItemRenderFlag> wFlags(Wt::None);
	int selectedRow = selectedIndex.row();
	int selectedCol = selectedIndex.column();
	int numRow = model->rowCount();
	int numCol = model->columnCount();

	// Apply a border box to the selected cell.
	if (iRow > 0 && iCol > 0 && (iRow != selectedRow || iCol != selectedCol))  
	{
		if (selectedRow > 0 && selectedCol > 0)  // Remove the previous border box.
		{
			wText = (Wt::WText*)itemWidget(selectedIndex);
			wText->decorationStyle().setBorder(wbNone);
			widCell.update(wText, selectedIndex, wFlags);
		}
		wText = (Wt::WText*)itemWidget(wmIndex);
		wText->decorationStyle().setBorder(wbSelected);
		widCell.update(wText, wmIndex, wFlags);
	}

	// Remove the previous row/column highlighting.
	if (iRow != selectedRow && selectedRow > 0)
	{
		for (int ii = 0; ii < numCol; ii++)
		{
			if (ii == selectedCol) { continue; }
			wsiTemp = model->item(selectedRow, ii);
			wmiTemp = model->indexFromItem(wsiTemp);
			wText = (Wt::WText*)itemWidget(wmiTemp);
			wText->decorationStyle().setBackgroundColor(wcWhite);
			widCell.update(wText, wmiTemp, wFlags);
		}
	}
	if (iCol != selectedCol && selectedCol > 0)
	{
		for (int ii = 0; ii < numRow; ii++)
		{
			wsiTemp = model->item(selectedCol, ii);
			wmiTemp = model->indexFromItem(wsiTemp);
			wText = (Wt::WText*)itemWidget(wmiTemp);
			wText->decorationStyle().setBackgroundColor(wcWhite);
			widCell.update(wText, wmiTemp, wFlags);
		}
	}

	// Apply new row/column highlighting.
	if (iRow != selectedRow && iRow > 0)
	{ 
		for (int ii = 0; ii < numCol; ii++)
		{
			wsiTemp = model->item(iRow, ii);
			wmiTemp = model->indexFromItem(wsiTemp);
			wText = (Wt::WText*)itemWidget(wmiTemp);
			wText->decorationStyle().setBackgroundColor(wcSelectedWeak);
			widCell.update(wText, wmiTemp, wFlags);
		}
	}
	if (iCol != selectedCol && iCol > 0)
	{
		for (int ii = 0; ii < numRow; ii++)
		{
			wsiTemp = model->item(ii, iCol);
			wmiTemp = model->indexFromItem(wsiTemp);
			wText = (Wt::WText*)itemWidget(wmiTemp);
			wText->decorationStyle().setBackgroundColor(wcSelectedWeak);
			widCell.update(wText, wmiTemp, wFlags);
		}
	}

	// Update the saved selectedIndex.
	selectedIndex = wmIndex;
}
void WJTABLE::display()
{
	setModel(model);
	int numCol = model->columnCount();
	setRowHeight(48.0);
	setPreloadMargin(4000.0);
	for (int ii = 0; ii < numCol; ii++)
	{
		setHeaderWordWrap(ii, 1);
		setColumnWidth(ii, 250.0);
	}
}
string WJTABLE::getCell(int iRow, int iCol)
{
	if (iRow < 0 || iCol < 0) { jf.err("Negative index-wjtable.getCell"); }
	if (iRow >= model->rowCount() || iCol >= model->columnCount()) { jf.err("Index beyond maximum-wjtable.getCell"); }
	Wt::WStandardItem* wsiCell = model->item(iRow, iCol);
	Wt::WString wsTemp = wsiCell->text();
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
	// WColors used.
	wcSelectedWeak.setRgb(200, 200, 255);
	wcSelectedStrong.setRgb(150, 150, 192);
	wcWhite.setRgb(255, 255, 255);

	// WLengths used.
	wlAuto = Wt::WLength::Auto;

	// WBorders used.
	wbNone = Wt::WBorder();
	wbSelected = Wt::WBorder(Wt::BorderStyle::Inset, Wt::BorderWidth::Medium, wcSelectedStrong);

	// Initial widget configuration.

}
void WJTABLE::modelSetCore(vector<vector<string>>& vvsData)
{
	Wt::WString wsTemp;
	for (int ii = 0; ii < vvsData.size(); ii++)
	{
		for (int jj = 0; jj < vvsData[ii].size(); jj++)
		{
			wsTemp = Wt::WString::fromUTF8(vvsData[ii][jj]);
			auto cellUnique = make_unique<Wt::WStandardItem>(wsTemp);
			model->setItem(ii + 1, jj + 1, move(cellUnique));
		}
	}
}
void WJTABLE::modelSetLeft(vector<string>& vsRow)
{
	Wt::WString wsTemp;
	for (int ii = 0; ii < vsRow.size(); ii++)
	{
		mapRowIndex.emplace(vsRow[ii], ii + 1);
		wsTemp = Wt::WString::fromUTF8(vsRow[ii]);
		auto cellUnique = make_unique<Wt::WStandardItem>(wsTemp);
		model->setItem(ii + 1, 0, move(cellUnique));
	}
}
void WJTABLE::modelSetTop(vector<string>& vsCol)
{
	Wt::WString wsTemp;
	for (int ii = 0; ii < vsCol.size(); ii++)
	{
		mapColIndex.emplace(vsCol[ii], ii + 1);
		wsTemp = Wt::WString::fromUTF8(vsCol[ii]);
		auto cellUnique = make_unique<Wt::WStandardItem>(wsTemp);
		model->setItem(0, ii + 1, move(cellUnique));
	}
}
void WJTABLE::modelSetTopLeft(string sRegion)
{
	Wt::WString wsTemp = Wt::WString::fromUTF8(sRegion);
	auto cellUnique = make_unique<Wt::WStandardItem>(wsTemp);
	model->setItem(0, 0, move(cellUnique));
}
void WJTABLE::reset()
{
	mapColIndex.clear();
	mapRowIndex.clear();
	mapColUnit.clear();
	mapRowUnit.clear();
	selectedIndex = Wt::WModelIndex();  // Makes it invalid.
	
	auto sptrModel = make_shared<Wt::WStandardItemModel>();
	swap(model, sptrModel);

	this->clicked().connect(this, std::bind(&WJTABLE::tableClicked, this, std::placeholders::_1, std::placeholders::_2));
}
void WJTABLE::tableClicked(const Wt::WModelIndex& wmIndex, const Wt::WMouseEvent& wmEvent)
{
	// Triggered in response to a user click.
	jf.timerStart();
	cellSelect(wmIndex);
}
