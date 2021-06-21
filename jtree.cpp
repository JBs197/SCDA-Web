#include "jtree.h"

void JTREE::addChild(string& sname, int& iname, string sparent)
{
	int indexParent;
	try { indexParent = mapS.at(sparent); }
	catch (out_of_range) { jf.err("mapS-jtree.addChild"); }
	addChildWorker(sname, iname, indexParent);
}
void JTREE::addChild(string& sname, int& iname, int iparent)
{
	int indexParent;
	try { indexParent = mapI.at(iparent); }
	catch (out_of_range) { jf.err("mapI-jtree.addChild"); }
	addChildWorker(sname, iname, indexParent);
}
void JTREE::addChildWorker(string& sname, int& iname, int indexParent)
{
	if (alreadyExist(sname, iname, indexParent)) { return; }
	treeSTdes[indexParent].push_back(count);
	mapS.emplace(sname, count);
	mapI.emplace(iname, count);
	treePL.push_back(sname);
	treePLi.push_back(iname);
	treeSTdes.push_back(vector<int>());
	if (treeSTanc[indexParent][0] >= 0)  // If parent is not root...
	{
		treeSTanc.push_back(treeSTanc[indexParent]);
		treeSTanc[count].push_back(indexParent);
	}
	else
	{
		treeSTanc.push_back({ 0 });
	}
	count++;
}
void JTREE::addChildren(vector<string>& snames, vector<int>& inames, string sparent)
{
	if (snames.size() != inames.size()) { jf.err("Size mismatch-jt.addChildren"); }
	int indexParent;
	try { indexParent = mapS.at(sparent); }
	catch (out_of_range) { jf.err("mapS-jtree.addChildren"); }
	for (int ii = 0; ii < snames.size(); ii++)
	{
		addChildWorker(snames[ii], inames[ii], indexParent);
	}
}
void JTREE::addChildren(vector<string>& snames, vector<int>& inames, int iparent)
{
	if (snames.size() != inames.size()) { jf.err("Size mismatch-jt.addChildren"); }
	int indexParent;
	try { indexParent = mapI.at(iparent); }
	catch (out_of_range) { jf.err("mapI-jtree.addChildren"); }
	for (int ii = 0; ii < snames.size(); ii++)
	{
		addChildWorker(snames[ii], inames[ii], indexParent);
	}
}
bool JTREE::alreadyExist(string sname, int iname, int parentIndex)
{
	int myIndex1, myIndex2;
	try
	{
		myIndex1 = mapS.at(sname);
		myIndex2 = mapI.at(iname);
	}
	catch (out_of_range& oor) { return 0; }
	for (int ii = 0; ii < treeSTdes[parentIndex].size(); ii++)
	{
		if (treeSTdes[parentIndex][ii] == myIndex1 && myIndex1 == myIndex2)
		{
			return 1;
		}
	}
	return 0;
}
void JTREE::deleteChildren(string& sParent)
{
	int indexNode;
	try { indexNode = mapS.at(sParent); }
	catch (out_of_range& oor) { return; }
	vector<int> vKids = treeSTdes[indexNode];
	for (int ii = vKids.size() - 1; ii >= 0; ii--)
	{
		deleteNodeHelper(vKids[ii]);
		treeSTdes[indexNode].erase(treeSTdes[indexNode].begin() + ii);
	}
}
void JTREE::deleteLeaves(string& sParent)
{
	int indexNode, numGrandKids;
	try { indexNode = mapS.at(sParent); }
	catch (out_of_range& oor) { return; }
	vector<int> vKids = treeSTdes[indexNode], vGKids;
	for (int ii = vKids.size() - 1; ii >= 0; ii--)
	{
		vGKids = treeSTdes[vKids[ii]];
		numGrandKids = vGKids.size();
		if (numGrandKids == 0)
		{
			mapS.erase(treePL[vKids[ii]]);
			mapI.erase(treePLi[vKids[ii]]);
			treePL[vKids[ii]].clear();
			treePLi[vKids[ii]] = -2;
			treeSTanc[vKids[ii]] = { -2 };
			treeSTdes[indexNode].erase(treeSTdes[indexNode].begin() + ii);
		}
	}
}
void JTREE::deleteNodeHelper(int index)
{
	vector<int> vKids = treeSTdes[index];
	if (vKids.size() == 0)
	{
		mapS.erase(treePL[index]);
		mapI.erase(treePLi[index]);
		treePL[index].clear();
		treePLi[index] = -2;
		treeSTanc[index] = { -2 };
	}
	else
	{
		for (int ii = vKids.size() - 1; ii >= 0; ii--)
		{
			deleteNodeHelper(vKids[ii]);
			treeSTdes[index].erase(treeSTdes[index].begin() + ii);
		}
	}
}
void JTREE::init(string nR, string pR)
{
	nameRoot = nR;
	pathRoot = pR;
	treePL.clear();
	treePLi.clear();
	treeSTanc.clear();
	treeSTdes.clear();
	mapI.clear();
	mapS.clear();
	treePL.push_back(nameRoot);
	treePLi.push_back(-1);
	treeSTanc.push_back({ -1 });
	treeSTdes.push_back(vector<int>());
	mapI.emplace(-1, 0);
	mapS.emplace(nameRoot, 0);
	count = 1;
	extHierarchy = { ".pdf", ".png", ".bin" };
	if (nameRoot == "") { invisRoot = 1; }
}
void JTREE::inputTreeSTPL(vector<vector<int>>& tree_st, vector<string>& tree_pl, vector<int>& tree_ipl)
{
	// NOTE: This function will not insert the STPL tree's first entry, as it is presumed
	// to be the tree root which the JTREE builds itself during initialization.
	if (tree_st[0][0] != 0) { jf.err("Bad root-jt.inputTreeSTPL"); }
	int iRoot = -1;
	int indexGen, indexTree, pivot, indexParent;
	vector<int> kids;
	vector<vector<int>> genLayers;
	genLayers.push_back(tree_st[0]);
	genLayers[0].erase(genLayers[0].begin());
	int numKids = genLayers[0].size();
	for (int ii = 0; ii < numKids; ii++)
	{
		addChild(tree_pl[genLayers[0][ii]], tree_ipl[genLayers[0][ii]], iRoot);  // Root always has index -1.
	}
	while (numKids > 0)
	{
		indexGen = genLayers.size();
		genLayers.push_back(vector<int>());
		numKids = 0;
		for (int ii = 0; ii < genLayers[indexGen - 1].size(); ii++)  // For every parent...
		{
			indexTree = genLayers[indexGen - 1][ii];
			pivot = jf.getPivot(tree_st[indexTree]);
			if (pivot >= tree_st[indexTree].size() - 1) { continue; }
			kids.clear();
			kids.insert(kids.begin(), tree_st[indexTree].begin() + pivot + 1, tree_st[indexTree].end());
			for (int jj = 0; jj < kids.size(); jj++)  // For every child...
			{
				genLayers[indexGen].push_back(kids[jj]);
				numKids++;
				addChild(tree_pl[kids[jj]], tree_ipl[kids[jj]], tree_pl[indexTree]);
			}
		}
	}
}
int JTREE::getHierarchy(string& ext)
{
	for (int ii = 0; ii < extHierarchy.size(); ii++)
	{
		if (ext == extHierarchy[ii])
		{
			return ii;
		}
	}
	return -1;
}
string JTREE::getRootName()
{
	return nameRoot;
}
void JTREE::listChildren(string& sparent, vector<int>& ikids, vector<string>& skids)
{
	int pIndex;
	try { pIndex = mapS.at(sparent); }
	catch (out_of_range& oor) { jf.err("mapS-jtree.listChildren"); }

	vector<int> viTemp = treeSTdes[pIndex];
	skids.resize(viTemp.size());
	ikids.resize(viTemp.size());
	for (int ii = 0; ii < viTemp.size(); ii++)
	{
		skids[ii] = treePL[viTemp[ii]];
		ikids[ii] = treePLi[viTemp[ii]];
	}
}

