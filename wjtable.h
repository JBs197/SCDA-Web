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
#include "jnumber.h"
#include "jtime.h"
#include "jtree.h"

class WJDELEGATE : public Wt::WItemDelegate
{
	double height;
	std::string styleCell = "white-space: normal; overflow-y: auto;";
	std::string temp;

public:
	WJDELEGATE(const double& h) : Wt::WItemDelegate(), height(h) {}
	~WJDELEGATE() {}

	std::unique_ptr<Wt::WWidget> update(Wt::WWidget* widget, const Wt::WModelIndex& index, Wt::WFlags< Wt::ViewItemRenderFlag > flags)
	{
		int iRow = index.row();
		int iCol = index.column();
		auto widgetUnique = Wt::WItemDelegate::update(widget, index, flags);
		if (!widget) {
			if (iCol > 0) {
				temp = styleCell + "line-height: 28px; text-align: right; font-size: x-large;";
			}
			else {
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
	std::string styleCell = "white-space: normal; overflow-y: auto;";
	std::string temp;

public:
	WJSDELEGATE(const double& h) : Wt::WItemDelegate(), height(h) {}
	~WJSDELEGATE() {}

	std::unique_ptr<Wt::WWidget> update(Wt::WWidget* widget, const Wt::WModelIndex& index, Wt::WFlags< Wt::ViewItemRenderFlag > flags)
	{
		int iRow = index.row();
		int iCol = index.column();
		auto widgetUnique = Wt::WItemDelegate::update(widget, index, flags);
		if (!widget) {
			if (iCol > 0) {
				temp = styleCell + "line-height: 28px; text-align: right; font-size: large;";
			}
			else {
				temp = styleCell + "line-height: 20px; text-align: left; font-size: medium; font-weight: bold;";
			}
			widgetUnique->setAttributeValue("style", temp);
		}
		else {
			int bbq = 1;
		}
		return widgetUnique;
	}
};
class WJHDELEGATE : public Wt::WItemDelegate
{
	double height;
	std::string styleCell = "line-height: 20px; white-space: normal; overflow-y: auto;";
	std::string temp;

	Wt::Signal<int, std::string> sigHeaderID_;

public:
	WJHDELEGATE(const double& h) : Wt::WItemDelegate(), height(h) {}
	~WJHDELEGATE() {}

	Wt::Signal<int, std::string>& sigHeaderID() { return sigHeaderID_; }

	std::unique_ptr<Wt::WWidget> update(Wt::WWidget* widget, const Wt::WModelIndex& index, Wt::WFlags< Wt::ViewItemRenderFlag > flags)
	{
		int iCol = index.column();
		auto widgetUnique = Wt::WItemDelegate::update(widget, index, flags);
		if (!widget) {
			if (iCol > 0) { temp = styleCell + " text-align: left;"; }
			else {
				int iNum = int(height / 2.0) - 10;
				temp = styleCell + " text-align: center;";
				temp += " padding-top: " + std::to_string(iNum) + "px;";
			}
			widgetUnique->setAttributeValue("style", temp);
			sigHeaderID_.emit(iCol, widgetUnique->id());
		}
		else {
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
	JNUMBER jnumber;
	JPARSE jparse;
	std::mutex m_time;
	double regionPopulation;
	std::string scSelectedWeak, scSelectedStrong, scWhite;
	Wt::WModelIndex selectedIndex = Wt::WModelIndex();
	std::set<std::string> setUnitBreaker;          // List of strings which disqualify a unit candidate.
	std::set<std::string> setUnitDollar1;          // List of strings which may indicate a unit of "$".
	std::set<std::string> setUnitDollar2;          // List of strings which confirm a unit of "$".
	std::set<std::string> setUnitPercent;          // List of strings which indicate a unit of "%".
	std::string sUnitPreference;
	Wt::WBorder wbNone, wbSelected;
	Wt::WColor wcSelectedWeak, wcSelectedStrong, wcWhite;
	Wt::WLength wlAuto;

	void err(std::string message);

public:
	WJTABLE(std::vector<std::vector<std::string>>& vvsCore, std::vector<std::vector<std::string>>& vvsCol, std::vector<std::vector<std::string>>& vvsRow, std::vector<std::string>& vsNamePop, std::string& configXML)
		: Wt::WTableView() {
		setRowHeaderCount(1); 
		init(vvsCore, vvsCol, vvsRow, vsNamePop, configXML);
	}
	WJTABLE() : Wt::WTableView() {
		setRowHeaderCount(1);
		initBlank();
	}
	~WJTABLE() {}

	JTIME jtime;

	std::unordered_map<std::string, int> mapColIndex, mapRowIndex;  // sHeader -> col/row index
	std::unordered_map<int, std::string> mapColUnit, mapRowUnit;  // index -> sUnit (if known)
	std::unordered_map<int, std::string> mapColValue;  // colIndex -> sHeader
	std::unordered_map<int, std::string> mapHeaderID;  // colIndex -> headerID
	std::unordered_map<int, int> mapIndentCol, mapIndentRow;  // col/row index -> header indentation.
	std::unordered_map<unsigned long long, std::string> mapPrefixNumber;  // multiple of thousand -> (Thousand, Million, etc).

	std::shared_ptr<Wt::WStandardItemModel> model = nullptr;
	std::shared_ptr<WJDELEGATE> wjDel = nullptr;
	std::shared_ptr<WJSDELEGATE> wjsDel = nullptr;
	std::shared_ptr<WJHDELEGATE> wjhDel = nullptr;

	Wt::Signal<int, int>& headerSignal() { return headerSignal_; }

	void cellSelect(int iRow, int iCol);
	void cellSelect(Wt::WModelIndex wmIndex);
	void cellSelect(Wt::WModelIndex wmIndex, int iRow, int iCol);
	void compressUnitCell(std::string& sUnit, std::string& sCell);
	void compressUnitCol(int iCol, std::vector<std::vector<std::string>>& vvsCore);
	void compressUnitRow(int iRow, std::vector<std::vector<std::string>>& vvsCore);
	std::string getCell(int iRow, int iCol);
	std::string getCellValue(int iRow, int iCol);
	int getColIndex(std::string sHeader);
	std::vector<int> getRowColSel();
	int getRowIndex(std::string sHeader);
	std::string getUnit();
	std::string getUnit(int iRow, int iCol);
	std::string getUnit(std::string header);
	std::string getUnitParser(std::string header);
	void headerSelect(int iCol);
	void init(std::vector<std::vector<std::string>>& vvsCore, std::vector<std::vector<std::string>>& vvsCol, std::vector<std::vector<std::string>>& vvsRow, std::vector<std::string>& vsNamePop, std::string& configXML);
	void initBlank();
	void initModel(int numRow, int numCol);
	void initValues(std::string& configXML);
	void modelSetCore(std::vector<std::vector<std::string>>& vvsCore);
	void modelSetLeft(std::vector<std::vector<std::string>>& vvsRow);
	void modelSetTop(std::vector<std::vector<std::string>>& vvsCol);
	void modelSetTopLeft(std::string sRegion);
	void saveHeader(const int& iCol, const std::string& sID);
	void setColUnit(std::string& colHeader, int index);
	void setProperty(Wt::WWidget* widget, std::string property, std::string value);
	void setProperty(Wt::WWidget* widget, std::vector<std::string> vsProperty, std::vector<std::string> vsValue);
	void setRowUnit(std::string& rowHeader, int index);
	void tableClicked(const Wt::WModelIndex& wmIndex, const Wt::WMouseEvent& wmEvent);
	void tableClickedSimulated(int iRow, int iCol);
	void tableHeaderClicked(const int& iCol, const Wt::WMouseEvent& wmEvent);
};

class WJTABLEBOX : public Wt::WContainerWidget
{
	Wt::WContainerWidget *boxOption, *boxTable, *boxTip;
	JSTRING jstr;
	std::unordered_map<std::string, Wt::WString> mapTooltip;  // sPrompt -> wsTooltip
	bool mobile = 0;
	std::string sRegion;
	Wt::Signal<std::string> tipSignal_;
	std::vector<std::vector<int>> vviFilter;
	std::vector<std::vector<std::string>> vvsCore, vvsCol, vvsRow;
	Wt::WColor wcSelectedWeak, wcWhite;
	WJTABLE* wjTable = nullptr;
	Wt::WLength wlAuto, wlTableWidth, wlTableHeight;

	void err(std::string message);
	void init();
	void initColour();
	void initMaps();

public:
	WJTABLEBOX() : Wt::WContainerWidget() { init(); }
	~WJTABLEBOX() {}

	std::string configXML;
	Wt::WLink linkIconClose = Wt::WLink();
	Wt::WPushButton *pbFilterCol, *pbFilterRow, *pbPinCol, *pbPinRow, *pbPinReset, *pbUnit;
	Wt::WPopupMenu* popupUnit;
	Wt::WText* textUnit;

	void addFilterBox();
	void addTipWidth();
	std::vector<std::vector<std::string>> getBargraphCol();
	std::vector<std::vector<std::string>> getBargraphRow();
	std::string makeCSV();
	std::unique_ptr<Wt::WContainerWidget> makeUnitPinBox(Wt::WPopupMenu*& popupUnit, Wt::WText*& textUnit, Wt::WPushButton*& pbUnit, Wt::WPushButton*& pbPinRow, Wt::WPushButton*& pbPinCol, Wt::WPushButton*& pbPinReset);
	void removeTipWidth();
	void resetMenu();
	WJTABLE* setTable(std::vector<std::vector<std::string>>& core, std::vector<std::vector<std::string>>& col, std::vector<std::vector<std::string>>& row, std::vector<std::string>& vsNamePop);
	void setTableSize();
	void setTableSize(Wt::WLength& wlWidth, Wt::WLength& wlHeight);
	Wt::Signal<std::string>& tipSignal() { return tipSignal_; }
	void updatePinButtons(std::vector<std::string> vsTooltip);
	WJTABLE* updateTable(std::vector<std::string>& vsNamePop, std::vector<int>& rowColSel);
	void widgetMobile(bool mobile);

};
