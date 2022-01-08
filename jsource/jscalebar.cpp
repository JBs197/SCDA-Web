#include "jscalebar.h"

int JSCALEBAR::addDataset(vector<double>& dataset)
{
	activeIndex = vDataset.size();
	vDataset.push_back(dataset);
	return activeIndex;
}
vector<int> JSCALEBAR::addDataset(vector<vector<double>>& datasetList)
{
	vector<int> vIndex(datasetList.size());
	for (int ii = 0; ii < vIndex.size(); ii++)
	{
		vIndex[ii] = vDataset.size();
		vDataset.push_back(datasetList[ii]);
	}
	return vIndex;
}
double JSCALEBAR::checkForCompressedUnit(string unit)
{
	// Look for "magnitude" prefixes in the unit. Return the denominator, if found.
	if (unit.size() == 1) { return 0.0; }
	if (unit[0] == '$' || unit[0] == '%' || unit[0] == '#') { return 0.0; }
	size_t pos1 = unit.find(' ');
	if (pos1 > unit.size()) { return 0.0; }
	double power = 0.0;
	string prefix = unit.substr(0, pos1);
	if (setPrefix.count(prefix)) {
		power = mapPrefix.at(prefix);
	}
	return power;
}
void JSCALEBAR::clear()
{
	activeIndex = -1;
	vDataset.clear();
	vDecimalPlaces.clear();
	vUnit.clear();
}
void JSCALEBAR::err(string message)
{
	string errorMessage = "JSCALEBAR error:\n" + message;
	JLOG::getInstance()->err(errorMessage);
}
vector<vector<double>> JSCALEBAR::getDatasetColour(vector<vector<double>>& keyColour, vector<double>& keyValue, int index)
{
	// From the matching sets of colours and values, return a list of colours for the dataset.
	// Only the first/last (min/max) values of keyValues are needed.
	if (index < 0) { index = activeIndex; }
	else if (keyValue.size() < 2) { err("Missing keyValue-jscalebar.getDatasetColour"); }
	int colourWidth = keyColour[0].size(), floor;
	vector<double> unknownColour = { 0.7, 0.7, 0.7 };  // Grey
	vector<double> extraColour = { 1.0, 0.0, 0.5 };  // Pink
	if (colourWidth == 4) 
	{ 
		unknownColour.push_back(1.0); 
		extraColour.push_back(1.0);
	}
	double dR, dG, dB, dA, percentage, remains;
	double keyMin = keyValue[0];
	double keyMax = keyValue[keyValue.size() - 1];
	vector<vector<double>> dataColour(vDataset[index].size(), vector<double>(colourWidth));
	if (keyMin == keyMax)
	{
		dataColour.assign(vDataset[index].size(), extraColour);
		return dataColour;
	}
	for (int ii = 0; ii < vDataset[index].size(); ii++)
	{
		// If the data point is ill-behaved, colour it grey.
		if (vDataset[index][ii] < keyMin || vDataset[index][ii] > keyMax)
		{
			dataColour[ii] = unknownColour;
			continue;
		}

		percentage = (vDataset[index][ii] - keyMin) / (keyMax - keyMin);
		if (percentage > 0.9999) { percentage = 0.9999; }
		percentage *= (double)(keyColour.size() - 1);
		floor = int(percentage);
		remains = percentage - (double)floor;
		dR = (keyColour[floor + 1][0] - keyColour[floor][0]) * remains;
		dG = (keyColour[floor + 1][1] - keyColour[floor][1]) * remains;
		dB = (keyColour[floor + 1][2] - keyColour[floor][2]) * remains;
		dataColour[ii][0] = keyColour[floor][0] + dR;
		dataColour[ii][1] = keyColour[floor][1] + dG;
		dataColour[ii][2] = keyColour[floor][2] + dB;
		if (colourWidth == 4)
		{
			dA = (keyColour[floor + 1][3] - keyColour[floor][3]) * remains;
			dataColour[ii][3] = keyColour[floor][3] + dA;
		}
	}
	return dataColour;
}
double JSCALEBAR::getDatasetValue(int index, int indexValue)
{
	if (index < 0) { index = activeIndex; }
	else if (index >= vDataset.size()) { err("Invalid dataset index-jscalebar.getDatasetValue"); }
	if (indexValue >= vDataset[index].size()) { err("Invalid value index-jscalebar.getDatasetValue"); }
	return vDataset[index][indexValue];
}
int JSCALEBAR::getDecimalPlaces(int index)
{
	if (index < 0) { index = activeIndex; }
	else if (index >= vDecimalPlaces.size()) { err("Invalid index-jscalebar.getDecimalPlaces"); }
	return vDecimalPlaces[index];
}
unordered_map<string, double> JSCALEBAR::getMapDatasetLabel(vector<string>& vsLabel, int index)
{
	if (vsLabel.size() != vDataset[index].size()) { err("Size mismatch-jscalebar.getMapDatasetLabel"); }
	unordered_map<string, double> mapLabel;
	for (int ii = 0; ii < vsLabel.size(); ii++)
	{
		mapLabel.emplace(vsLabel[ii], vDataset[index][ii]);
	}
	return mapLabel;
}
vector<double> JSCALEBAR::getTickValues(int index, int numTicks)
{
	if (index < 0) { index = activeIndex; }
	vector<double> tickValues = getTickValues(index, numTicks, {});
	return tickValues;
}
vector<double> JSCALEBAR::getTickValues(int index, int numTicks, vector<int> vExclude)
{
	// vExclude specifies indices within the dataset to exclude from consideration.
	if (index < 0) { index = activeIndex; }
	vector<double> vTick(numTicks);
	vector<double> dataset = vDataset[index];
	for (int ii = vExclude.size() - 1; ii >= 0; ii--)
	{
		dataset.erase(dataset.begin() + ii);
	}
	vector<int> minMax = jf.minMax(dataset);
	double dMin = dataset[minMax[0]];
	double dMax = dataset[minMax[1]];
	if (dMin == dMax)
	{
		vTick.assign(numTicks, dMin);
		return vTick;
	}

	string sNum = to_string(dMin);
	size_t pos1 = sNum.find('.');
	int zeroes = -1 * (pos1 - 1);
	double dMinFloor = jf.roundingFloor(dMin, zeroes);
	sNum = to_string(dMax);
	pos1 = sNum.find('.');
	zeroes = -1 * (pos1 - 1);
	double dMaxCeil = jf.roundingCeil(dMax, zeroes);

	double dTemp;
	double scaleWidth = (dMaxCeil - dMinFloor) / (double)(numTicks - 1);
	for (int ii = 0; ii < numTicks; ii++)
	{
		dTemp = dMinFloor + ((double)ii * scaleWidth);
		vTick[ii] = jf.rounding(dTemp, vDecimalPlaces[index]);
	}
	return vTick;
}
void JSCALEBAR::initMaps()
{
	setPrefix.emplace("Thousand");
	setPrefix.emplace("Million");
	setPrefix.emplace("Billion");
	setPrefix.emplace("Trillion");
	setPrefix.emplace("Quadrillion");

	mapPrefix.emplace("", 1.0);
	mapPrefix.emplace("Thousand", 1000.0);
	mapPrefix.emplace("Million", 1000000.0);
	mapPrefix.emplace("Billion", 1000000000.0);
	mapPrefix.emplace("Trillion", 1000000000000.0);
	mapPrefix.emplace("Quadrillion", 1000000000000000.0);  // Just in case...
}
int JSCALEBAR::makeDataset(vector<int> vIndex, char operation)
{
	// Returns the index of the newly-made dataset.
	if (vIndex.size() < 2 || vDataset.size() < 2) { err("Missing parameters-jscalebar.makeDataset"); }
	int length;
	int index = vDataset.size();
	vDataset.push_back(vDataset[vIndex[0]]);
	for (int ii = 1; ii < vIndex.size(); ii++)
	{
		length = min(vDataset[vIndex[0]].size(), vDataset[vIndex[ii]].size());
		for (int jj = 0; jj < length; jj++)
		{
			if (operation == '+') { vDataset[index][jj] += vDataset[vIndex[ii]][jj]; }
			else if (operation == '-') { vDataset[index][jj] -= vDataset[vIndex[ii]][jj]; }
			else if (operation == '*') { vDataset[index][jj] *= vDataset[vIndex[ii]][jj]; }
			else if (operation == '/') { vDataset[index][jj] /= vDataset[vIndex[ii]][jj]; }
			else { err("Invalid operation-jscalebar.makeDataset"); }
		}
	}
	return index;
}
string JSCALEBAR::getUnit(int index)
{
	if (index < 0) { index = activeIndex; }
	else if (index >= vUnit.size()) { err("Invalid index-jscalebar.getUnit"); }
	return vUnit[index];
}
void JSCALEBAR::setUnit(int index, string unit)
{
	if (index >= vUnit.size()) { vUnit.resize(index + 1); }
	if (index >= vDecimalPlaces.size()) { vDecimalPlaces.resize(index + 1); }
	vUnit[index] = unit;
	activeIndex = index;

	// Make adjustments to the dataset if necessary.
	double power = checkForCompressedUnit(unit);
	if (power > 0.0) {
		for (int ii = 0; ii < vDataset[index].size(); ii++)
		{
			vDataset[index][ii] /= power;
		}
	}
	else if (unit == "% of population") {
		for (int ii = 0; ii < vDataset[index].size(); ii++)
		{
			vDataset[index][ii] *= 100.0;
		}
	}

	if (power > 0.0) {
		vDecimalPlaces[index] = 3;
	}
	else if (unit[0] == '%') {
		vDecimalPlaces[index] = 1;
	}
	else {
		vDecimalPlaces[index] = 0;
	}
}
