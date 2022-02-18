#pragma once
#include "jnumber.h"
#include "jstring.h"

struct JSCALEBAR
{
	int activeIndex = -1;
	JNUMBER jnumber;
	std::unordered_map<std::string, double> mapPrefix;  // Prefix (Thousand, Million, etc) -> multiple of thousand
	std::set<std::string> setPrefix;  // Prefix (Thousand, Million, etc)
	std::vector<std::vector<double>> vDataset;
	std::vector<int> vDecimalPlaces;
	std::vector<std::string> vUnit;

	JSCALEBAR() { initMaps(); }
	~JSCALEBAR() {}

	int addDataset(std::vector<double>& dataset);
	std::vector<int> addDataset(std::vector<std::vector<double>>& datasetList);
	double checkForCompressedUnit(std::string unit);
	void clear();
	void err(std::string message);
	int getActiveIndex() { return activeIndex; }
	std::vector<std::vector<double>> getDatasetColour(std::vector<std::vector<double>>& keyColour, std::vector<double>& keyValue, int index);
	double getDatasetValue(int index, int indexValue);
	int getDecimalPlaces(int index);
	std::unordered_map<std::string, double> getMapDatasetLabel(std::vector<std::string>& vsLabel, int index);
	std::vector<double> getTickValues(int index, int numTicks);
	std::vector<double> getTickValues(int index, int numTicks, std::vector<int> vExclude);
	std::string getUnit(int index);
	void initMaps();
	int makeDataset(std::vector<int> vIndex, char operation);
	void setUnit(int index, std::string unit);
};

