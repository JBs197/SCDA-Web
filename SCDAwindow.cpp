#include "SCDAwindow.h"

using namespace std;

SCDAwindow::SCDAwindow(const Wt::WEnvironment& env) : Wt::WApplication(env)
{
	app_box = root()->addWidget(make_unique<Wt::WPanel>());
	wtf.init_app_box(app_box);
}

