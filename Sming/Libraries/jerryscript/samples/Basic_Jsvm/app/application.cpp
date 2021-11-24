#include <SmingCore.h>
#include <jsvm.h>
#include "JsvmTask.h"

namespace
{
Jsvm jsVm;
JsvmTask jsTask(jsVm);

IMPORT_FSTR(main_snap, PROJECT_DIR "/out/jerryscript/main.js.snap")

void startJsvm()
{
	if(!jsVm.load(main_snap)) {
		debug_e("Failed to load snapshot");
		return;
	}

	// Now you can initialize your script by calling a setup() JavaScript function
	if(!jsVm.runFunction("setup")) {
		debug_e("Failed executing the setup function.");
	}

	// And run the loop JavaScript function in the background as a Sming task.
	jsTask.resume();
}

} // namespace

void init()
{
	Serial.begin(SERIAL_BAUD_RATE);
	Serial.systemDebugOutput(true);

	startJsvm();
}
