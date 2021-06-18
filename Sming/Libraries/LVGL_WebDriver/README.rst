LittleVGL websocket driver
=========================

.. highlight:: bash

This library provides a driver allowing LittleVGL to use a modern web browser as a display. The code is written for Sming 
and inspired by Dan Julio's <lv_port_esp32_web https://github.com/danjulio/lv_port_esp32_web>_ project.

Usage
-----

Protocol
--------

The websocket payload sent from the driver to the webpage consists of the following fields.

	Byte  0: Pixel Depth (8, 16 or 32)
	Byte  1: Canvas Width[15:8]
	Byte  2: Canvas Width[7:0]
	Byte  3: Canvas Height[15:8]
	Byte  4: Canvas Height[7:0]
	Byte  5: Redraw Region X1[15:8]
	Byte  6: Redraw Region X1[7:0]
	Byte  7: Redraw Region Y1[15:8]
	Byte  8: Redraw Region Y1[7:0]
	Byte  9: Redraw Region X2[15:8]
	Byte 10: Redraw Region X2[7:0]
	Byte 11: Redraw Region Y2[15:8]
	Byte 12: Redraw Region Y2[7:0]
	Byte 13-N: Pixel data (high-byte first for 32- and 16-bit pixels)

The websocket payload sent from the webpage to the driver consists of the following fields.

	Byte 0: Flag (Touch = 1, Release = 0)
	Byte 1: Pointer X[15:8]
	Byte 2: Pointer X[7:0]
	Byte 3: Pointer Y[15:8]
	Byte 4: Pointer Y[7:0]

Color depth
-----------

The driver supports 8-bit, 16-bit, and 32-bit pixels with each increase in pixel depth requiring twice the number pixel data bytes (and corresponding slow-down).  
Pixel depth is configured in the LittleVGL configuration file (`components/lvgl/lvgl.conf`).

The driver should support any resolution (16-bits).  Resolution is also configured in the LittleVGL configuration file.  This project's resolution is 480x320 pixels.

The project uses the dual display buffer technique described in the LittleVGL [Display porting guide](https://docs.littlevgl.com/en/html/porting/display.html).  
This allows it to prepare one buffer while the other is being displayed.  

LittleVGL updates the display in regions that have changed.  The maximum amount of area to be updated at a time is controlled by the `DISP_BUF_SIZE` define in `websocket_driver.h`.  This is very important because it is directly related to a memory buffer that has to exist (I statically allocate this buffer in the driver).  The buffer holds pixels (1, 2 or 4 bytes per pixel).  Too large a value and the ESP32 will crash or the build will fail with a memory-overflow.  The driver currently specifies this as a number of lines.  That means that increasing the display width will increase the memory required.  If things go boom, this is a place to reduce your memory use.

* The driver supports multiple (current maximum = 4) simultaneously connected browsers.  It will update all and take input from all although sending input from more than one browser at a time will currently confuse the driver (and LittleVGL).  The driver forces LittleVGL to invalidate the screen whenever a new session is attached.  This forces it to repaint the entire screen so the new session has a valid starting point.  The maximum number of supported connections is set by a configuration item in the websocket configuration available from menuconfig.  From the main menuconfig screen, select `Component Config` and then select `Websocket Server`.

![menuconfig websocket server max clients](images/menuconfig_3.png)

* You can change the title of the web page, for example to set the name of your program, in the `index.html` file.  You can also change the 16x16 pixel favicon using any number of programs to generate a `favicon.ico` file.

* The way to improve performance is to reduce the amount of redrawing LittleVGL has to do.  8-bit pixels are the fastest but limit you to 256 colors.  16-bit pixels, typically used with low-cost LCD displays, are a bit slower but present a good visual image.  32-bit pixels are possible but you want to eliminate backgrounds and things like animations that cause LittleVGL to have to refresh large portions of the screen at a time.

* Input handling on mobile devices is currently a little wonky, no doubt because of my inexperience with javascript/mobile.  There are conflicts between the browsers page scrolling and the ability to detect movement with touch.  Buttons work ok because touch/release occur but LittleVGL gestures are problematic.  It works sometimes if you hold your finger down for more than 0.3 second before starting the gesture.

