#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <stdexcept>
#include "jfunc.h"

using namespace std;

class JTREE
{
	int count;
	string errorPath = sroot + "\\SCDA Error Log.txt";
	vector<string> extHierarchy;
	JFUNC jf;
	unordered_map<string, int> mapS;  // Index from string.
	unordered_map<int, int> mapI;  // Index from int.
	string pathRoot;  // Path to root folder. 
	string nameRoot;  // Name given to the root tree item.
	vector<vector<int>> treeSTanc;  // Form [index][anc1, anc2, ...]
	vector<vector<int>> treeSTdes;  // Form [index][des1, des2, ...]
	vector<string> treePL;  // snames by index.
	vector<int> treePLi;  // inames by index.

public:
	JTREE() {}
	~JTREE() {}

	void deleteNodeHelper(int index);
	void init(string nameRoot, string pathRoot);
	void inputTreeSTPL(vector<vector<int>>& tree_st, vector<string>& tree_pl, vector<int>& tree_ipl);
	int getHierarchy(string& ext);
	string getRootName();

	// TEMPLATES 

	template<typename ... Args> void addChild(string& sname, int& iname, Args& ... args) 
	{
		jf.err("addChild template-jt");
	}
	template<> void addChild<string>(string& sname, int& iname, string& sparent)
	{
		int parentIndex;
		try { parentIndex = mapS.at(sparent); }
		catch (out_of_range& oor) { jf.err("mapS-jtree.addChild"); }
		if (alreadyExist(sname, iname, parentIndex, extHierarchy)) { return; }
		treeSTdes[parentIndex].push_back(count);
		mapS.emplace(sname, count);
		mapI.emplace(iname, count);
		treePL.push_back(sname);
		treePLi.push_back(iname);
		treeSTdes.push_back(vector<int>());
		if (treeSTanc[parentIndex][0] >= 0)  // If parent is not root...
		{
			treeSTanc.push_back(treeSTanc[parentIndex]);
			treeSTanc[count].push_back(parentIndex);
		}
		else
		{
			treeSTanc.push_back({ 0 });
		}
		count++;
	}
	template<> void addChild<int>(string& sname, int& iname, int& iparent)
	{
		int parentIndex;
		try { parentIndex = mapI.at(iparent); }
		catch (out_of_range& oor) { jf.err("mapI-jtree.addChild"); }
		if (alreadyExist(sname, iname, parentIndex, extHierarchy)) { return; }
		treeSTdes[parentIndex].push_back(count);
		mapS.emplace(sname, count);
		mapI.emplace(iname, count);
		treePL.push_back(sname);
		treePLi.push_back(iname);
		treeSTdes.push_back(vector<int>());
		if (treeSTanc[parentIndex][0] >= 0)  // If parent is not root...
		{
			treeSTanc.push_back(treeSTanc[parentIndex]);
			treeSTanc[count].push_back(parentIndex);
		}
		else
		{
			treeSTanc.push_back({ 0 });
		}
		count++;
	}

	template<typename ... Args> void addChildren(vector<string>& snames, vector<int>& inames, Args& ... args) {}
	template<> void addChildren<string>(vector<string>& snames, vector<int>& inames, string& sparent)
	{
		for (int ii = 0; ii < snames.size(); ii++)
		{
			addChild(snames[ii], inames[ii], sparent);
		}
	}
	template<> void addChildren<int>(vector<string>& snames, vector<int>& inames, int& iparent)
	{
		for (int ii = 0; ii < snames.size(); ii++)
		{
			addChild(snames[ii], inames[ii], iparent);
		}
	}

