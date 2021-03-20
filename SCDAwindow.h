#pragma once
#include <Wt/WApplication.h>
#include <Wt/WBreak.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WLineEdit.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>
#include <Wt/WPanel.h>
#include <string>
#include <vector>
#include "jfunc.h"
#include "winfunc.h"
#include "wtfunc.h"

using namespace std;

class SCDAwindow : public Wt::WApplication
{
	WTFUNC wtf;

	Wt::WPanel* app_box;

public:
	SCDAwindow(const Wt::WEnvironment& env);
	~SCDAwindow() {}

};

