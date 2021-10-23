#pragma once
#include "mathfunc.h"

using namespace std;

struct JSCALEBAR
{
	int activeIndex = -1;
	JFUNC jf;
	unordered_map<string, double> mapPrefix;  // Prefix (Thousand, Million, etc) -> multiple of thousand
	MATHFUNC mf;
	set<string> setPrefix;  // Prefix (Thousand, Million, etc)
	vector<vector<double>> vDataset;
	vector<int> vDecimalPlaces;
	vector<string> vUnit;

	JSCALEBAR() { initMaps(); }
	~JSCALEBAR() {}

	int addDataset(vector<double>& dataset);
	vector<int> addDataset(vector<vector<double>>& datasetList);
	double checkForCompressedUnit(string unit);
	void clear();
	int getActiveIndex() { return activeIndex; }
	vector<vector<double>> getDatasetColour(vector<vector<double>>& keyColour, vector<double>& keyValue, int index);
	double getDatasetValue(int index, int indexValue);
	int getDecimalPlaces(int index);
	unordered_map<string, double> getMapDatasetLabel(vector<string>& vsLabel, int index);
	vector<double> getTickValues(int index, int numTicks);
	vector<double> getTickValues(int index, int numTicks, vector<int> vExclude);
	string getUnit(int index);
	void initMaps();
	int makeDataset(vector<int> vIndex, char operation);
	void setUnit(int index, string unit);
};

