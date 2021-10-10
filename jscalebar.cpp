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
vector<vector<double>> JSCALEBAR::getDatasetColour(vector<vector<double>>& keyColour, vector<double>& keyValue, int index)
{
	// From the matching sets of colours and values, return a list of colours for the dataset.
	if (keyColour.size() != keyValue.size()) { jf.err("Size mismatch-jscalebar.getDatasetColour"); }
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
	if (index >= vDataset.size()) { jf.err("Invalid dataset index-jscalebar.getDatasetValue"); }
	if (indexValue >= vDataset[index].size()) { jf.err("Invalid value index-jscalebar.getDatasetValue"); }
	return vDataset[index][indexValue];
}
unordered_map<string, double> JSCALEBAR::getMapDatasetLabel(vector<string>& vsLabel, int index)
{
	if (vsLabel.size() != vDataset[index].size()) { jf.err("Size mismatch-jscalebar.getMapDatasetLabel"); }
	unordered_map<string, double> mapLabel;
	for (int ii = 0; ii < vsLabel.size(); ii++)
	{
		mapLabel.emplace(vsLabel[ii], vDataset[index][ii]);
	}
	return mapLabel;
}
vector<double> JSCALEBAR::getTickValues(int index, int numTicks)
{
	vector<double> tickValues = getTickValues(index, numTicks, {});
	return tickValues;
}
vector<double> JSCALEBAR::getTickValues(int index, int numTicks, vector<int> vExclude)
{
	// vExclude specifies indices within the dataset to exclude from consideration.
	activeIndex = index;
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
	double dMinFloor = mf.roundingFloor(dMin, zeroes);
	sNum = to_string(dMax);
	pos1 = sNum.find('.');
	zeroes = -1 * (pos1 - 1);
	double dMaxCeil = mf.roundingCeil(dMax, zeroes);

	double dTemp;
	double scaleWidth = (dMaxCeil - dMinFloor) / (double)(numTicks - 1);
	for (int ii = 0; ii < numTicks; ii++)
	{
		dTemp = dMinFloor + ((double)ii * scaleWidth);
		vTick[ii] = mf.rounding(dTemp, vDecimalPlaces[index]);
	}
	return vTick;
}
int JSCALEBAR::makeDataset(vector<int> vIndex, char operation)
{
	if (vIndex.size() < 2 || vDataset.size() < 2) { jf.err("Missing parameters-jscalebar.makeDataset"); }
	int length;
	activeIndex = vDataset.size();
	vDataset.push_back(vDataset[vIndex[0]]);
	for (int ii = 1; ii < vIndex.size(); ii++)
	{
		length = min(vDataset[vIndex[0]].size(), vDataset[vIndex[ii]].size());
		for (int jj = 0; jj < length; jj++)
		{
			if (operation == '+') { vDataset[activeIndex][jj] += vDataset[vIndex[ii]][jj]; }
			else if (operation == '-') { vDataset[activeIndex][jj] -= vDataset[vIndex[ii]][jj]; }
			else if (operation == '*') { vDataset[activeIndex][jj] *= vDataset[vIndex[ii]][jj]; }
			else if (operation == '/') { vDataset[activeIndex][jj] /= vDataset[vIndex[ii]][jj]; }
			else { jf.err("Invalid operation-jscalebar.makeDataset"); }
		}
	}
	return activeIndex;
}
void JSCALEBAR::setUnit(int index, string unit, int decimalPlaces)
{
	if (index >= vUnit.size()) { vUnit.resize(index + 1); }
	vUnit[index] = unit;
	if (unit[0] == '%')
	{
		for (int ii = 0; ii < vDataset[index].size(); ii++)
		{
			vDataset[index][ii] *= 100.0;
		}
	}
	if (index >= vDecimalPlaces.size()) { vDecimalPlaces.resize(index + 1); }
	vDecimalPlaces[index] = decimalPlaces;
	activeIndex = index;
}
