#include <SmingCore.h>
#include <Adafruit_VL53L0X.h>
#include <tof_gestures.h>
#include <tof_gestures_SWIPE_1.h>

// GPIO - NodeMCU pins
#define SDA 4	// D2
#define SCL 5	// D1
#define XSHUT 14 // D5
#define INT 12   // D6

Adafruit_VL53L0X lox = Adafruit_VL53L0X();

// Gesture structure.
Gesture_SWIPE_1_Data_t gestureSwipeData;

// Range value
uint32_t distance_top;

Timer loopTimer;

void loop()
{
	VL53L0X_RangingMeasurementData_t measure;

	Serial.print("Reading a measurement... ");
	lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!

	if(measure.RangeStatus != 4) { // phase failures have incorrect data
		Serial.print("Distance (mm): ");
		Serial.println(measure.RangeMilliMeter);
		distance_top = measure.RangeMilliMeter;
	} else {
		Serial.println(" out of range ");
		distance_top = 1200;
	}

	// Launch gesture detection algorithm.
	int gestureCode = tof_gestures_detectSWIPE_1(distance_top, &gestureSwipeData);

	// Check the result of the gesture detection algorithm.
	String gesture = "NONE";

	switch(gestureCode) {
	case GESTURES_NULL:
		break;
	case GESTURES_SINGLE_TAP:
		gesture = "Single tap detected : __TAP__";
		break;
	case GESTURES_SINGLE_SWIPE:
		gesture = "Single swipe detected (flat hand) : __WIPE__";
		break;
	case GESTURES_DOUBLE_TAP:
		gesture = "Double tap detected : __TAPS__";
		break;
	case GESTURES_DOUBLE_SWIPE:
		gesture = "Double swipe detected : __SWIPES__";
		break;
	case GESTURES_HAND_ENTERING:
		gesture = "Hand is entering in device field of view : __LC__ ";
		break;
	case GESTURES_HAND_LEAVING:
		gesture = "Hand is leaving from device field of view : __LC__";
		break;
	case GESTURES_LEVEL_CONTROLLED:
		gesture = "Hand is controlling a level from the measured distance : __LC__";
		break;
	case GESTURES_SWIPE_LEFT_RIGHT:
		gesture = "Directional swipe from left to right : __DIRSWIPE__";
		break;
	case GESTURES_SWIPE_RIGHT_LEFT:
		gesture = "Directional swipe from right to left : __DIRSWIPE__";
		break;
	case GESTURES_FLIP_PAGE_LEFT_RIGHT:
		gesture = "Page flip from left to right (hand at 45) : __FLIPPAGE__";
		break;
	case GESTURES_FLIP_PAGE_RIGHT_LEFT:
		gesture = "Page flip from right to left (hand at 45) : __FLIPPAGE__";
		break;
	case GESTURES_STARTED:
		gesture = " A gesture start is detected but full gesture detection is not completed (yet)";
		break;
	case GESTURES_DISCARDED:
		gesture = "Gesture is discarded";
		break;
	case GESTURES_DISCARDED_TOO_SLOW:
		gesture = "Gesture is discarded because it is too slow";
		break;
	case GESTURES_DISCARDED_TOO_FAST:
		gesture = "Gesture is discarded because it is too fast";
		break;
	default:
		break;
	}

	Serial.printf("Gesture: %s\n", gesture.c_str());
}

void init()
{
	Serial.begin(SERIAL_BAUD_RATE); // 115200 by default
	Serial.systemDebugOutput(true); // Enable debug output to serial

	// WIFI not needed for demo. So disabling WIFI.
	//WifiStation.enable(false);
	//WifiAccessPoint.enable(false);

	// Create components.
	Wire.begin(SDA, SCL);

	lox.setLongRangeMode(true);

	if(!lox.begin()) {
		Serial.println(F("Failed to boot VL53L0X"));
		while(1) {
		}
	}

	// Initialize gesture library.
	tof_gestures_initSWIPE_1(&gestureSwipeData);

	Serial.println(F("VL53L0X API Simple Ranging example\n\n"));
	loopTimer.initializeMs(100, loop).start();
}
