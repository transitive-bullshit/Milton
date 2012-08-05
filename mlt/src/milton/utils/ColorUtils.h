#pragma once

#include <common/common.h>

namespace milton {

/* Utilities to convert from one color space to another */
MILTON_DLL_EXPORT void RGBtoHSV(real_t r, real_t g, real_t b, real_t &h, real_t &s, real_t &v);
MILTON_DLL_EXPORT void HSVtoRGB(real_t h, real_t s, real_t v, real_t &r, real_t &g, real_t &b);
MILTON_DLL_EXPORT void RGBtoYIQ(real_t r, real_t g, real_t b, real_t &y, real_t &i, real_t &q);
MILTON_DLL_EXPORT void YIQtoRGB(real_t y, real_t i, real_t q, real_t &r, real_t &g, real_t &b);
MILTON_DLL_EXPORT void RGBtoXYZ(real_t r, real_t g, real_t b, real_t &x, real_t &y, real_t &z);
MILTON_DLL_EXPORT void XYZtoRGB(real_t x, real_t y, real_t z, real_t &r, real_t &g, real_t &b);

MILTON_DLL_EXPORT void XYZtoxyY(real_t x, real_t y, real_t z, real_t &xP, real_t &yP, real_t &Y);
MILTON_DLL_EXPORT void xyYtoXYZ(real_t xP, real_t yP, real_t Y, real_t &x, real_t &y, real_t &z);

}

