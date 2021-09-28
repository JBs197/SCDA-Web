#pragma once
#include <Wt/WEnvironment.h>
#include <Wt/WApplication.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WTableView.h>
#include <Wt/WCssStyleSheet.h>
#include <Wt/WStandardItemModel.h>
#include <Wt/WStandardItem.h>
#include <Wt/WItemDelegate.h>
#include <Wt/WGridLayout.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WVBoxLayout.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>
#include "jtree.h"

using namespace std;

class WJDELEGATE : public Wt::WItemDelegate
{
	double height;
	string styleCell = "white-space: normal; overflow-y: auto;";
	string temp;

public:
	WJDELEGATE(const double& h) : Wt::WItemDelegate(), height(h) {}
	~WJDELEGATE() {}

	unique_ptr<Wt::WWidget> update(Wt::WWidget* widget, const Wt::WModelIndex& index, Wt::WFlags< Wt::ViewItemRenderFlag > flags)
	{
		int iRow = index.row();
		int iCol = index.column();
		auto widgetUnique = Wt::WItemDelegate::update(widget, index, flags);
		if (!widget)
		{
			if (iCol > 0)
			{
				temp = styleCell + "line-height: 28px; text-align: right; font-size: x-large;";
			}
			else
			{
				temp = styleCell + "line-height: 20px; text-align: left; font-size: medium; font-weight: bold;";
			}
			widgetUnique->setAttributeValue("style", temp);
		}
		else
		{
			int bbq = 1;
		}
		return widgetUnique;
	}
};
class WJHDELEGATE : public Wt::WItemDelegate
{
	double height;
	string styleCell = "line-height: 20px; white-space: normal; overflow-y: auto;";
	string temp;

	Wt::Signal<int, string> sigHeaderID_;

public:
	WJHDELEGATE(const double& h) : Wt::WItemDelegate(), height(h) {}
	~WJHDELEGATE() {}

	Wt::Signal<int, string>& sigHeaderID() { return sigHeaderID_; }

	unique_ptr<Wt::WWidget> update(Wt::WWidget* widget, const Wt::WModelIndex& index, Wt::WFlags< Wt::ViewItemRenderFlag > flags)
	{
		int iCol = index.column();
		auto widgetUnique = Wt::WItemDelegate::update(widget, index, flags);
		if (!widget)
		{
			if (iCol > 0) { temp = styleCell + " text-align: left;"; }
			else
			{
				int iNum = int(height / 2.0) - 10;
				temp = styleCell + " text-align: center;";
				temp += " padding-top: " + to_string(iNum) + "px;";
			}
			widgetUnique->setAttributeValue("style", temp);
			sigHeaderID_.emit(iCol, widgetUnique->id());
		}
		else
		{
			int bbq = 1;
		}
		return widgetUnique;
	}
};

class WJTABLE : public Wt::WTableView
{
	Wt::Signal<int, int> headerSignal_;
	const double heightCell = 68.0;
	const double heightHeader = 68.0;
	mutex m_time;
	string scSelectedWeak, scSelectedStrong, scWhite;
	Wt::WModelIndex selectedIndex = Wt::WModelIndex();
	set<string> setUnitBreaker;          // List of strings which disqualify a unit candidate.
	set<string> setUnitPercent;          // List of strings which indicate a unit of "%".
	Wt::WBorder wbNone, wbSelected;
	Wt::WColor wcSelectedWeak, wcSelectedStrong, wcWhite;
	Wt::WLength wlAuto;

public:
	WJTABLE(vector<vector<string>> vvsTable, vector<string> vsCol, vector<string> vsRow, string sRegion) 
		: Wt::WTableView() {
		setRowHeaderCount(1); 
		init(vvsTable, vsCol, vsRow, sRegion);
	}
	WJTABLE() : Wt::WTableView() {
		setRowHeaderCount(1);
		initBlank();
	}
	~WJTABLE() {}

	JFUNC jf;
	unordered_map<string, int> mapColIndex, mapRowIndex;  // sHeader -> col/row index
	unordered_map<int, string> mapColUnit, mapRowUnit;  // index -> sUnit (if known)
	unordered_map<int, string> mapColValue;  // colIndex -> sHeader
	unordered_map<int, string> mapHeaderID;  // colIndex -> headerID
	unordered_map<int, int> mapIndentCol, mapIndentRow;  // col/row index -> header indentation.
	
	shared_ptr<Wt::WStandardItemModel> model = nullptr;
	shared_ptr<WJDELEGATE> wjDel = nullptr;
	shared_ptr<WJHDELEGATE> wjhDel = nullptr;

	Wt::Signal<int, int>& headerSignal() { return headerSignal_; }

	void cellSelect(int iRow, int iCol);
	void cellSelect(Wt::WModelIndex wmIndex);
	void cellSelect(Wt::WModelIndex wmIndex, int iRow, int iCol);
	string getCell(int iRow, int iCol);
	int getColIndex(string sHeader);
	vector<int> getRowColSel();
	int getRowIndex(string sHeader);
	string getUnit();
	string getUnit(int iRow, int iCol);
	string getUnit(string header);
	string getUnitParser(string header);
	void headerSelect(int iCol);
	void init(vector<vector<string>> vvsTable, vector<string> vsCol, vector<string> vsRow, string sRegion);
	void initBlank();
	void initModel(int numRow, int numCol);
	void initValues();
	void modelSetCore(vector<vector<string>>& vvsData);
	void modelSetLeft(vector<string>& vsRow);
	void modelSetTop(vector<string>& vsCol);
	void modelSetTopLeft(string sRegion);
	void saveHeader(const int& iCol, const string& sID);
	void setColUnit(string& colHeader, int index);
	void setProperty(Wt::WWidget* widget, string property, string value);
	void setProperty(Wt::WWidget* widget, vector<string> vsProperty, vector<string> vsValue);
	void setRowUnit(string& rowHeader, int index);
	void tableClicked(const Wt::WModelIndex& wmIndex, const Wt::WMouseEvent& wmEvent);
	void tableHeaderClicked(const int& iCol, const Wt::WMouseEvent& wmEvent);
};

class WJTABLEBOX : public Wt::WContainerWidget
{
	Wt::WContainerWidget *boxTable, *boxTip;
	string sRegion;
	Wt::Signal<string> tipSignal_;
	vector<vector<int>> vviFilter;
	vector<string> vsCol, vsRow;
	vector<vector<string>> vvsData;
	Wt::WLength wlAuto = Wt::WLength();

	void init();

public:
	WJTABLEBOX() : Wt::WContainerWidget() { init(); }
	~WJTABLEBOX() {}

	Wt::WLink linkIconClose = Wt::WLink();
	Wt::WPushButton *pbFilterCol, *pbFilterRow;
	JFUNC jf;

	void addFilterBox();
	void addTipWidth();
	string makeCSV();
	void removeTipWidth();
	WJTABLE* setTable(vector<vector<string>>& vvsTable, vector<string>& vsColHeader, vector<string>& vsRowHeader, string& sRegionName);
	Wt::Signal<string>& tipSignal() { return tipSignal_; }
	void widgetMobile(bool mobile);

};
