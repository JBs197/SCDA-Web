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

struct WJRCSV : public Wt::WMemoryResource
{
	uint64_t length;
	std::string sCSV;
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
	std::vector<unsigned char> binPDF;
	std::string error, pathPNG;
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
	Wt::WContainerWidget* boxDownload;
	std::vector<int> displayData;
	GSFUNC gs;
	JSORT jsort;
	JSTRING jstr;
	JNUMBER jnumber;
	Wt::WLineEdit *leFileName;
	int legendBarDouble = -1;                // 0 = single bar, 1 = double bars, 2 = single bar with Canada.
	int legendFontSize = 12;
	int legendTickLines = -1;                // How many lines of text are needed at each tick mark on the bar.
	const double legendIdleThreshold = 0.4;  // Minimum percentage of bar unused, to trigger action.
	Wt::WLength maxWidth, maxHeight;
	std::vector<std::vector<std::vector<double>>> mapBorderKM, mapFrameKM;
	std::vector<std::vector<double>> mapData;
	std::vector<std::string> mapRegion;
	bool mobile = 0;
	const int numDLtypes = 3;
	Wt::Signal<int> previewSignal_;
	int selectedMode = -1;
	int sessionCounter = 0;                  // For PDF/PNG temp files.
	Wt::WStackedWidget *stackedPB, *stackedPreview;
	Wt::WText *textExt;
	Wt::WToolBar *toolButtons;
	std::string sCSV, sUnit;
	std::vector<Wt::WColor> vToolColour;
	WINFUNC wf;
	std::shared_ptr<WJRCSV> wjrCSV = nullptr;
	std::shared_ptr<WJRPDF> wjrPDFbargraph = nullptr;
	std::shared_ptr<Wt::WMemoryResource> wmrPNGbargraph = nullptr;
	std::shared_ptr<WJRPDF> wjrPDFmap = nullptr;
	std::shared_ptr<Wt::WMemoryResource> wmrPNGmap = nullptr;
	WTPAINT* wpPreview = nullptr;

	void err(std::string message);

public:
	WJDOWNLOAD(Wt::WLength width, Wt::WLength height) 
		: Wt::WContainerWidget(), maxWidth(width), maxHeight(Wt::WLength::Auto) { init(); }
	~WJDOWNLOAD() {}

	Wt::WString styleTextPlain, styleTextShaded;
	std::shared_ptr<Wt::WButtonGroup> wbGroup = nullptr;
	Wt::WColor wcBlack, wcSelected, wcWhite;
	Wt::WLength wlAuto = Wt::WLength();

	void adjustLineEditWidth();
	void clear();
	void displayCSV(std::string& csv);
	void displayPDFbargraph(std::vector<std::vector<double>>& seriesColour, std::vector<std::vector<std::vector<int>>>& panelColourIndex, std::vector<std::vector<std::vector<std::string>>>& panelText, std::string unit, std::vector<std::vector<std::string>>& modelData, std::vector<double> minMaxY);
	void displayPDFmap(std::vector<std::vector<std::string>>& vvsParameter, std::vector<int>& vChanged);
	void downloadSettings(int mode);
	int getLegendBarDouble(std::vector<std::string>& vsRegion, std::string sUnit, int displayDataSize);
	int getLegendTickLines(std::string sUnit);
	int getRadioIndex();
	std::string getSessionCounter(int length);
	void init();
	void initColour();
	void initJPDF(JPDF& jpdf);
	void initMap(std::vector<std::string>& region, std::vector<std::vector<std::vector<double>>>& frame, std::vector<std::vector<std::vector<double>>>& border, std::vector<std::vector<double>>& data);
	Wt::WString initStyleCSS(std::shared_ptr<Wt::WMemoryResource>& wmrCSS);
	std::unique_ptr<Wt::WContainerWidget> makeDownloadBox(bool horizontal);
	std::unique_ptr<Wt::WContainerWidget> makeFileNameBox();
	std::shared_ptr<WJRCSV> makeWJRCSV(std::string& sCSV);
	std::shared_ptr<WJRPDF> makeWJRPDF();
	std::shared_ptr<WJRPDF> makeWJRPDF(std::string& sName);
	Wt::Signal<int>& previewSignal() { return previewSignal_; }
	void setRadioEnabled(std::vector<int> vEnabled);
	void setUnit(std::string unit, std::vector<int> viIndex);
	Wt::WString stringToHTML(std::string& sLine);
	void updateStackedPB(int index);
	void updateStackedPreview(int index);
	void widgetMobile(bool mobile);
};

