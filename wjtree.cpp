#include "wjtree.h"

using namespace std;

WJTREE::WJTREE()
{
	initGUI();

	vector<string> vsHeader{ "" };
	modelGeo = make_shared<WJTREEMODEL>(vsHeader);
	defaultRowExpansion = -1;
}

void WJTREE::err(string message)
{
	string errorMessage = "WJTREE error:\n" + message;
	JLOG::getInstance()->err(errorMessage);
}
void WJTREE::initGUI()
{
	setOverflow(Wt::Overflow::Auto, Wt::Orientation::Horizontal);

	auto vLayoutUnique = make_unique<Wt::WVBoxLayout>();
	auto vLayout = this->setLayout(std::move(vLayoutUnique));

	auto labelUnique = make_unique<Wt::WText>(" ");
	auto label = vLayout->insertWidget(index::Label, std::move(labelUnique));

	auto treeUnique = make_unique<Wt::WTreeView>();
	auto tree = vLayout->insertWidget(index::Tree, std::move(treeUnique));
	tree->setObjectName("Tree");
	tree->setSelectionMode(Wt::SelectionMode::Single);

	vLayout->insertStretch(index::Stretch, 1);
}
void WJTREE::makeTreeGeo(const vector<vector<string>>& vvsGeo)
{
	// Populates the JTREE using a database table. vvsGeo should have form
	// [region index][GEO_CODE, sRegion, GEO_LEVEL, Ancestor0, Ancestor1, ...]
	
	modelGeo->jt.reset();
	modelGeo->jt = make_shared<JTREE>();
	
	unordered_map<string, int> mapGCID;
	string sParent;
	int parentID;
	JNODE jnRoot = modelGeo->jt->getRoot();
	int numRegion = (int)vvsGeo.size();
	for (int ii = 0; ii < numRegion; ii++) {
		JNODE jn(3);
		mapGCID.emplace(vvsGeo[ii][0], jn.ID);
		jn.vsData[0] = vvsGeo[ii][1];  // Region name.
		jn.vsData[1] = vvsGeo[ii][0];  // GEO_CODE.
		jn.vsData[2] = vvsGeo[ii][2];  // GEO_LEVEL.
		if (vvsGeo[ii].size() < 4) {  // This row is the root.
			modelGeo->jt->addChild(jnRoot.ID, jn);
		}
		else {
			sParent = vvsGeo[ii].back();
			parentID = mapGCID.at(sParent);
			modelGeo->jt->addChild(parentID, jn);
		}
	}

	if (defaultRowExpansion >= 0) { modelGeo->jt->setExpandGeneration(defaultRowExpansion); }
}
void WJTREE::populateTree(JTREE*& jt, int parentID, Wt::WTreeNode*& parentNode)
{
	// Recursive function that takes an existing node and makes its children.

	string sID;
	Wt::WTreeNode* child = nullptr;
	vector<int> childrenID = jt->getChildrenID(parentID);
	int numChildren = (int)childrenID.size();
	if (numChildren == 0) { return; }
	for (int ii = 0; ii < numChildren; ii++) {
		JNODE& jn = jt->getNode(childrenID[ii]);
		sID = to_string(jn.ID);

		auto childUnique = make_unique<Wt::WTreeNode>(jn.vsData[0]);
		child = childUnique.get();
		parentNode->addChildNode(move(childUnique));
		child->setObjectName(sID);
		if (jn.expanded) { child->expand(); }

		populateTree(jt, childrenID[ii], child);
	}
}
void WJTREE::setLabel(const string& cata)
{
	sCata = cata;
	auto vLayout = (Wt::WVBoxLayout*)this->layout();
	auto wlItem = vLayout->itemAt(index::Label);
	auto label = (Wt::WText*)wlItem->widget();
	label->setText("Displaying geographic region tree for catalogue " + cata);
}
void WJTREE::setTree(const vector<vector<string>>& vvsGeo)
{
	// vvsGeo should be a raw geo table from the database.
	if (vvsGeo.size() == 0) { return; }	
	
	JTIME jtime;
	jtime.timerStart();

	makeTreeGeo(vvsGeo);
	long long time = jtime.timerRestart();
	cout << "makeTreeGeo time: " + to_string(time) << endl;

	JTREE* jt = jtGeo.get();
	JNODE& jnRoot = jt->getRoot();		
	Wt::WTreeNode* treeRoot = nullptr;
	
	auto vLayout = (Wt::WVBoxLayout*)this->layout();
	auto wlItem = vLayout->itemAt(index::Tree);
	auto tree = (Wt::WTree*)wlItem->widget();
	
	if (jnRoot.vsData[0].size() > 0) {
		const Wt::WString wsName(jnRoot.vsData[0]);
		auto treeRootUnique = make_unique<Wt::WTreeNode>(wsName);
		treeRoot = treeRootUnique.get();
		tree->setTreeRoot(std::move(treeRootUnique));
	}
	else {
		const Wt::WString wsName(" ");
		auto treeRootUnique = make_unique<Wt::WTreeNode>(wsName);
		treeRoot = treeRootUnique.get();
		tree->setTreeRoot(std::move(treeRootUnique));
	}

	treeRoot->setNodeVisible(0);
	treeRoot->expand();
	populateTree(jt, jnRoot.ID, treeRoot);

	time = jtime.timerStop();
	cout << "populateTree time: " + to_string(time) << endl;
}
