#include "jtree.h"

void JTREE::init(string root)
{
	treePL.clear();
	treeSTanc.clear();
	treeSTdes.clear();
	mapI.clear();
	mapS.clear();
	treePL.push_back(root);
	treePLi.push_back(-1);
	treeSTanc.push_back({ -1 });
	treeSTdes.push_back({ -1 });
	mapI.emplace(-1, 0);
	mapS.emplace(root, 0);
	count = 1;
}
