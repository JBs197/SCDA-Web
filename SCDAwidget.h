#pragma once
#include <Wt/WBootstrapTheme.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WVBoxLayout.h>
#include <Wt/WSelectionBox.h>
#include <Wt/WTree.h>
#include <Wt/WTreeNode.h>
#include <Wt/WTable.h>
#include <Wt/WPanel.h>
#include <Wt/WPushButton.h>
#include "SCDAserver.h"

using namespace std;

class SCDAwidget : public Wt::WContainerWidget, public SCDAserver::User
{
	string activeDesc;
	string activeYear;
	vector<int> cbActive;
	string db_path = sroot + "\\SCDA.db";
	JFUNC jf;
	const int num_filters = 3;
	string selectedCataName;
	string sessionID;
	enum treeType { Tree, Subtree };
	const Wt::WString wsAll = Wt::WString("All");

	Wt::WContainerWidget *boxControl, *boxTreelist, *boxTable;
	Wt::WComboBox *cbYear, *cbDesc, *cbRegion;
	Wt::WPanel *panelYear, *panelDesc, *panelRegion;
	Wt::WPushButton* pbTest;
	Wt::WSelectionBox* sbList;
	SCDAserver& sRef;
	Wt::WTable* wtTable;
	Wt::WText* textTest;
	Wt::WTree* treeCata;

	void connect();
	void init();
	//void updateYear();
	void processDataEvent(const DataEvent& event);
	static void setDesc(SCDAserver&, vector<string>);
	static void setYear(SCDAserver&, vector<string>);

public:
	SCDAwidget(SCDAserver& myserver) : WContainerWidget(), sRef(myserver) { init(); }

	void askTree();  // Populates only the year and catalogue layers of the tree.
	static void askRegion(SCDAserver&, vector<string>);  // Populates the region data for a single catalogue, on demand.
	void cbDescClicked();
	void cbYearClicked();


	// TEMPLATES

	template <typename S> void processDataEventHelper(treeType, vector<vector<int>>&, vector<S>&, int, Wt::WTreeNode*&, string) {}
	template<> void processDataEventHelper<string>(treeType tt, vector<vector<int>>& tree_st, vector<string>& tree_pl, int pl_index, Wt::WTreeNode*& me, string sessionID)
	{
		vector<int> kids;
		vector<Wt::WTreeNode*> pkids;
		int pivot;
		int rowSize = tree_st[pl_index].size();

		for (int ii = 0; ii < rowSize; ii++)
		{
			if (tree_st[pl_index][ii] < 0)
			{
				pivot = ii;
				break;
			}
			else if (ii == rowSize - 1)
			{
				pivot = 0;
			}
		}
		if (tt == 0 && pivot == 1)  // This node has exactly 1 parent (root) so it's a year.
		{
			vector<string> ancestry(3);
			ancestry[0] = sessionID;
			ancestry[1] = tree_pl[pl_index];
			function<void()> fn = bind(&SCDAwidget::setYear, ref(sRef), ancestry);
			me->expanded().connect(fn);
		}
		else if (tt == 0 && pivot == 2)  // This node has exactly 2 parents (root->year) so it's a catalogue.
		{
			vector<string> ancestry(3);
			ancestry[0] = sessionID;
			ancestry[2] = tree_pl[pl_index];
			int iparent = tree_st[pl_index][1];
			ancestry[1] = tree_pl[iparent];
			function<void()> fn = bind(&SCDAwidget::askRegion, ref(sRef), ancestry);
			me->expanded().connect(fn);
		}
		if (rowSize <= pivot + 1)  // Not a parent.
		{
			if (tt == 1) { return; }
			else if (tt == 0)  // Catalogue nodes receive a dummy entry by default. If the user
			{                  // tries to view a catalogue, it will be loaded on demand.
				const Wt::WString wstemp("Loading...");
				auto branch = make_unique<Wt::WTreeNode>(wstemp);
				me->addChildNode(move(branch));
				return;
			}
		}
		kids.resize(rowSize - pivot - 1);
		pkids.resize(rowSize - pivot - 1);
		for (int ii = 0; ii < kids.size(); ii++)
		{
			kids[ii] = abs(tree_st[pl_index][pivot + 1 + ii]);
		}
		for (int ii = 0; ii < kids.size(); ii++)
		{
			const Wt::WString wstemp(tree_pl[kids[ii]]);
			auto branch = make_unique<Wt::WTreeNode>(wstemp);
			pkids[ii] = me->addChildNode(move(branch));

		}
		for (int ii = 0; ii < pkids.size(); ii++)
		{
			processDataEventHelper(tt, tree_st, tree_pl, kids[ii], pkids[ii], sessionID);
		}
	}
	template<> void processDataEventHelper<wstring>(treeType tt, vector<vector<int>>& tree_st, vector<wstring>& tree_pl, int pl_index, Wt::WTreeNode*& me, string sessionID)
	{
		vector<int> kids;
		vector<Wt::WTreeNode*> pkids;
		int pivot;
		int rowSize = tree_st[pl_index].size();

		for (int ii = 0; ii < rowSize; ii++)
		{
			if (tree_st[pl_index][ii] < 0)
			{
				pivot = ii;
				break;
			}
			else if (ii == rowSize - 1)
			{
				pivot = 0;
			}
		}
		if (tt == 0 && pivot == 2)  // This node has exactly 2 parents (root->year) so it's a catalogue.
		{
			vector<string> ancestry(3);
			ancestry[0] = sessionID;
			ancestry[2] = jf.utf16to8(tree_pl[pl_index]);
			int iparent = tree_st[pl_index][1];
			ancestry[1] = jf.utf16to8(tree_pl[iparent]);
			function<void()> fn = bind(&SCDAwidget::askRegion, ref(sRef), ancestry);
			me->expanded().connect(fn);
		}
		if (rowSize <= pivot + 1)  // Not a parent.
		{
			if (tt == 1) { return; }
			else if (tt == 0)  // Catalogue nodes receive a dummy entry by default. If the user
			{                  // tries to view a catalogue, it will be loaded on demand.
				const Wt::WString wstemp("Loading...");
				auto branch = make_unique<Wt::WTreeNode>(wstemp);
				me->addChildNode(move(branch));
				return;
			}
		}
		kids.resize(rowSize - pivot - 1);
		pkids.resize(rowSize - pivot - 1);
		for (int ii = 0; ii < kids.size(); ii++)
		{
			kids[ii] = abs(tree_st[pl_index][pivot + 1 + ii]);
		}
		for (int ii = 0; ii < kids.size(); ii++)
		{
			const Wt::WString wstemp(tree_pl[kids[ii]]);
			auto branch = make_unique<Wt::WTreeNode>(wstemp);
			pkids[ii] = me->addChildNode(move(branch));

		}
		for (int ii = 0; ii < pkids.size(); ii++)
		{
			processDataEventHelper(tt, tree_st, tree_pl, kids[ii], pkids[ii], sessionID);
		}
	}

};

