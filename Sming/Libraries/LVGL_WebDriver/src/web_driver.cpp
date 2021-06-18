#include "include/lv_drivers/web_driver.h"
#include "include/lv_drivers/web_server.h"

namespace lvgl
{
namespace driver
{
constexpr int PIXEL_BUF_HEADER_LEN = 13;

typedef struct {
	uint8_t flag;
	uint16_t x;
	uint16_t y;
} pointer_t;

// Pre-allocated websocket buffer containing pixel data
static uint8_t* buf = nullptr;
static size_t bufLength = 0;

// Pixel depth in bits
static int pixelDepth;

// Current pointer value
static pointer_t pointer;

// List of all connected users
static WebsocketList usersList;

void broadcast(const char* message, size_t length, ws_frame_type_t type)
{
	for(unsigned i = 0; i < usersList.count(); i++) {
		usersList[i]->send(message, length, type);
	}
}

void onConnected(WebsocketConnection& socket)
{
	if(!usersList.contains(&socket)) {
		usersList.addElement(&socket);
	}
	lv_obj_invalidate(lv_disp_get_scr_act(lv_disp_get_default()));
	debug_d("Got new user. Resetting display");
}

void onInput(WebsocketConnection& socket, uint8_t* data, size_t size)
{
	if(size != 5) {
		debug_e("Received invalid input data");
		return;
	}

	pointer.flag = (uint8_t)data[0];
	pointer.x = ((uint8_t)data[1] << 8) | (uint8_t)data[2];
	pointer.y = ((uint8_t)data[3] << 8) | (uint8_t)data[4];
}

void onDisconnected(WebsocketConnection& socket)
{
	usersList.removeElement(&socket);
}

bool init(uint32_t port, uint32_t displayBufferSize, uint8_t colorDepth)
{
	delete buf;
	bufLength = displayBufferSize * colorDepth + PIXEL_BUF_HEADER_LEN;
	buf = new uint8_t[displayBufferSize * colorDepth + PIXEL_BUF_HEADER_LEN];
	if(buf == nullptr) {
		bufLength = 0;
		return false;
	}

	startWebServer(port, onConnected, onInput, onDisconnected);

	// Configure initial state
	pixelDepth = colorDepth;

	pointer.flag = 0;
	pointer.x = 0;
	pointer.y = 0;

	return true;
}

void flush(lv_disp_drv_t* drv, const lv_area_t* area, lv_color_t* color_map)
{
	unsigned i = 0;

	if(usersList.count()) {
		debug_d("Flushing data ...");
		size_t size = lv_area_get_width(area) * lv_area_get_height(area);
		int width = lv_disp_get_hor_res(NULL);
		int height = lv_disp_get_ver_res(NULL);
		int pos = 0;

		// Add a binary message containing the coordinates and 32-bit pixel
		// data.  This must match the javascript unpacking routine in index.html.
		// This way we don't have to worry about endianness.
		//
		// Load the region coordinates
		buf[pos++] = pixelDepth;
		buf[pos++] = (width >> 8) & 0xFF;
		buf[pos++] = width & 0xFF;
		buf[pos++] = (height >> 8) & 0xFF;
		buf[pos++] = height & 0xFF;
		buf[pos++] = (area->x1 >> 8) & 0xFF;
		buf[pos++] = area->x1 & 0xFF;
		buf[pos++] = (area->y1 >> 8) & 0xFF;
		buf[pos++] = area->y1 & 0xFF;
		buf[pos++] = (area->x2 >> 8) & 0xFF;
		buf[pos++] = area->x2 & 0xFF;
		buf[pos++] = (area->y2 >> 8) & 0xFF;
		buf[pos++] = area->y2 & 0xFF;

		if(pixelDepth == 32) {
			// Load the 32-bit pixel data: RGBA8888
			lv_color32_t* p32 = (lv_color32_t*)color_map;
			for(i = 0; i < size; i++) {
				buf[pos++] = p32[i].ch.red;
				buf[pos++] = p32[i].ch.green;
				buf[pos++] = p32[i].ch.blue;
				buf[pos++] = p32[i].ch.alpha;
			}
		} else if(pixelDepth == 16) {
			// Load the 16-bit pixel data: RGB565
			lv_color16_t* p16 = (lv_color16_t*)color_map;
			for(i = 0; i < size; i++) {
				buf[pos++] = p16[i].full >> 8;
				buf[pos++] = p16[i].full & 0xFF;
			}
		} else {
			// Load the 8-bit pixel data: RGB332
			lv_color8_t* p8 = (lv_color8_t*)color_map;
			for(i = 0; i < size; i++) {
				buf[pos++] = p8[i].full;
			}
		}

		// Send the buffer to the web page for display
		broadcast(reinterpret_cast<const char*>(buf), bufLength, WS_FRAME_BINARY);
	}

	lv_disp_flush_ready(drv);
}

void read(lv_indev_drv_t* drv, lv_indev_data_t* data)
{
	data->point.x = (int16_t)pointer.x;
	data->point.y = (int16_t)pointer.y;
	data->state = (pointer.flag == 0) ? LV_INDEV_STATE_REL : LV_INDEV_STATE_PR;
}

void deinit()
{
	delete buf;
	bufLength = 0;
	usersList.clear();
	stopWebServer();
}

} // namespace driver

} // namespace lvgl
