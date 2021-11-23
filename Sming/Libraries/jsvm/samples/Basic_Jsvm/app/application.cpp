#include <SmingCore.h>
#include <jsvm.h>
#include "JsvmTask.h"

namespace
{
Jsvm jsVm;
JsvmTask* jsTask;

constexpr char MAIN_JS_FILE[]{"main.js.snap"};

void startJsvm()
{
	// Load the snapshot file
	if(jsVm.exec(MAIN_JS_FILE) < 0) {
		debug_e("Failed executing the following script: %s", MAIN_JS_FILE);
		return;
	}

	// Now you can initilize your script by calling a setup() JavaScript function
	if(!jsVm.runFunction("setup")) {
		debug_e("Failed executing the setup function.");
	}

	// And run the loop JavaScript function in the background as a Sming task.
	jsTask = new JsvmTask(jsVm);
	jsTask->resume();
}

} // namespace

void init()
{
	Serial.begin(SERIAL_BAUD_RATE); // 115200 by default
	Serial.systemDebugOutput(true); // Enable debug output to serial

	// Mount file system, in order to work with files
	if(!spiffs_mount()) {
		Serial.println("Unable to mount the JS file!");
		return;
	}

	startJsvm();
}
