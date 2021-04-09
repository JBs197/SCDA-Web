#include "jtree.h"

void JTREE::init(string root)
{
	treePL.clear();
	treeSTanc.clear();
	treeSTdes.clear();
	mapI.clear();
	mapS.clear();
	treePL.push_back(root);
	treeSTanc.push_back({ -1 });
	treeSTdes.push_back({ -1 });
	mapI.emplace(-1, 0);
	mapS.emplace(root, 0);
	count = 1;
}
