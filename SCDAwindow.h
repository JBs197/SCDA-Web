#pragma once
#include <Wt/WApplication.h>
#include <Wt/WServer.h>
#include <Wt/WBreak.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WTabWidget.h>
#include <Wt/WLineEdit.h>
#include <Wt/WPushButton.h>
#include <Wt/WComboBox.h>
#include <Wt/WText.h>
#include <Wt/WPanel.h>
#include <Wt/WTable.h>
#include <Wt/WTableCell.h>
#include <Wt/WTreeView.h>
#include <Wt/WTreeNode.h>
#include <Wt/WSelectionBox.h>
#include <Wt/WCssStyleSheet.h>
#include <Wt/WBootstrapTheme.h>
#include <Wt/WStringListModel.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WVBoxLayout.h>
#include "jfunc.h"
#include "winfunc.h"
#include "sqlfunc.h"
#include "wtfunc.h"

using namespace std;
extern const string sroot;

class SCDAwindow : public Wt::WApplication
{
	SQLFUNC sf;
	//WTFUNC wtf;

	Wt::WHBoxLayout* layout_app;
	Wt::WContainerWidget *box_main, *box_app, *box_control, *box_treelist, *box_table;
	Wt::WComboBox* cb_year, cb_topic;
	Wt::WPanel* panel_year;
	Wt::WPushButton* pb_input;
	Wt::WSelectionBox* selBox_tlist;
	Wt::WTable* wtable;
	Wt::WTreeView* treeV_cata;

	int list_max = 100;  // Maximum number of elements to display on the list widget.
	vector<string> list_year;
	string proj_dir;
	int tnum;  // The number of tables in the database that satisfy the existing filters.

public:
	SCDAwindow(const Wt::WEnvironment& env);
	~SCDAwindow() {}

	void init_proj_dir(string);
	void list_update(vector<string>&);
	void table_update(string);

	void on_cb_year_changed();

};

