#pragma once

#include <lvgl/lvgl.h>

namespace lvgl
{
namespace driver
{
/**
 * @brief Initializes the web driver
 * @param port on which a web server will be started
 * @param displayBufferSize
 * @param colorDepth
 */
bool init(uint32_t port = 90, uint32_t displayBufferSize = 1, uint8_t colorDepth = 8);

/**
 * @brief Called from LVGL when data needs to be sent to the display
 * @param drv
 * @param area
 * @param color_map
 */
void flush(lv_disp_drv_t* drv, const lv_area_t* area, lv_color_t* color_map);

/**
 * @brief Called from LVGL to get the input data
 * @param drv
 * @param data
 */
void read(lv_indev_drv_t* drv, lv_indev_data_t* data);

/**
 * @brief Deinitializes the web driver
 */
void deinit();

} // namespace driver

} // namespace lvgl
