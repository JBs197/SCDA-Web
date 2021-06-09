#include "SCDAwindow.h"

using namespace std;

SCDAwindow::SCDAwindow(const Wt::WEnvironment& env) : Wt::WApplication(env)
{
	sf.init(sroot + "\\SCDA.db");

	auto BootstrapTheme = make_unique<Wt::WBootstrapTheme>();
	BootstrapTheme->setVersion(Wt::BootstrapVersion::v3);
	this->setTheme(move(BootstrapTheme));

	box_main = root()->addNew<Wt::WContainerWidget>();
	box_main->addWidget(make_unique<Wt::WText>("Statistics Canada Data Analysis Web Tool"));
	box_app = box_main->addNew<Wt::WContainerWidget>();
	layout_app = box_app->setLayout(make_unique<Wt::WHBoxLayout>());
	box_control = layout_app->addWidget(make_unique<Wt::WContainerWidget>());
	box_treelist = layout_app->addWidget(make_unique<Wt::WContainerWidget>());
	box_table = layout_app->addWidget(make_unique<Wt::WContainerWidget>());
	panel_year = box_control->addWidget(make_unique<Wt::WPanel>());
	panel_year->setTitle("Select a Year");
	cb_year = panel_year->setCentralWidget(make_unique<Wt::WComboBox>());
	selBox_tlist = box_treelist->addWidget(make_unique<Wt::WSelectionBox>());
	wtable = box_table->addWidget(make_unique<Wt::WTable>());
	//auto dpb_input = unique_ptr<Wt::WPushButton>();
	
	vector<string> year_list = sf.selectYears();
	cb_year->changed().connect(this, &SCDAwindow::on_cb_year_changed);
	Wt::WString wstemp = Wt::WString("All");
	cb_year->addItem(wstemp);
	for (int ii = 0; ii < year_list.size(); ii++)
	{
		Wt::WString wstemp = Wt::WString(year_list[ii]);
		cb_year->addItem(wstemp);
	}
	
	//sf.all_tables(tnum);  // Populate tnum with the whole number of tables in the db.
	vector<string> vtemp;  
	//list_update(vtemp);  // NULL.

	string temp = "";  
	table_update(temp);  // NULL.

	//pb_input->clicked().connect(this, &SCDAwindow::pb_input);
	/*
	dpanel_year.release();
	dselB_tlist.release();
	dtable_view.release();
	dcb_year.release();
	dbox_control.release();
	dbox_tree.release();
	dbox_table.release();
	dlayout_app.release();
	dbox_app.release();
	dbm.release();
	*/

}

void SCDAwindow::init_proj_dir(string exec_dir)
{
	size_t pos1 = exec_dir.rfind('\\');  // Debug or release.
	pos1 = exec_dir.rfind('\\', pos1 - 1);  // Project folder.
	proj_dir = exec_dir.substr(0, pos1);
}
void SCDAwindow::list_update(vector<string>& slist)
{
	// REDO
	selBox_tlist = new Wt::WSelectionBox();
	string temp;
	if (tnum < list_max)
	{
		for (int ii = 0; ii < slist.size(); ii++)
		{
			Wt::WString wstemp = Wt::WString(slist[ii]);
			selBox_tlist->addItem(wstemp);
		}
	}
	else
	{
		temp = to_string(tnum) + " results from the database.";
		Wt::WString wstemp = Wt::WString(temp);
		selBox_tlist->addItem(wstemp);
		selBox_tlist->addItem("Apply more filters to view results here.");
	}

}
void SCDAwindow::on_cb_year_changed()
{
	cout << "BLAH BLAH BLAH" << endl;
}

void SCDAwindow::table_update(string tname)
{
	wtable = new Wt::WTable();
	vector<string> column_titles, search;
	vector<vector<string>> rows;
	int max_col;
	if (sf.table_exist(tname))
	{
		sf.get_col_titles(tname, column_titles);
		wtable->setHeaderCount(1);
		search = { "*" };
		max_col = sf.select(search, tname, rows);
		for (int ii = 0; ii < column_titles.size(); ii++)
		{
			Wt::WString wstemp = Wt::WString(column_titles[ii]);
			wtable->elementAt(0, ii)->addWidget(make_unique<Wt::WText>(wstemp));
		}
		for (int ii = 0; ii < rows.size(); ii++)
		{
			for (int jj = 0; jj < rows[ii].size(); jj++)
			{
				Wt::WString wstemp = Wt::WString(rows[ii][jj]);
				wtable->elementAt(1 + ii, jj)->addWidget(make_unique<Wt::WText>(wstemp));
			}
		}
	}
	else
	{
		for (int ii = 0; ii < 3; ii++)
		{
			for (int jj = 0; jj < 3; jj++)
			{
				if (ii == jj)
				{
					switch (ii)
					{
					case 0:
						wtable->elementAt(ii, jj)->addNew<Wt::WText>("table");
						break;
					case 1:
						wtable->elementAt(ii, jj)->addNew<Wt::WText>("is");
						break;
					case 2:
						wtable->elementAt(ii, jj)->addNew<Wt::WText>("empty");
						break;
					}
				}
				else
				{
					wtable->elementAt(ii, jj)->addNew<Wt::WText>(" ");
				}
			}
		}
	}
	//WApplication* app = Wt::WApplication::instance();

}
