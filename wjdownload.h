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
#include <Wt/WFileResource.h>
#include <Wt/Render/WPdfRenderer.h>
#include "gsfunc.h"
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
	vector<unsigned char> binPDF;
	string error, pathPNG;
	JPDF jpdf;
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
	vector<int> displayData;
	GSFUNC gs;
	Wt::WLineEdit *leFileName;
	int legendBarDouble = -1;                // 0 = single bar, 1 = double bars, 2 = single bar with Canada.
	int legendFontSize = 12;
	int legendTickLines = -1;                // How many lines of text are needed at each tick mark on the bar.
	const double legendIdleThreshold = 0.4;  // Minimum percentage of bar unused, to trigger action.
	Wt::WLength maxWidth, maxHeight;
	vector<vector<vector<double>>> mapBorderKM, mapFrameKM;
	vector<vector<double>> mapData;
	vector<string> mapRegion;
	MATHFUNC mf;
	const int numDLtypes = 3;
	Wt::Signal<int> previewSignal_;
	int selectedMode = -1;
	int sessionCounter = 0;                  // For PDF/PNG temp files.
	Wt::WStackedWidget *stackedPB, *stackedPreview;
	Wt::WText *textExt;
	Wt::WToolBar *toolButtons;
	string sCSV, sUnit;
	vector<Wt::WLink> vLink;
	vector<Wt::WColor> vToolColour;
	WINFUNC wf;
	shared_ptr<WJRCSV> wjrCSV = nullptr;
	shared_ptr<WJRPDF> wjrPDFbargraph = nullptr;
	shared_ptr<Wt::WMemoryResource> wmrPNGbargraph = nullptr;
	shared_ptr<WJRPDF> wjrPDFmap = nullptr;
	shared_ptr<Wt::WMemoryResource> wmrPNGmap = nullptr;
	WTPAINT* wpPreview = nullptr;

public:
	WJDOWNLOAD(Wt::WLength width, Wt::WLength height) 
		: Wt::WContainerWidget(), maxWidth(width), maxHeight(height) { init(); }
	~WJDOWNLOAD() {}

	JFUNC jf;
	Wt::WString styleTextPlain, styleTextShaded;
	shared_ptr<Wt::WButtonGroup> wbGroup = nullptr;
	Wt::WColor wcBlack, wcSelected, wcWhite;
	Wt::WLength wlAuto = Wt::WLength();

	void adjustLineEditWidth();
	void displayCSV(string& csv);
	void displayPDFbargraph(vector<vector<double>>& seriesColour, vector<vector<vector<int>>>& panelColourIndex, vector<vector<vector<string>>>& panelText, string unit, vector<vector<string>>& modelData, vector<double> minMaxY);
	void displayPDFmap(vector<vector<string>>& vvsParameter, vector<int>& vChanged);
	void downloadSettings(int mode);
	int getLegendBarDouble(vector<string>& vsRegion, string sUnit, int displayDataSize);
	int getLegendTickLines(string sUnit);
	int getRadioIndex();
	string getSessionCounter(int length);
	void init();
	void initColour();
	void initJPDF(JPDF& jpdf);
	void initMap(vector<string>& region, vector<vector<vector<double>>>& frame, vector<vector<vector<double>>>& border, vector<vector<double>>& data);
	Wt::WString initStyleCSS(shared_ptr<Wt::WMemoryResource>& wmrCSS);
	unique_ptr<Wt::WContainerWidget> makeDownloadBox();
	unique_ptr<Wt::WContainerWidget> makeFileNameBox();
	shared_ptr<WJRCSV> makeWJRCSV(string& sCSV);
	shared_ptr<WJRPDF> makeWJRPDF();
	shared_ptr<WJRPDF> makeWJRPDF(string& sName);
	Wt::Signal<int>& previewSignal() { return previewSignal_; }
	void setUnit(string unit, vector<int> viIndex);
	Wt::WString stringToHTML(string& sLine);
	void updateStackedPB(int index);
	void updateStackedPreview(int index);
};

