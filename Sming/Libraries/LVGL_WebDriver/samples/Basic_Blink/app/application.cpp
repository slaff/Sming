#include <SmingCore.h>
#include <lvgl.h>
#include <lv_drivers/web_driver.h>

// If you want, you can define WiFi settings globally in Eclipse Environment Variables
#ifndef WIFI_SSID
#define WIFI_SSID "PleaseEnterSSID" // Put your SSID and password here
#define WIFI_PWD "PleaseEnterPass"
#endif

#define LED_PIN 2 // GPIO2

Timer ticker;
bool state = true;

constexpr int MONITOR_HOR_RES = 240;
constexpr int MONITOR_VER_RES = 320;

void setPixel(int32_t x, int32_t y, lv_color_t color)
{
	debug_d("%d:%d=>%d", x, y, color.full);
}

void flushDisplay(lv_disp_drv_t* disp, const lv_area_t* area, lv_color_t* color_p)
{
	int32_t x, y;
	/*It's a very slow but simple implementation.
	 *`set_pixel` needs to be written by you to a set pixel on the screen*/
	for(y = area->y1; y <= area->y2; y++) {
		for(x = area->x1; x <= area->x2; x++) {
			setPixel(x, y, *color_p);
			color_p++;
		}
	}

	lv_disp_flush_ready(disp); /* Indicate you are ready with the flushing*/
}

void blink()
{
	state = !state;
}

void tick()
{
	lv_tick_inc(
		100); // Call lv_tick_inc(x) every x milliseconds in a Timer or Task (x should be between 1 and 10). It is required for the internal timing of LVGL.
	lv_timer_handler();
}

// Initialize the Hardware Abstraction Layer (HAL) for the LVGL graphics
void initHal()
{
	/*Create a display buffer*/
	static lv_disp_draw_buf_t disp_buf1;
	constexpr int DISPLAY_BUFFER_LENGTH = 8 * 1024;
	static lv_color_t buf1_1[DISPLAY_BUFFER_LENGTH];
	static lv_color_t buf1_2[DISPLAY_BUFFER_LENGTH];
	lv_disp_draw_buf_init(&disp_buf1, buf1_1, buf1_2, DISPLAY_BUFFER_LENGTH);

	lvgl::driver::init(80, DISPLAY_BUFFER_LENGTH, sizeof(lv_color_t));

	/* Tick init.
	 * You have to call 'lv_tick_inc()' in periodically to inform LittelvGL about
	 * how much time were elapsed Create an SDL thread to do this*/
	ticker.initializeMs(100, tick).start();

	/*Create a display*/
	static lv_disp_drv_t disp_drv;
	lv_disp_drv_init(&disp_drv); /*Basic initialization*/
	disp_drv.draw_buf = &disp_buf1;
	disp_drv.flush_cb = lvgl::driver::flush;
	disp_drv.hor_res = MONITOR_HOR_RES;
	disp_drv.ver_res = MONITOR_VER_RES;
	disp_drv.antialiasing = 1;

	lv_disp_t* disp = lv_disp_drv_register(&disp_drv);

	lv_theme_t* th = lv_theme_default_init(disp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED),
										   LV_THEME_DEFAULT_DARK, LV_FONT_DEFAULT);
	lv_disp_set_theme(disp, th);

	lv_obj_t* scr = lv_disp_get_scr_act(NULL);

	auto label1 = lv_label_create(scr);
	lv_label_set_text(label1, "DODO");
	lv_obj_set_pos(label1, 30, 30); // position, position);

	lv_group_t* g = lv_group_create();
	lv_group_set_default(g);

	/* Add the mouse as input device
	 * Use the 'mouse' driver which reads the PC's mouse*/
	static lv_indev_drv_t indev_drv_1;
	lv_indev_drv_init(&indev_drv_1); /*Basic initialization*/
	indev_drv_1.type = LV_INDEV_TYPE_POINTER;

	/*This function will be called periodically (by the library) to get the mouse position and state*/
	indev_drv_1.read_cb = lvgl::driver::read;
	lv_indev_t *mouse_indev = lv_indev_drv_register(&indev_drv_1);

#if 0
	keyboard_init();
	static lv_indev_drv_t indev_drv_2;
	lv_indev_drv_init(&indev_drv_2); /*Basic initialization*/
	indev_drv_2.type = LV_INDEV_TYPE_KEYPAD;
	indev_drv_2.read_cb = keyboard_read;
	lv_indev_t *kb_indev = lv_indev_drv_register(&indev_drv_2);
	lv_indev_set_group(kb_indev, g);
	mousewheel_init();
	static lv_indev_drv_t indev_drv_3;
	lv_indev_drv_init(&indev_drv_3); /*Basic initialization*/
	indev_drv_3.type = LV_INDEV_TYPE_ENCODER;
	indev_drv_3.read_cb = mousewheel_read;

	lv_indev_t *enc_indev = lv_indev_drv_register(&indev_drv_3);
	lv_indev_set_group(enc_indev, g);

	/*Set a cursor for the mouse*/
	LV_IMG_DECLARE(mouse_cursor_icon); /*Declare the image file.*/
	lv_obj_t *cursor_obj = lv_img_create(lv_scr_act()); /*Create an image object for the cursor */
	lv_img_set_src(cursor_obj, &mouse_cursor_icon); /*Set the image source*/
	lv_indev_set_cursor(mouse_indev, cursor_obj); /*Connect the image  object to the driver*/
#endif
}

// Will be called when WiFi station becomes fully operational
void gotIP(IpAddress ip, IpAddress netmask, IpAddress gateway)
{
	initHal();
}

void init()
{
	Serial.begin(SERIAL_BAUD_RATE); // 115200 by default
	Serial.systemDebugOutput(true); // Enable debug output to serial

	lv_init();

	// The web driver requires WIFI to be enabled
	WifiStation.enable(true);
	WifiStation.config(WIFI_SSID, WIFI_PWD);
	WifiAccessPoint.enable(false);

	// Run our method when station was connected to AP
	WifiEvents.onStationGotIP(gotIP);
}
