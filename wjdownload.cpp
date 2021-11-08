#include "wjdownload.h"

void WJRCSV::handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
{
	response.setMimeType("csv");
	response.setContentLength(length);
	response.out().write(sCSV.c_str(), length);
}

void WJRPDF::handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
{
	response.setMimeType("pdf");
	size_t binSize = binPDF.size();
	if (!binSize)
	{
		error = "WJRPDF ERROR: pdfSize is zero";
		response.out().write(error.c_str(), error.size());
		return;
	}
	response.setContentLength(binSize + 1);
	string sPDF;
	sPDF.resize(binSize);
	for (int ii = 0; ii < binSize; ii++)
	{
		sPDF[ii] = (char)binPDF[ii];
	}
	response.out().write(sPDF.c_str(), binSize + 1);
}

void WJDOWNLOAD::adjustLineEditWidth()
{
	Wt::WString wsTemp = leFileName->text();
	string temp = wsTemp.toUTF8();
	int numChar = temp.size();
	double dWidth = numChar * 10.0;
	double dMax = min(700.0, dWidth);
	leFileName->setMaximumSize(dMax, wlAuto);
	leFileName->setTextSize(max(numChar - 4, 0));
}
void WJDOWNLOAD::clear()
{
	displayData.clear();
	legendBarDouble = -1;
	legendFontSize = 12;
	legendTickLines = -1;
	mapBorderKM.clear();
	mapFrameKM.clear();
	mapData.clear();
	mapRegion.clear();
	selectedMode = -1;
	wjrCSV.reset();
	wjrPDFbargraph.reset();
	wmrPNGbargraph.reset();
	wjrPDFmap.reset();
	wmrPNGmap.reset();
	wpPreview = nullptr; 
}
void WJDOWNLOAD::displayCSV(string& csv)
{
	if (wjrCSV != nullptr) { wjrCSV.reset(); }
	wjrCSV = makeWJRCSV(csv);
	sCSV = csv;
	updateStackedPB(selectedMode);
	updateStackedPreview(selectedMode);
}
void WJDOWNLOAD::displayPDFbargraph(vector<vector<double>>& seriesColour, vector<vector<vector<int>>>& panelColourIndex, vector<vector<vector<string>>>& panelText, string unit, vector<vector<string>>& modelData, vector<double> minMaxY)
{
	// Render the active bar graph and its parameter panels on a single page. 
	if (wjrPDFbargraph != nullptr) { wjrPDFbargraph.reset(); }
	wjrPDFbargraph = makeWJRPDF();
	int tableFontSize = 12;
	int cellPadding = 2;
	wjrPDFbargraph->jpdf.setFontSize(tableFontSize);
	int parameterColourWidth = 8;
	double xLabelRotation = 30.0;  // Degrees

	// Firstly, render the parameter panels at the bottom.
	int indexTable, maxBar, numRow, spaceUsed;
	vector<int> vNumLine;
	string sTitle;
	string separator = "|";
	double textWidth;
	vector<double> vRowHeight;
	vector<vector<double>> panelBLTR, vColourBar;
	vector<int> vColour = { 1, 2 };  // White, grey alternating.
	vector<double> pageDimensions = wjrPDFbargraph->jpdf.getPageDimensions();
	wjrPDFbargraph->jpdf.cursor[0] = wjrPDFbargraph->jpdf.margin;
	wjrPDFbargraph->jpdf.cursor[1] = wjrPDFbargraph->jpdf.margin;
	for (int ii = 2; ii >= 0; ii--)  // For every panel...
	{
		if (panelText[ii].size() < 1) { continue; }
		switch (ii)
		{
		case 0:
			sTitle = "Unique Parameters";
			break;
		case 1:
			sTitle = "Differentiating Parameters";
			break;
		case 2:
			sTitle = "Common Parameters";
			break;
		}
		numRow = panelColourIndex[ii].size();

		// Draw the background, panel frame, and title.
		maxBar = 0;
		for (int jj = 0; jj < numRow; jj++)
		{
			if (panelColourIndex[ii][jj].size() > maxBar) { maxBar = panelColourIndex[ii][jj].size(); }
		}
		spaceUsed = (maxBar * parameterColourWidth) + (4 * cellPadding);
		textWidth = pageDimensions[0] - (double)spaceUsed;
		vRowHeight.resize(numRow);
		vNumLine.resize(numRow);
		for (int jj = 0; jj < numRow; jj++)
		{
			vNumLine[jj] = wjrPDFbargraph->jpdf.getNumLines(panelText[ii][jj], textWidth, tableFontSize);
			vRowHeight[jj] = (double)((tableFontSize + (2 * cellPadding)) * vNumLine[jj]);
		}
		indexTable = wjrPDFbargraph->jpdf.addTable(1, vNumLine, vRowHeight, sTitle, (double)(tableFontSize + 2));
		wjrPDFbargraph->jpdf.vSection[indexTable]->jpTable->setColourBackground(vColour);
		panelBLTR = wjrPDFbargraph->jpdf.vSection[indexTable]->jpTable->drawTable();
		for (int jj = 0; jj < numRow; jj++)  // For every row...
		{
			vColourBar.resize(panelColourIndex[ii][jj].size());
			for (int kk = 0; kk < vColourBar.size(); kk++)  // For every series colour...
			{
				vColourBar[kk] = seriesColour[panelColourIndex[ii][jj][kk]];
			}
			wjrPDFbargraph->jpdf.vSection[indexTable]->jpTable->addColourBars(vColourBar, jj, 0);
		}
		wjrPDFbargraph->jpdf.vSection[indexTable]->jpTable->drawColourBars(parameterColourWidth);
		wjrPDFbargraph->jpdf.vSection[indexTable]->jpTable->drawColSplit();

		for (int jj = 0; jj < panelText[ii].size(); jj++)
		{
			wjrPDFbargraph->jpdf.vSection[indexTable]->jpTable->addTextList(panelText[ii][jj], jj, 0);
		}
		wjrPDFbargraph->jpdf.vSection[indexTable]->jpTable->drawTextList(2, separator);

		wjrPDFbargraph->jpdf.cursor[1] += (panelBLTR[1][1] - panelBLTR[0][1]) + (4.0 * (double)cellPadding);
	}

	// Use the remaining space on the page to render the bar graph.
	wjrPDFbargraph->jpdf.cursor[1] += (2.0 * (double)cellPadding);
	vector<vector<double>> bargraphBLTR(2, vector<double>(2));
	bargraphBLTR[0] = wjrPDFbargraph->jpdf.cursor;
	bargraphBLTR[1][0] = bargraphBLTR[0][0] + pageDimensions[0];
	bargraphBLTR[1][1] = wjrPDFbargraph->jpdf.margin + pageDimensions[1];
	int indexBarGraph = wjrPDFbargraph->jpdf.addBargraph(bargraphBLTR);
	int numRegion = modelData.size();
	vector<string> xValues(numRegion);
	for (int ii = 0; ii < numRegion; ii++) {
		xValues[ii] = modelData[ii][0];
	}
	wjrPDFbargraph->jpdf.vSection[indexBarGraph]->jpBargraph->initAxisY(minMaxY, unit);
	wjrPDFbargraph->jpdf.vSection[indexBarGraph]->jpBargraph->drawAxisX(xValues, xLabelRotation);
	wjrPDFbargraph->jpdf.vSection[indexBarGraph]->jpBargraph->drawAxisY(minMaxY, unit);
	int numSeries = modelData[0].size() - 1;
	vector<double> regionData(numSeries);
	for (int ii = 0; ii < numRegion; ii++)
	{
		for (int jj = 0; jj < numSeries; jj++)
		{
			regionData[jj] = (stod(modelData[ii][jj + 1]) - minMaxY[0]) / (minMaxY[1] - minMaxY[0]);
		}
		wjrPDFbargraph->jpdf.vSection[indexBarGraph]->jpBargraph->addRegionData(ii, regionData);
	}
	wjrPDFbargraph->jpdf.vSection[indexBarGraph]->jpBargraph->drawData(seriesColour);

	// Make a local image file for the given PDF.
	sessionCounter++;
	auto app = Wt::WApplication::instance();
	string unique = app->sessionId() + getSessionCounter(8);
	wjrPDFbargraph->jpdf.getPDF(wjrPDFbargraph->binPDF);
	long long sizePNG;
	wjrPDFbargraph->pathPNG = gs.binToPng(wjrPDFbargraph->binPDF, unique, sizePNG);
	vector<unsigned char> binPNG = wf.loadBin(wjrPDFbargraph->pathPNG);
	wmrPNGbargraph = make_shared<Wt::WMemoryResource>("png", binPNG);

	// Paint the preview screen.
	updateStackedPB(selectedMode);
	updateStackedPreview(selectedMode);
}
void WJDOWNLOAD::displayPDFmap(vector<vector<string>>& vvsParameter, vector<int>& vChanged)
{
	// Render the active map and its parameter list on a single page. 
	if (wjrPDFmap != nullptr) { wjrPDFmap.reset(); }
	wjrPDFmap = makeWJRPDF();
	int tableFontSize = 12;
	int cellPadding = 2, sectionPadding = 6;
	int numCol = 2;

	// Firstly, render the parameter section at the bottom.
	string separator = "|";
	vector<int> vColour(vChanged.size());
	for (int ii = 0; ii < vColour.size(); ii++)
	{
		if (vChanged[ii] > 0) 
		{ 
			if (ii % 2 == 1) { vColour[ii] = 4; }
			else { vColour[ii] = 3; }
		}
		else
		{
			if (ii % 2 == 1) { vColour[ii] = 2; }
			else { vColour[ii] = 1; }
		}
	}
	wjrPDFmap->jpdf.setFontSize(tableFontSize);
	vector<double> pageDimensions = wjrPDFmap->jpdf.getPageDimensions();
	wjrPDFmap->jpdf.cursor[0] = wjrPDFmap->jpdf.margin;
	wjrPDFmap->jpdf.cursor[1] = wjrPDFmap->jpdf.margin;
	double textWidth = pageDimensions[0] - (2 * cellPadding);
	int numRow = vvsParameter.size();
	vector<int> vNumLine(numRow);
	vector<double> vRowHeight(numRow);
	for (int ii = 0; ii < numRow; ii++)
	{
		vNumLine[ii] = wjrPDFmap->jpdf.getNumLines(vvsParameter[ii], textWidth, tableFontSize);
		vRowHeight[ii] = (double)((tableFontSize + (2 * cellPadding)) * vNumLine[ii]);
	}
	int indexParameter = wjrPDFmap->jpdf.addTable(1, vNumLine, vRowHeight, "Parameters", (double)(tableFontSize + 2));
	wjrPDFmap->jpdf.vSection[indexParameter]->jpTable->setColourBackground(vColour);
	vector<vector<double>> parameterBLTR = wjrPDFmap->jpdf.vSection[indexParameter]->jpTable->drawTable();
	for (int ii = 0; ii < numRow; ii++)
	{
		wjrPDFmap->jpdf.vSection[indexParameter]->jpTable->addTextList(vvsParameter[ii], ii, 0);
	}
	wjrPDFmap->jpdf.vSection[indexParameter]->jpTable->drawTextList(2, separator);
	wjrPDFmap->jpdf.cursor[0] = parameterBLTR[0][0];
	wjrPDFmap->jpdf.cursor[1] = parameterBLTR[1][1] + (double)sectionPadding;

	// Then, add a coloured region section lacking data values.
	numRow = mapRegion.size() / numCol;
	if (mapRegion.size() % numCol > 0) { numRow++; }
	vRowHeight.assign(numRow, (double)(tableFontSize + (2 * cellPadding)));
	vNumLine.assign(numRow, 1);
	int indexTable = wjrPDFmap->jpdf.addTable(numCol, vNumLine, vRowHeight, "Geographic Region Data", (double)(tableFontSize + 2));
	vColour = { 1, 2 };  // Alternating white and weak selection.
	wjrPDFmap->jpdf.vSection[indexTable]->jpTable->setColourBackground(vColour);
	vector<vector<double>> regionBLTR = wjrPDFmap->jpdf.vSection[indexTable]->jpTable->drawTable();
	wjrPDFmap->jpdf.cursor[0] = regionBLTR[0][0];
	wjrPDFmap->jpdf.cursor[1] = regionBLTR[1][1] + (double)sectionPadding;

	// Add the map section and legend bar(s).
	vector<vector<double>> mapBLTR(2, vector<double>(2));
	mapBLTR[0] = wjrPDFmap->jpdf.cursor;
	mapBLTR[1][0] = pageDimensions[0] + wjrPDFmap->jpdf.margin;
	mapBLTR[1][1] = pageDimensions[1] + wjrPDFmap->jpdf.margin;
	int indexMap = wjrPDFmap->jpdf.addMap(mapBLTR);
	wjrPDFmap->jpdf.vSection[indexMap]->jpMap->setValues(mapRegion, mapData);
	wjrPDFmap->jpdf.vSection[indexMap]->jpMap->initScaleBar(sUnit, displayData, legendBarDouble, legendTickLines);

	// Convert the raw coordinate data into an HPDF-friendly format.
	int decimalPlaces = -1;
	wjrPDFmap->jpdf.vSection[indexMap]->jpMap->kmToPixel(mapFrameKM, mapBorderKM);
	unordered_map<string, double> mapRegionData = wjrPDFmap->jpdf.vSection[indexMap]->jpMap->getMapRegionData(decimalPlaces);

	// Insert data and region names into the region section.
	vector<string> vsValue(mapRegion.size());
	string suffix;
	double dVal;
	vector<string> vsRegion;
	vsRegion.assign(mapRegion.begin() + 1, mapRegion.end());
	jf.sortAlphabetically(vsRegion);
	if (displayData.size() > 1 || sUnit[0] == '%')
	{
		suffix = " %";
		dVal = mapRegionData.at(mapRegion[0]);
		vsValue[0] = jf.doubleToCommaString(dVal, 1) + suffix;
		for (int ii = 0; ii < vsRegion.size(); ii++)
		{
			dVal = mapRegionData.at(vsRegion[ii]);
			vsValue[ii + 1] = jf.doubleToCommaString(dVal, 1) + suffix;
		}
	}
	else if (sUnit[0] == '$')
	{
		suffix = " $";
		dVal = mapRegionData.at(mapRegion[0]);
		vsValue[0] = jf.doubleToCommaString(dVal, 0) + suffix;
		for (int ii = 0; ii < vsRegion.size(); ii++)
		{
			dVal = mapRegionData.at(vsRegion[ii]);
			vsValue[ii + 1] = jf.doubleToCommaString(dVal, 0) + suffix;
		}
	}
	else if (sUnit[sUnit.size() - 1] == '$') {
		dVal = mapRegionData.at(mapRegion[0]);
		if (dVal < 10.0) { vsValue[0] = jf.doubleToCommaString(dVal, decimalPlaces); }
		else if (dVal < 100.0) { vsValue[0] = jf.doubleToCommaString(dVal, decimalPlaces - 1); }
		else { vsValue[0] = jf.doubleToCommaString(dVal, decimalPlaces - 2); }
		for (int ii = 0; ii < vsRegion.size(); ii++)
		{
			dVal = mapRegionData.at(vsRegion[ii]);
			if (dVal < 10.0) { vsValue[ii + 1] = jf.doubleToCommaString(dVal, decimalPlaces); }
			else if (dVal < 100.0) { vsValue[ii + 1] = jf.doubleToCommaString(dVal, decimalPlaces - 1); }
			else { vsValue[ii + 1] = jf.doubleToCommaString(dVal, decimalPlaces - 2); }
		}
	}
	else 
	{ 
		dVal = mapRegionData.at(mapRegion[0]);
		vsValue[0] = jf.doubleToCommaString(dVal, decimalPlaces) + suffix;
		for (int ii = 0; ii < vsRegion.size(); ii++)
		{
			dVal = mapRegionData.at(vsRegion[ii]);
			vsValue[ii + 1] = jf.doubleToCommaString(dVal, decimalPlaces);
		}
	}
	vsRegion.insert(vsRegion.begin(), mapRegion[0]);
	wjrPDFmap->jpdf.vSection[indexTable]->jpTable->addText(vsValue);
	wjrPDFmap->jpdf.vSection[indexTable]->jpTable->drawText(0);
	wjrPDFmap->jpdf.vSection[indexTable]->jpTable->drawColSplit();
	wjrPDFmap->jpdf.vSection[indexTable]->jpTable->addText(vsRegion);
	wjrPDFmap->jpdf.vSection[indexTable]->jpTable->drawText(1);

	// Render the map and legend bar using the data as colour.	
	wjrPDFmap->jpdf.vSection[indexMap]->jpMap->drawMap();

	// Make a local image file for the given PDF.
	sessionCounter++;
	auto app = Wt::WApplication::instance();
	string unique = app->sessionId() + getSessionCounter(8);
	wjrPDFmap->jpdf.getPDF(wjrPDFmap->binPDF);
	long long sizePNG;
	wjrPDFmap->pathPNG = gs.binToPng(wjrPDFmap->binPDF, unique, sizePNG);
	vector<unsigned char> binPNG = wf.loadBin(wjrPDFmap->pathPNG);
	if (wmrPNGmap != nullptr) { wmrPNGmap.reset(); }
	wmrPNGmap = make_shared<Wt::WMemoryResource>("png", binPNG);

	// Paint the preview screen.
	updateStackedPB(selectedMode);
	updateStackedPreview(selectedMode);
}
void WJDOWNLOAD::downloadSettings(int mode)
{
	selectedMode = mode;

	// Temporarily disable the download button until the data has been received.
	stackedPB->setDisabled(1);

	// Update the default name, if the default name has not yet been changed.
	Wt::WString wsTemp = leFileName->text();
	string fileName = wsTemp.toUTF8();
	size_t pos1 = fileName.find("Default");
	if (pos1 < fileName.size() && mapRegion.size() > 0)
	{
		switch (mode)
		{
		case 0:
			fileName = "Bar Graph (" + mapRegion[0] + ") Default";
			break;
		case 1:
			fileName = "Data Map (" + mapRegion[0] + ") Default";
			break;
		case 2:
			fileName = "Data Table (" + mapRegion[0] + ") Default";
			break;
		}
		wsTemp = Wt::WString::fromUTF8(fileName);
		leFileName->setText(wsTemp);
	}
	else if (pos1 < fileName.size())
	{
		switch (mode)
		{
		case 0:
			fileName = "Bar Graph Default";
			break;
		case 1:
			fileName = "Data Map Default";
			break;
		case 2:
			fileName = "Data Table Default";
			break;
		}
		wsTemp = Wt::WString::fromUTF8(fileName);
		leFileName->setText(wsTemp);
	}
	adjustLineEditWidth();

	// Update the file extension.
	if (selectedMode == 2) { wsTemp = ".csv"; }
	else { wsTemp = ".pdf"; }
	textExt->setText(wsTemp);

	// Send out a signal to fetch the requested data.
	previewSignal_.emit(selectedMode);
}
int WJDOWNLOAD::getLegendBarDouble(vector<string>& vsRegion, string sUnit, int displayDataSize)
{
	if (vsRegion.size() > 2 && sUnit == "# of persons" && displayDataSize == 1) { return 1; }
	else if (vsRegion[0] == "Canada") { return 2; }
	else { return 0; }
}
int WJDOWNLOAD::getLegendTickLines(string sUnit)
{
	if (sUnit == "$" || sUnit == "%") { return 1; }
	else { return 2; }
}
int WJDOWNLOAD::getRadioIndex()
{
	int index = -1;
	if (wbGroup != nullptr)
	{
		index = wbGroup->checkedId();
	}
	return index;
}
string WJDOWNLOAD::getSessionCounter(int length)
{
	// Returns a unique number for this sessionID.
	string session = to_string(sessionCounter);
	while (session.size() < length) {
		session.insert(session.begin(), '0');
	}
	return session;
}
void WJDOWNLOAD::init()
{
	initColour();

	auto app = Wt::WApplication::instance();
	string appPath = app->appRoot() + "gswin64c.exe";
	string docFolder = app->docRoot();
	gs.init(appPath, docFolder);

	this->setMaximumSize(maxWidth, maxHeight);
	auto vLayout = make_unique<Wt::WVBoxLayout>();

	auto fileNameBox = makeFileNameBox();
	vLayout->addWidget(move(fileNameBox));

	auto hLayout = make_unique<Wt::WHBoxLayout>();
	auto downloadBox = makeDownloadBox(0);
	boxDownload = hLayout->addWidget(move(downloadBox));

	auto stackedPreviewUnique = make_unique<Wt::WStackedWidget>();
	stackedPreview = stackedPreviewUnique.get();
	auto previewBar = make_unique<Wt::WImage>();
	stackedPreview->addWidget(move(previewBar));
	auto previewMap = make_unique<Wt::WImage>();
	stackedPreview->addWidget(move(previewMap));
	auto textArea = make_unique<Wt::WTextArea>();
	stackedPreview->addWidget(move(textArea));
	hLayout->addWidget(move(stackedPreviewUnique));
	
	vLayout->addLayout(move(hLayout));
	vLayout->addStretch(1);
	this->setLayout(move(vLayout));
}
void WJDOWNLOAD::initColour()
{
	wcBlack = Wt::WColor(0, 0, 0);
	wcSelected = Wt::WColor(200, 200, 255);
	wcWhite = Wt::WColor(255, 255, 255);
}
void WJDOWNLOAD::initJPDF(JPDF& jpdf)
{
	auto app = Wt::WApplication::instance();
	string fontPath = app->docRoot() + "/font/framd.ttf";
	string fontPathItalic = app->docRoot() + "/font/framdit.ttf";
	jpdf.setFont(fontPath, fontPathItalic);
}
void WJDOWNLOAD::initMap(vector<string>& region, vector<vector<vector<double>>>& frame, vector<vector<vector<double>>>& border, vector<vector<double>>& data)
{
	// Save the raw pixel data from the active map, for possible rendering later.
	mapRegion = region;
	mapFrameKM = frame;
	mapBorderKM = border;
	mapData = data;
}
Wt::WString WJDOWNLOAD::initStyleCSS(shared_ptr<Wt::WMemoryResource>& wmrCSS)
{
	// Complete CSS style sheets for PDF rendering.
	vector<unsigned char> binCSS = wmrCSS->data();
	string sStyle;
	sStyle.resize(binCSS.size());
	for (int ii = 0; ii < binCSS.size(); ii++)
	{
		sStyle[ii] = (char)binCSS[ii];
	}
	Wt::WString wsStyle = Wt::WString::fromUTF8(sStyle);
	return wsStyle;
}
unique_ptr<Wt::WContainerWidget> WJDOWNLOAD::makeDownloadBox(bool horizontal)
{
	auto box = make_unique<Wt::WContainerWidget>();
	auto layoutUnique = make_unique<Wt::WVBoxLayout>();
	auto vLayout = box->setLayout(move(layoutUnique));
	auto group = make_shared<Wt::WButtonGroup>();
	swap(group, wbGroup);
	Wt::WRadioButton* wrb = nullptr;

	auto boxDummy = make_unique<Wt::WContainerWidget>();
	boxDummy->setMinimumSize(wlAuto, 20.0);
	boxDummy->setMaximumSize(wlAuto, 20.0);

	if (horizontal) {
		auto hLayout = make_unique<Wt::WHBoxLayout>();
		wrb = hLayout->addWidget(make_unique<Wt::WRadioButton>("Bar Graph (PDF)"));
		wrb->setInline(horizontal);
		wbGroup->addButton(wrb);
		wrb = hLayout->addWidget(make_unique<Wt::WRadioButton>("Data Map (PDF)"));
		wrb->setInline(horizontal);
		wbGroup->addButton(wrb);
		wrb = hLayout->addWidget(make_unique<Wt::WRadioButton>("Data Table (CSV)"));
		wrb->setInline(horizontal);
		wbGroup->addButton(wrb);		
		vLayout->addLayout(move(hLayout));
	}
	else {
		box->setMaximumSize(120.0, wlAuto);
		wrb = vLayout->addWidget(make_unique<Wt::WRadioButton>("Bar Graph (PDF)"));
		wrb->setInline(horizontal);
		wbGroup->addButton(wrb);
		wrb = vLayout->addWidget(make_unique<Wt::WRadioButton>("Data Map (PDF)"));
		wrb->setInline(horizontal);
		wbGroup->addButton(wrb);
		wrb = vLayout->addWidget(make_unique<Wt::WRadioButton>("Data Table (CSV)"));
		wrb->setInline(horizontal);
		wbGroup->addButton(wrb);

		vLayout->addWidget(move(boxDummy));
	}

	wbGroup->checkedChanged().connect([=](Wt::WRadioButton* wrb) {
			Wt::WButtonGroup* wbg = wrb->group();
			int index = wbg->id(wrb);
			downloadSettings(index);
		});

	if (!stackedPB) {
		auto dlUnique = make_unique<Wt::WStackedWidget>();
		dlUnique->setMinimumSize(wlAuto, 42.0);
		for (int ii = 0; ii < numDLtypes; ii++)
		{
			auto pbUnique = make_unique<Wt::WPushButton>("Download");
			pbUnique->decorationStyle().setBackgroundColor(wcSelected);
			dlUnique->addWidget(move(pbUnique));
		}
		dlUnique->setCurrentIndex(0);
		dlUnique->setDisabled(1);
		stackedPB = vLayout->addWidget(move(dlUnique), 0, Wt::AlignmentFlag::Center);
	}
	else {
		auto boxLayout = (Wt::WVBoxLayout*)boxDownload->layout();
		auto unique = boxLayout->removeWidget(stackedPB);
		stackedPB = (Wt::WStackedWidget*)unique.get();
		vLayout->addWidget(move(unique), 0, Wt::AlignmentFlag::Center);
	}

	vLayout->addStretch(1);
	return box;
}
unique_ptr<Wt::WContainerWidget> WJDOWNLOAD::makeFileNameBox()
{
	auto box = make_unique<Wt::WContainerWidget>();
	auto fileNameLayout = make_unique<Wt::WHBoxLayout>();
	auto fileName = make_unique<Wt::WText>("Filename: ");
	fileNameLayout->addWidget(move(fileName), 0, Wt::AlignmentFlag::Left | Wt::AlignmentFlag::Middle);
	auto lineEditUnique = make_unique<Wt::WLineEdit>();
	lineEditUnique->setText("SCDA Default");
	lineEditUnique->setMaximumSize(700.0, wlAuto);
	lineEditUnique->setMinimumSize(120.0, 46.0);
	lineEditUnique->textInput().connect(this, &WJDOWNLOAD::adjustLineEditWidth);
	leFileName = fileNameLayout->addWidget(move(lineEditUnique), 0, Wt::AlignmentFlag::Left | Wt::AlignmentFlag::Middle);
	auto fileExtension = make_unique<Wt::WText>(".pdf");
	textExt = fileNameLayout->addWidget(move(fileExtension), 0, Wt::AlignmentFlag::Left | Wt::AlignmentFlag::Middle);
	fileNameLayout->addStretch(1);
	box->setLayout(move(fileNameLayout));
	return box;
}
shared_ptr<WJRCSV> WJDOWNLOAD::makeWJRCSV(string& sCSV)
{
	Wt::WString wsFileName = leFileName->text() + textExt->text();
	auto csvTemp = make_shared<WJRCSV>(wsFileName);
	auto buffer = new unsigned char[1];
	csvTemp->setData(buffer, 1);
	csvTemp->length = sCSV.size();
	csvTemp->sCSV = sCSV;
	delete[] buffer;
	return csvTemp;
}
shared_ptr<WJRPDF> WJDOWNLOAD::makeWJRPDF()
{
	Wt::WString wsFileName = leFileName->text() + textExt->text();
	auto pdfTemp = make_shared<WJRPDF>(wsFileName);
	initJPDF(pdfTemp->jpdf);  // Font.
	return pdfTemp;
}
shared_ptr<WJRPDF> WJDOWNLOAD::makeWJRPDF(string& sName)
{
	Wt::WString wsFileName = Wt::WString::fromUTF8(sName);
	auto pdfTemp = make_shared<WJRPDF>(wsFileName);
	initJPDF(pdfTemp->jpdf);  // Font.
	return pdfTemp;
}
void WJDOWNLOAD::setRadioEnabled(vector<int> vEnabled)
{
	auto vButton = wbGroup->buttons();
	if (vButton.size() != vEnabled.size()) { jf.err("Size mismatch-wjdownload.setRadioEnabled"); }
	for (int ii = 0; ii < vButton.size(); ii++) {
		vButton[ii]->setEnabled(vEnabled[ii]);
	}
}
void WJDOWNLOAD::setUnit(string unit, vector<int> viIndex)
{
	sUnit = unit;
	displayData = viIndex;
	legendBarDouble = getLegendBarDouble(mapRegion, unit, viIndex.size());
	legendTickLines = getLegendTickLines(unit);
}
Wt::WString WJDOWNLOAD::stringToHTML(string& sLine)
{
	Wt::WString wsLine = "<p>" + Wt::WString::fromUTF8(sLine + "\n") + "</p>";
	return wsLine;
}
void WJDOWNLOAD::updateStackedPB(int index)
{
	// This function specifically re-creates the pushButton within the "download"
	// stacked widget, using the latest WJR struct. The pushButton must be recreated
	// (rather than relinked) due to the nature of (or a bug in) WT.
	Wt::WPushButton* pbDownload = (Wt::WPushButton*)stackedPB->widget(index);
	auto oldUnique = stackedPB->removeWidget(pbDownload);
	Wt::WString wsLabel = oldUnique->text();
	Wt::WCssDecorationStyle& style = oldUnique->decorationStyle();
	auto pbUnique = make_unique<Wt::WPushButton>();
	switch (index)
	{
	case 0:
		pbUnique->setLink(Wt::WLink(wjrPDFbargraph));
		break;
	case 1:
		pbUnique->setLink(Wt::WLink(wjrPDFmap));
		break;
	case 2:
		pbUnique->setLink(Wt::WLink(wjrCSV));
		break;
	}	
	pbUnique->setText(wsLabel);
	pbUnique->setDecorationStyle(style);
	stackedPB->insertWidget(index, move(pbUnique));
	stackedPB->setCurrentIndex(index);
	stackedPB->setDisabled(0);
}
void WJDOWNLOAD::updateStackedPreview(int index)
{
	// This function specifically re-creates the preview widget within the "preview"
	// stacked widget, using the latest WJR struct. The preview widget must be recreated
	// (rather than relinked) due to the nature of (or a bug in) WT.
	switch (index)
	{
	case 0:
	{
		Wt::WImage* wImageOld = (Wt::WImage*)stackedPreview->widget(index);
		stackedPreview->removeWidget(wImageOld);
		Wt::WLink wLink = Wt::WLink(wmrPNGbargraph);
		auto wImage = make_unique<Wt::WImage>(wLink);
		double imageWidth = maxWidth.value() - 200.0;
		double imageHeight = round(imageWidth * 1.294117647);
		Wt::WLength wlWidth = Wt::WLength(imageWidth);
		Wt::WLength wlHeight = Wt::WLength(imageHeight);
		wImage->setMinimumSize(wlWidth, wlHeight);
		wImage->setMaximumSize(wlWidth, wlHeight);
		stackedPreview->insertWidget(index, move(wImage));
		break;
	}
	case 1:
	{
		Wt::WImage* wImageOld = (Wt::WImage*)stackedPreview->widget(index);
		stackedPreview->removeWidget(wImageOld);
		Wt::WLink wLink = Wt::WLink(wmrPNGmap);
		auto wImage = make_unique<Wt::WImage>(wLink);
		double imageWidth = maxWidth.value() - 200.0;
		double imageHeight = round(imageWidth * 1.294117647);
		Wt::WLength wlWidth = Wt::WLength(imageWidth);
		Wt::WLength wlHeight = Wt::WLength(imageHeight);
		wImage->setMinimumSize(wlWidth, wlHeight);
		wImage->setMaximumSize(wlWidth, wlHeight);
		stackedPreview->insertWidget(index, move(wImage));
		break;
	}
	case 2:
	{
		Wt::WTextArea* wTextAreaOld = (Wt::WTextArea*)stackedPreview->widget(index);
		auto oldUnique = stackedPreview->removeWidget(wTextAreaOld);
		Wt::WCssDecorationStyle& style = oldUnique->decorationStyle();
		auto wTextArea = make_unique<Wt::WTextArea>();
		Wt::WString wsTemp = Wt::WString::fromUTF8(sCSV);
		wTextArea->setText(wsTemp);
		wTextArea->setDecorationStyle(style);
		
		int numLine = jf.countChar(sCSV, '\n');
		auto minHeight = Wt::WLength((20 * numLine) + 40);
		auto minWidth = Wt::WLength::Auto;
		wTextArea->setMinimumSize(minWidth, minHeight);

		stackedPreview->insertWidget(index, move(wTextArea));
		break;
	}
	}
	stackedPreview->setCurrentIndex(index);
	stackedPreview->setDisabled(0);
}
void WJDOWNLOAD::widgetMobile(bool Mobile)
{
	if (Mobile == mobile) { return; }
	mobile = Mobile;
	Wt::WVBoxLayout* vLayout = (Wt::WVBoxLayout*)this->layout();
	Wt::WLayoutItem* wlItem = nullptr;
	if (mobile) {
		wlItem = vLayout->itemAt(1);
		auto hLayout = (Wt::WBoxLayout*)wlItem->layout();
		auto stackedPreviewUnique = hLayout->removeWidget(stackedPreview);

		auto downloadBox = makeDownloadBox(1);
		boxDownload = downloadBox.get();

		auto itemUniqueOld = vLayout->removeItem(wlItem);
		vLayout->insertWidget(1, move(downloadBox));
		vLayout->insertWidget(2, move(stackedPreviewUnique));
	}
	else
	{
		auto downloadBox = makeDownloadBox(0);

		auto stackedPreviewUnique = vLayout->removeWidget(stackedPreview);
		wlItem = vLayout->itemAt(2);
		vLayout->removeItem(wlItem);
		wlItem = vLayout->itemAt(1);
		vLayout->removeItem(wlItem);

		auto hLayout = make_unique<Wt::WHBoxLayout>();
		boxDownload = hLayout->addWidget(move(downloadBox));
		hLayout->addWidget(move(stackedPreviewUnique));
		vLayout->insertLayout(1, move(hLayout));
	}
}
