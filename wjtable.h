#pragma once
#include <Wt/WEnvironment.h>
#include <Wt/WApplication.h>
#include <Wt/WTableView.h>
#include <Wt/WStandardItemModel.h>
#include <Wt/WStandardItem.h>
#include <Wt/WItemDelegate.h>
#include <Wt/WText.h>
#include "jfunc.h"

using namespace std;

class WJTABLE : public Wt::WTableView
{
	unordered_map<string, int> mapColIndex, mapRowIndex;  // sHeader -> col/row index
	unordered_map<int, string> mapColUnit, mapRowUnit;  // index -> sUnit (if known)
	Wt::WModelIndex selectedIndex;
	Wt::WBorder wbNone, wbSelected;
	Wt::WColor wcSelectedWeak, wcSelectedStrong, wcWhite;
	Wt::WLength wlAuto;

public:
	WJTABLE() : Wt::WTableView() { 
		setRowHeaderCount(1); 
		init();
	}
	~WJTABLE() {}

	JFUNC jf;
	shared_ptr<Wt::WStandardItemModel> model = nullptr;

	void cellSelect(int iRow, int iCol);
	void cellSelect(Wt::WModelIndex wmIndex);
	void cellSelect(Wt::WModelIndex wmIndex, int iRow, int iCol);
	void display();
	string getCell(int iRow, int iCol);
	int getColIndex(string sHeader);
	int getRowIndex(string sHeader);
	string getUnit();
	string getUnit(int iRow, int iCol);
	string getUnit(string header);
	string getUnitParser(string header);
	void init();
	void modelSetCore(vector<vector<string>>& vvsData);
	void modelSetLeft(vector<string>& vsRow);
	void modelSetTop(vector<string>& vsCol);
	void modelSetTopLeft(string sRegion);
	void reset();
	void tableClicked(const Wt::WModelIndex& wmIndex, const Wt::WMouseEvent& wmEvent);
};