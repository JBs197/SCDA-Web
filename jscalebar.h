#pragma once
#include "mathfunc.h"

using namespace std;

class JSCALEBAR
{
	int activeIndex = -1;
	JFUNC jf;
	MATHFUNC mf;
	vector<vector<double>> vDataset;
	vector<int> vDecimalPlaces;
	vector<string> vUnit;

public:
	JSCALEBAR() {}
	~JSCALEBAR() {}

	int addDataset(vector<double>& dataset);
	vector<int> addDataset(vector<vector<double>>& datasetList);
	int getActiveIndex() { return activeIndex; }
	vector<vector<double>> getDatasetColour(vector<vector<double>>& keyColour, vector<double>& keyValue, int index);
	double getDatasetValue(int index, int indexValue);
	unordered_map<string, double> getMapDatasetLabel(vector<string>& vsLabel, int index);
	vector<double> getTickValues(int index, int numTicks);
	vector<double> getTickValues(int index, int numTicks, vector<int> vExclude);
	int makeDataset(vector<int> vIndex, char operation);
	void setUnit(int index, string unit, int decimalPlaces);
};
