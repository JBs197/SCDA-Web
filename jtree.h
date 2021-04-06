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
	JFUNC jf;
	unordered_map<string, int> mapS;  // Index from string.
	unordered_map<int, int> mapI;  // Index from int.
	vector<vector<int>> treeSTanc;  // Form [index][anc1, anc2, ...]
	vector<vector<int>> treeSTdes;  // Form [index][des1, des2, ...]
	vector<string> treePL;  // Names by index.

public:
	JTREE(string rt) { 
		treePL.push_back(rt); 
		treeSTanc.push_back({ -1 });
		treeSTdes.push_back({ -1 });
		mapI.emplace(-1, 0);
		mapS.emplace(rt, 0);
		count = 1;
	}
	~JTREE() {}

	// TEMPLATES 

	template<typename ... Args> void addChild(string& sname, int& iname, Args& ... args) {}
	template<> void addChild<string>(string& sname, int& iname, string& sparent)
	{
		int parentIndex;
		try { parentIndex = mapS.at(sparent); }
		catch (out_of_range& oor) { jf.err("mapS-jtree.addChild"); }
		treeSTdes[parentIndex].push_back(count);
		mapS.emplace(sname, count);
		mapI.emplace(iname, count);
		treePL.push_back(sname);
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
		treeSTdes[parentIndex].push_back(count);
		mapS.emplace(sname, count);
		mapI.emplace(iname, count);
		treePL.push_back(sname);
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

};

