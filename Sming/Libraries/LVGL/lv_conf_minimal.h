#pragma once

/* Maximal horizontal and vertical resolution to support by the library.*/
#define LV_HOR_RES_MAX          (240)
#define LV_VER_RES_MAX          (128)
/* Color depth:
 * - 1:  1 byte per pixel
 * - 8:  RGB233
 * - 16: RGB565
 * - 32: ARGB8888
 */
#define LV_COLOR_DEPTH     1
/* Enable anti-aliasing (lines, and radiuses will be smoothed) */
#define LV_ANTIALIAS        0
/*1: Enable the Animations */
#define LV_USE_ANIMATION        0
/* Enable GPU optimization */
#define USE_LV_GPU              0
