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
#include <Wt/WPopupMenu.h>
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
class WJSDELEGATE : public Wt::WItemDelegate  // Used for a smaller cell font.
{
	double height;
	string styleCell = "white-space: normal; overflow-y: auto;";
	string temp;

public:
	WJSDELEGATE(const double& h) : Wt::WItemDelegate(), height(h) {}
	~WJSDELEGATE() {}

	unique_ptr<Wt::WWidget> update(Wt::WWidget* widget, const Wt::WModelIndex& index, Wt::WFlags< Wt::ViewItemRenderFlag > flags)
	{
		int iRow = index.row();
		int iCol = index.column();
		auto widgetUnique = Wt::WItemDelegate::update(widget, index, flags);
		if (!widget)
		{
			if (iCol > 0)
			{
				temp = styleCell + "line-height: 28px; text-align: right; font-size: large;";
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
	double regionPopulation;
	string scSelectedWeak, scSelectedStrong, scWhite;
	Wt::WModelIndex selectedIndex = Wt::WModelIndex();
	set<string> setUnitBreaker;          // List of strings which disqualify a unit candidate.
	set<string> setUnitPercent;          // List of strings which indicate a unit of "%".
	string sUnitPreference;
	Wt::WBorder wbNone, wbSelected;
	Wt::WColor wcSelectedWeak, wcSelectedStrong, wcWhite;
	Wt::WLength wlAuto;

public:
	WJTABLE(vector<vector<string>>& vvsCore, vector<vector<string>>& vvsCol, vector<vector<string>>& vvsRow, vector<string>& vsNamePop)
		: Wt::WTableView() {
		setRowHeaderCount(1); 
		init(vvsCore, vvsCol, vvsRow, vsNamePop);
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
	unordered_map<unsigned long long, string> mapPrefixNumber;  // multiple of thousand -> (Thousand, Million, etc).

	shared_ptr<Wt::WStandardItemModel> model = nullptr;
	shared_ptr<WJDELEGATE> wjDel = nullptr;
	shared_ptr<WJSDELEGATE> wjsDel = nullptr;
	shared_ptr<WJHDELEGATE> wjhDel = nullptr;

	Wt::Signal<int, int>& headerSignal() { return headerSignal_; }

	void cellSelect(int iRow, int iCol);
	void cellSelect(Wt::WModelIndex wmIndex);
	void cellSelect(Wt::WModelIndex wmIndex, int iRow, int iCol);
	void compressUnitCell(string& sUnit, string& sCell);
	void compressUnitCol(int iCol, vector<vector<string>>& vvsCore);
	void compressUnitRow(int iRow, vector<vector<string>>& vvsCore);
	string getCell(int iRow, int iCol);
	string getCellValue(int iRow, int iCol);
	int getColIndex(string sHeader);
	vector<int> getRowColSel();
	int getRowIndex(string sHeader);
	string getUnit();
	string getUnit(int iRow, int iCol);
	string getUnit(string header);
	string getUnitParser(string header);
	void headerSelect(int iCol);
	void init(vector<vector<string>>& vvsCore, vector<vector<string>>& vvsCol, vector<vector<string>>& vvsRow, vector<string>& vsNamePop);
	void initBlank();
	void initModel(int numRow, int numCol);
	void initValues();
	void modelSetCore(vector<vector<string>>& vvsCore);
	void modelSetLeft(vector<vector<string>>& vvsRow);
	void modelSetTop(vector<vector<string>>& vvsCol);
	void modelSetTopLeft(string sRegion);
	void saveHeader(const int& iCol, const string& sID);
	void setColUnit(string& colHeader, int index);
	void setProperty(Wt::WWidget* widget, string property, string value);
	void setProperty(Wt::WWidget* widget, vector<string> vsProperty, vector<string> vsValue);
	void setRowUnit(string& rowHeader, int index);
	void tableClicked(const Wt::WModelIndex& wmIndex, const Wt::WMouseEvent& wmEvent);
	void tableClickedSimulated(int iRow, int iCol);
	void tableHeaderClicked(const int& iCol, const Wt::WMouseEvent& wmEvent);
};

class WJTABLEBOX : public Wt::WContainerWidget
{
	Wt::WContainerWidget *boxOption, *boxTable, *boxTip;
	unordered_map<string, Wt::WString> mapTooltip;  // sPrompt -> wsTooltip
	string sRegion;
	Wt::Signal<string> tipSignal_;
	vector<vector<int>> vviFilter;
	vector<vector<string>> vvsCore, vvsCol, vvsRow;
	Wt::WColor wcSelectedWeak, wcWhite;
	WJTABLE* wjTable = nullptr;
	Wt::WLength wlAuto, wlTableWidth, wlTableHeight;

	void init();
	void initColour();
	void initMaps();

public:
	WJTABLEBOX() : Wt::WContainerWidget() { init(); }
	~WJTABLEBOX() {}

	JFUNC jf;
	Wt::WLink linkIconClose = Wt::WLink();
	Wt::WPushButton *pbFilterCol, *pbFilterRow, *pbPinCol, *pbPinRow, *pbPinReset, *pbUnit;
	Wt::WPopupMenu* popupUnit;
	Wt::WText* textUnit;

	void addFilterBox();
	void addTipWidth();
	vector<vector<string>> getBargraphCol();
	vector<vector<string>> getBargraphRow();
	string makeCSV();
	unique_ptr<Wt::WContainerWidget> makeUnitPinBox(Wt::WPopupMenu*& popupUnit, Wt::WText*& textUnit, Wt::WPushButton*& pbUnit, Wt::WPushButton*& pbPinRow, Wt::WPushButton*& pbPinCol, Wt::WPushButton*& pbPinReset);
	void removeTipWidth();
	void resetMenu();
	WJTABLE* setTable(vector<vector<string>>& core, vector<vector<string>>& col, vector<vector<string>>& row, vector<string>& vsNamePop);
	void setTableSize();
	void setTableSize(Wt::WLength& wlWidth, Wt::WLength& wlHeight);
	Wt::Signal<string>& tipSignal() { return tipSignal_; }
	void updatePinButtons(vector<string> vsTooltip);
	WJTABLE* updateTable(vector<string>& vsNamePop, vector<int>& rowColSel);
	void widgetMobile(bool mobile);

};
