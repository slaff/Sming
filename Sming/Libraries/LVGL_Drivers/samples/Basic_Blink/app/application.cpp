#include <SmingCore.h>
#include <lvgl.h>
#include <lv_drivers/display/monitor.h>

#define LED_PIN 2 // GPIO2

Timer ticker;
bool state = true;

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
	constexpr int DISPLAY_BUFFER_LENGTH = MONITOR_HOR_RES * 100;
	static lv_color_t buf1_1[DISPLAY_BUFFER_LENGTH];
	static lv_color_t buf1_2[DISPLAY_BUFFER_LENGTH];
	lv_disp_draw_buf_init(&disp_buf1, buf1_1, buf1_2, DISPLAY_BUFFER_LENGTH);

	monitor_init();

	ticker.initializeMs(100, tick).start();

	/*Create a display*/
	static lv_disp_drv_t disp_drv;
	lv_disp_drv_init(&disp_drv); /*Basic initialization*/
	disp_drv.draw_buf = &disp_buf1;
	disp_drv.flush_cb = monitor_flush;
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

#if 0
	/* Add the mouse as input device
	 * Use the 'mouse' driver which reads the PC's mouse*/
	static lv_indev_drv_t indev_drv_1;
	lv_indev_drv_init(&indev_drv_1); /*Basic initialization*/
	indev_drv_1.type = LV_INDEV_TYPE_POINTER;

	/*This function will be called periodically (by the library) to get the mouse position and state*/
	indev_drv_1.read_cb = mouse_read;
	lv_indev_t* mouse_indev = lv_indev_drv_register(&indev_drv_1);

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

void init()
{
	Serial.begin(SERIAL_BAUD_RATE); // 115200 by default
	Serial.systemDebugOutput(true); // Enable debug output to serial

	lv_init();

	initHal();
}
