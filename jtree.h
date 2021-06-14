#pragma once

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

	void addChild(string& sname, int& iname, string& sparent);
	void addChild(string& sname, int& iname, int& iparent);
	void addChildWorker(string& sname, int& iname, int indexParent);
	void addChildren(vector<string>& snames, vector<int>& inames, string& sparent);
	void addChildren(vector<string>& snames, vector<int>& inames, int& iparent);
	bool alreadyExist(string sname, int iname, int parentIndex);
	void deleteChildren(string& sParent);
	void deleteLeaves(string& sParent);
	void deleteNodeHelper(int index);
	void init(string nameRoot, string pathRoot);
	void inputTreeSTPL(vector<vector<int>>& tree_st, vector<string>& tree_pl, vector<int>& tree_ipl);
	int getHierarchy(string& ext);
	string getRootName();
	void listChildren(string& sparent, vector<int>& ikids, vector<string>& skids);

	// TEMPLATES 

};
