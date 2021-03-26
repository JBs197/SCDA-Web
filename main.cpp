#include "SCDAwindow.h"

using namespace std;
mutex m_err;
const string sroot = "F:";

int main()
{
	WINFUNC wf;
	const string exec_dir = wf.get_exec_dir();
	WTFUNC wtf;
	const vector<string> args = wtf.make_wrun_args(exec_dir);

	Wt::WRun(exec_dir, args, [](const Wt::WEnvironment& env) {
		return make_unique<SCDAwindow>(env);
	});

	return 0;
}