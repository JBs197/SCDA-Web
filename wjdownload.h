#pragma once
#define HPDF_DLL
#include <Wt/WStackedWidget.h>
#include <Wt/WVBoxLayout.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WTextArea.h>
#include <Wt/WLineEdit.h>
#include <Wt/WToolBar.h>
#include <Wt/WPushButton.h>
#include <Wt/WRadioButton.h>
#include <Wt/WPdfImage.h>
#include <Wt/Http/Request.h>
#include <Wt/Http/Response.h>
#include <Wt/WMemoryResource.h>
#include <Wt/Render/WPdfRenderer.h>
#include <hpdf.h>
#include "jscalebar.h"
#include "jpdf.h"
#include "wtpaint.h"
#pragma comment(lib, "libhpdfd.lib")

using namespace std;

struct WJRCSV : public Wt::WMemoryResource
{
	uint64_t length;
	string sCSV;
	WJRCSV(Wt::WString wsFileName) : Wt::WMemoryResource("csv")
	{ 
		setDispositionType(Wt::ContentDisposition::Attachment);
		suggestFileName(wsFileName); 
	}
	~WJRCSV() { beingDeleted(); }
	void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response) override;
};

struct WJRPDF : public Wt::WMemoryResource
{
	string error;
	JPDF jpdf;
	HPDF_UINT32 pdfSize;
	WJRPDF(Wt::WString wsFileName) : Wt::WMemoryResource("pdf")
	{
		setDispositionType(Wt::ContentDisposition::Attachment);
		suggestFileName(wsFileName);
	}
	~WJRPDF() { beingDeleted(); }
	void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response) override;
};

class WJDOWNLOAD : public Wt::WContainerWidget
{
	double barNumberHeight = 14.0;  // Unit of pixels, for a horizontal legend bar.
	double barNumberWidth = 100.0;  // Unit of pixels, for a vertical legend bar.
	double barThickness = 14.0;  // Unit of pixels.
	vector<int> displayData;
	vector<double> extraColour, unknownColour;
	unique_ptr<JSCALEBAR> jScaleBar = nullptr;
	vector<vector<double>> keyColour;  // Form [colour index][r, g, b].
	Wt::WLineEdit *leFileName;
	int legendBarDouble = -1;  // 0 = single bar, 1 = double bars, 2 = single bar with Canada.
	int legendFontSize = 12;
	int legendTickLines = -1;  // How many lines of text are needed at each tick mark on the bar.
	const double legendIdleThreshold = 0.4;  // Minimum percentage of bar unused, to trigger action.
	double legendMax, legendMin;
	Wt::WLength maxWidth, maxHeight;
	vector<vector<vector<double>>> mapBorder, mapFrame;
	vector<string> mapRegion;
	MATHFUNC mf;
	double pdfPPKM;
	const int numDLtypes = 4;
	Wt::Signal<int> previewSignal_;
	int selectedMode = -1, selIndex = -1;
	Wt::WStackedWidget *stackedPB, *stackedPreview;
	Wt::WTextArea *taPreview;
	Wt::WText *textExt;
	Wt::WToolBar *toolButtons;
	string sUnit;
	vector<Wt::WLink> vLink;
	vector<Wt::WColor> vToolColour;
	shared_ptr<Wt::WButtonGroup> wbGroup = nullptr;
	shared_ptr<WJRCSV> wjrCSV = nullptr;
	shared_ptr<WJRPDF> wjrPDFbargraph = nullptr, wjrPDFmap = nullptr;
	WTPAINT* wpPreview = nullptr;

public:
	WJDOWNLOAD(Wt::WLength width, Wt::WLength height) 
		: Wt::WContainerWidget(), maxWidth(width), maxHeight(height) { init(); }
	~WJDOWNLOAD() {}

	vector<vector<double>> barBLTR, imgBLTR;
	JFUNC jf;
	Wt::WString styleTextPlain, styleTextShaded;
	Wt::WColor wcBlack, wcSelected, wcWhite;
	Wt::WLength wlAuto = Wt::WLength();

	void adjustLineEditWidth();
	vector<vector<vector<double>>> borderKMToBorderPDF();
	void displaceChildToParent(vector<vector<double>>& vvdBorder, vector<vector<double>>& childFrame, vector<double> dispTL);
	void displaceParentToFrame(vector<vector<double>>& parentBorderKM, vector<vector<double>>& parentFrameKM);
	void displayCSV(string& sCSV);
	void displayPDFbargraph(vector<vector<double>>& seriesColour, vector<vector<vector<int>>>& panelColourIndex, vector<vector<vector<string>>>& panelText);
	void displayPDFmap(vector<string>& vsParameter, vector<int>& vChanged);
	void downloadSettings(int mode);
	vector<vector<double>> drawLegend(int indexDataset);
	void drawLegendTicks(vector<vector<double>> rectBLTR, vector<double>& tickValues, double tickThickness);
	void drawLegendTicks(vector<vector<double>> rectBLTR, vector<vector<double>>& tickValues, double tickThickness);
	void drawLegendTicks(vector<vector<double>> rectBLTR, vector<double>& tickValues, double parentValue, double dotRadius, double tickThickness);
	void drawGradientBar(vector<vector<double>> rectBLTR, double frameThickness);
	void drawGradientBar(vector<vector<double>> rectBLTR, vector<vector<double>> gradientBLTR, double frameThickness);
	void drawMap(vector<vector<vector<double>>>& vvvdBorder, vector<vector<double>>& vColour);
	vector<double> getChildTL(vector<vector<double>>& vpfBorder, vector<vector<double>>& childFrameKM, vector<vector<double>>& parentFrameKM);
	int getLegendBarDouble(vector<string>& vsRegion, string sUnit, int displayDataSize);
	int getLegendTickLines(string sUnit);
	int getRadioIndex();
	void init();
	void initColour();
	void initImgBar(vector<vector<double>>& mapBLTR, vector<vector<double>>& parentFrameKM);
	void initJPDF(JPDF& jpdf);
	void initMap(vector<string>& region, vector<vector<vector<double>>>& frame, vector<vector<vector<double>>>& border, vector<vector<double>>& data);
	Wt::WString initStyleCSS(shared_ptr<Wt::WMemoryResource>& wmrCSS);
	unique_ptr<Wt::WContainerWidget> makeDownloadBox();
	unique_ptr<Wt::WContainerWidget> makeFileNameBox();
	shared_ptr<WJRCSV> makeWJRCSV(string& sCSV);
	shared_ptr<WJRPDF> makeWJRPDF();
	Wt::Signal<int>& previewSignal() { return previewSignal_; }
	void scaleChildToPage(vector<vector<double>>& vpfBorder, vector<vector<double>>& vpfFrame);
	void scaleParentToPage(vector<vector<double>>& parentBorder, vector<vector<double>>& parentFrameKM);
	void setUnit(string unit, vector<int> viIndex);
	Wt::WString stringToHTML(string& sLine);
	void updateStackedPB(int index);
};

