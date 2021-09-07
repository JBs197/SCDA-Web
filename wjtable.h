#pragma once
#include <Wt/WContainerWidget.h>
#include <Wt/WPaintedWidget.h>
#include <Wt/WPainter.h>
#include <Wt/WSvgImage.h>
#include <Wt/WSignal.h>
#include <Wt/WCssDecorationStyle.h>
#include <Wt/WTable.h>
#include <Wt/WText.h>
#include "jfunc.h"

using namespace std;

class WJCELL : public Wt::WPaintedWidget
{
public:
	WJCELL() : Wt::WPaintedWidget() 
	{ 
		setLayoutSizeAware(true);
		resize(100.0, 100.0);
	}
	~WJCELL() override {}

	int minWidth;  // Unit of pixels.

	void initText(string text, JFUNC& jf, Wt::WFont& wfCell);
	void setTL(int x, int y);

private:
	string sText;
	int xTL, yTL;  // Unit of pixels.

protected:
	virtual void paintEvent(Wt::WPaintDevice* paintDevice) override;
};

class WJTABLE : public Wt::WContainerWidget
{
	string activeColTitle, activeRegion, activeRowTitle;
	vector<int> colMinWidth, rowMaxHeight;
	int height = -1, width = -1;                          // Unit is pixels.
	unordered_map<string, int> mapColIndex, mapRowIndex;  // sHeader -> col/row index
	unordered_map<int, string> mapColUnit, mapRowUnit;  // col/row index -> sUnit (if not default)
	int numCol = -2, numRow = -2;
	int selectedCol = -2, selectedRow = -2;
	vector<vector<unique_ptr<WJCELL>>> vvcTable;  // Form [row index][columnHeader, columnData1, columnData2, ...].
	Wt::WBorder wbNone, wbSelectedCell;
	Wt::WColor wcSelectedStrong, wcSelectedWeak, wcWhite;
	Wt::WFont wfCell;
	Wt::WLength wlAuto, wlCellPadding, wlRowHeaderMin;

	Wt::WContainerWidget *boxColHeader, *boxData, *boxRegion, *boxRowHeader;
	Wt::WTable* tableColHeader, *tableData, *tableRowHeader;
	Wt::WText* textRegion;

	Wt::Signal<int, int, string> wsigCellSel_;

	void setColUnit(string& colHeader, int index);
	void setRowUnit(string& rowHeader, int index);

public:
	WJTABLE() : WContainerWidget() { 
		this->setId("tableData"); 
		this->setLayoutSizeAware(1);
	}
	~WJTABLE() override {}

	JFUNC jf;
	Wt::Signal<int, int, string>& wsigCellSel() { return wsigCellSel_; }

	void cellClicked(int iRow, int iCol);
	virtual void clear() override;
	void colDeselect();
	void colSelect(int colIndex);
	int columnCount();
	void display();
	vector<string> elementAt(int rowIndex, int colIndex);
	vector<string> getCol(int iCol);
	string getColHeader(int iCol);
	int getColIndex(string colHeader);
	string getRegion();
	vector<string> getRow(int iRow);
	string getRowHeader(int iRow);
	int getRowIndex(string rowHeader);
	string getUnit();
	string getUnit(int iCol, int iRow);
	string getUnit(string header);
	string getUnitParser(string header);
	void init();
	void initColHeader(vector<string>& vsCol);
	void initData(vector<vector<string>>& vvsData);
	void initRowHeader(vector<string>& vsRow);
	virtual void layoutSizeChanged(int iWidth, int iHeight) override;
	int rowCount();
	void rowDeselect();
	void rowSelect(int rowIndex);
	void updateColMinWidth();
};
