#include "include/lv_drivers/web_driver.h"
#include "include/lv_drivers/web_server.h"

#include <Data/Stream/SharedMemoryStream.h>

extern "C" {
#include <heatshrink/heatshrink_encoder.h>
}

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
uint8_t* buf = nullptr;
static size_t bufLength = 0;

// Pixel depth in bits
static int pixelDepth;

// Current pointer value
static pointer_t pointer;

// List of all connected users
static WebsocketList usersList;

static heatshrink_encoder* compressor;

void onConnected(WebsocketConnection& socket)
{
	if(!usersList.contains(&socket)) {
		usersList.addElement(&socket);
	}
	lv_obj_invalidate(lv_disp_get_scr_act(lv_disp_get_default()));
	debug_d("Got new user. Total users: %d", usersList.count());
}

void onInput(WebsocketConnection& socket, uint8_t* data, size_t size)
{
	if(size != 5) {
		debug_e("Received invalid input data");
		return;
	}

	pointer.flag = data[0];
	pointer.x = (data[1] << 8) | data[2];
	pointer.y = (data[3] << 8) | data[4];

	debug_d("%s @ %d x %d", pointer.flag ? _F("pressed") : _F("released"), pointer.x, pointer.y);
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
	if(usersList.count() == 0) {
		lv_disp_flush_ready(drv);
		return;
	}

	size_t size = lv_area_get_width(area) * lv_area_get_height(area);
	int width = lv_disp_get_hor_res(NULL);
	int height = lv_disp_get_ver_res(NULL);

	debug_d("Flushing data. Area from %d,%d to %d,%d ...", area->x1, area->y1, area->x2, area->y2);

	uint32_t pos = 0;
	unsigned i = 0;

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

	size_t dataSize = pos;
	uint8_t *compressedData = new uint8_t[dataSize];
	memset(compressedData, 0, dataSize);

	if(compressor == nullptr) {
		compressor = heatshrink_encoder_alloc(10, 4);
	}
	heatshrink_encoder_reset(compressor);

	uint32_t consumed = 0;
	uint32_t compressedLength = 0;
	while (consumed < pos) {
		size_t count = 0;
		if(heatshrink_encoder_sink(compressor, &buf[consumed], pos - consumed, &count) < 0) {
			debug_e("Unable to compress bytes");
		}
		consumed += count;
		if (consumed == pos) {
			if(heatshrink_encoder_finish(compressor) != HSER_FINISH_MORE) {
				debug_e("Invalid compressor state!");
			}
		}

		HSE_poll_res pres;
		do {
			pres = heatshrink_encoder_poll(compressor, compressedData + compressedLength, dataSize - compressedLength, &count);
			if(pres <  0) {
				debug_e("Invalid compressor pres state!");
			}
			compressedLength += count;
		} while (pres == HSER_POLL_MORE);

		if(pres != HSER_POLL_EMPTY) {
			debug_e("Invalid compressor final pres state!");
		}


		if (compressedLength >= dataSize) {
			debug_e("compression should never expand that much");
			delete compressedData;
			return;
		}

		if (consumed == pos) {
			if(heatshrink_encoder_finish(compressor) != HSER_FINISH_DONE) {
				debug_e("Invalid compressor state!");
			}
		}
	}

	// Send the buffer to the web page for display
	std::shared_ptr<const char> sharedData(reinterpret_cast<char *>(compressedData), [drv](char* p) {
		delete[] p;
	});

	for(i = 0; i < usersList.count(); i++) {
		auto stream = new SharedMemoryStream<const char>(sharedData, compressedLength);
		usersList[i]->send(stream, WS_FRAME_BINARY);
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
	delete compressor;
	bufLength = 0;
	usersList.clear();
	stopWebServer();
}

} // namespace driver

} // namespace lvgl