	template<typename ... Args> bool alreadyExist(string sname, int iname, int parentIndex, Args& ... args)
	{
		jf.err("alradyExist template-jt");
	}
	template<> bool alreadyExist< >(string sname, int iname, int parentIndex)
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
	template<> bool alreadyExist<vector<string>>(string sname, int iname, int parentIndex, vector<string>& extHierarchy)
	{
		int myIndex1, myIndex2, myRank, existingRank;
		string snameCut, snamePaste;
		size_t pos1;
		for (int ii = 0; ii < extHierarchy.size(); ii++)
		{
			pos1 = sname.rfind(extHierarchy[ii]);
			if (pos1 < sname.size())
			{
				myRank = ii;
				break;
			}
			else if (ii == extHierarchy.size() - 1)
			{
				myRank = -1;
			}
		}

		if (myRank < 0)
		{
			try
			{
				myIndex1 = mapS.at(sname);
			}
			catch (out_of_range& oor) { return 0; }
			for (int ii = 0; ii < treeSTdes[parentIndex].size(); ii++)
			{
				if (treeSTdes[parentIndex][ii] == myIndex1)
				{
					return 1;
				}
			}
			return 0;
		}
		else
		{
			pos1 = sname.rfind('.');
			snameCut = sname.substr(0, pos1);
			existingRank = -1;
			for (int ii = 2; ii >= 0; ii--)
			{
				snamePaste = snameCut + extHierarchy[ii];
				try
				{
					myIndex1 = mapS.at(snamePaste);
				}
				catch (out_of_range& oor) { continue; }
				for (int jj = 0; jj < treeSTdes[parentIndex].size(); jj++)
				{
					if (treeSTdes[parentIndex][jj] == myIndex1)
					{
						existingRank = ii;
						break;
					}
				}
				if (existingRank >= 0) { break; }
			}
		}

		if (myRank <= existingRank) { return 1; }
		else if (existingRank < 0) { return 0; }
		else
		{
			treePL[myIndex1] = sname;
			mapS.erase(snamePaste);
			mapS.emplace(sname, myIndex1);
			return 1;
		}
		
		return 0;
	}

	template<typename ... Args> void deleteChildren(Args& ... args) {}
	template<> void deleteChildren<string>(string& sParent)
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

	template<typename ... Args> void deleteLeaves(Args& ... args) {}
	template<> void deleteLeaves<string>(string& sParent)
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

    template<typename ... Args> void listChildren(Args& ... args) {}
    template<> void listChildren<int, vector<int>>(int& iparent, vector<int>& ikids)
    {
        int pIndex;
        try { pIndex = mapI.at(iparent); }
        catch (out_of_range &oor) { jf.err("mapI-jtree.listChildren"); }

        ikids = treeSTdes[pIndex];
    }
    template<> void listChildren<int, vector<string>>(int& iparent, vector<string>& skids)
    {
        int pIndex;
        try { pIndex = mapI.at(iparent); }
        catch (out_of_range &oor) { jf.err("mapI-jtree.listChildren"); }

        vector<int> ikids = treeSTdes[pIndex];
        skids.resize(ikids.size());
        for (int ii = 0; ii < skids.size(); ii++)
        {
            skids[ii] = treePL[ikids[ii]];
        }
    }
    template<> void listChildren<string, vector<int>>(string& sparent, vector<int>& ikids)
    {
        int pIndex;
        try { pIndex = mapS.at(sparent); }
        catch (out_of_range &oor) { jf.err("mapS-jtree.listChildren"); }

        vector<int> vtemp = treeSTdes[pIndex];
		ikids.resize(vtemp.size());
		for (int ii = 0; ii < ikids.size(); ii++)
		{
			ikids[ii] = treePLi[vtemp[ii]];  // 16?? 17???
		}
    }
    template<> void listChildren<string, vector<string>>(string& sparent, vector<string>& skids)
    {
        int pIndex;
        try { pIndex = mapS.at(sparent); }
        catch (out_of_range &oor) { jf.err("mapS-jtree.listChildren"); }

        vector<int> ikids = treeSTdes[pIndex];
        skids.resize(ikids.size());
        for (int ii = 0; ii < skids.size(); ii++)
        {
            skids[ii] = treePL[ikids[ii]];
        }
    }

	template< typename ... Args> void listChildrenNoPath(vector<string>& sKids, vector<int>& iKids, Args& ... args)
	{
		jf.err("listChildrenNoPath template-jt");
	}
	template<> void listChildrenNoPath<string>(vector<string>& sKids, vector<int>& iKids, string& sParent)
	{
		int pIndex;
		try { pIndex = mapS.at(sParent); }
		catch (out_of_range& oor) { jf.err("mapS-jt.listChildrenNoPath"); }

		string sPath;
		size_t pos1;
		vector<int> indexKids = treeSTdes[pIndex];
		iKids.resize(indexKids.size());
		sKids.resize(indexKids.size());
		for (int ii = 0; ii < indexKids.size(); ii++)
		{
			iKids[ii] = treePLi[indexKids[ii]];
			sPath = treePL[indexKids[ii]];
			pos1 = sPath.rfind('\\');
			if (pos1 > sPath.size())
			{
				sKids[ii] = sPath;
			}
			else
			{
				sKids[ii] = sPath.substr(pos1 + 1);
			}
		}
	}

};
