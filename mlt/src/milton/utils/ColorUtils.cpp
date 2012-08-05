#include "ColorUtils.h"
#include <common.h>
#include <math.h>

namespace milton {

static void Matrix3x3Multiply(const real_t matrix[9], 
                              real_t a, real_t b, real_t c, 
                              real_t &x, real_t &y, real_t &z)
{
   x = matrix[0]*a + matrix[1]*b + matrix[2]*c;	
   y = matrix[3]*a + matrix[4]*b + matrix[5]*c;	
   z = matrix[6]*a + matrix[7]*b + matrix[8]*c;	
}

void RGBtoHSV(real_t r, real_t g, real_t b, real_t &h, real_t &s, real_t &v) {
   real_t min, max, delta;

   min = MIN3(r, g, b);
   max = MAX3(r, g, b);
   v = max;

   delta = max - min;
   if (max != 0) {
      s = delta / max;
   }
   else {
      s = 0;
      h = -1;
      return;
   }

   if (r == max) {
      h = (g - b) / delta;
   }
   else if (g == max) {
      h = 2 + (b-r) / delta;
   }
   else {
      h = 4 + (r-g) / delta;
   }

   h *= 60;
   if (h < 0)
      h += 360;
}

void HSVtoRGB(real_t h, real_t s, real_t v, real_t &r, real_t &g, real_t &b) {
   int i;
   real_t f, p, q, t;
   if (s == 0) {
      r = g = b = v;
      return;
   }

   h /= 60;
   i = (int)floor(h);
   f = h - i;
   p = v * (1 - s);
   q = v * (1 - s*f);
   t = v * (1 - s*(1-f));

   switch (i) {
      case 0:
         r = v;
         g = t;
         b = p;
         break;
      case 1:
         r = q;
         g = v;
         b = p;
         break;
      case 2:
         r = p;
         g = v;
         b = t;
         break;
      case 3:
         r = p;
         g = p;
         b = v;
      case 4:
         r = t;
         g = p;
         b = v;
      default:
         r = v;
         g = p;
         b = q;
         break;
   }
}

void RGBtoYIQ(real_t r, real_t g, real_t b, real_t &y, real_t &i, real_t &q) {
   static const real_t matrix[] = {
      0.299,  0.587,  0.114,
      0.596, -0.275, -0.321,
      0.212, -0.523,  0.311
   };

   Matrix3x3Multiply(matrix, r, g, b, y, i, q);
}

void YIQtoRGB(real_t y, real_t i, real_t q, real_t &r, real_t &g, real_t &b) {
   static const real_t matrix[] = {
      1.0,  0.956,  0.621,
      1.0, -0.272, -0.647,
      1.0, -1.105,  1.702
   };

   Matrix3x3Multiply(matrix, y, i, q, r, g, b);
}

void RGBtoXYZ(real_t r, real_t g, real_t b, real_t &x, real_t &y, real_t &z) {
   static const real_t matrix[] = {
      0.41245322014416, 0.35757958129350, 0.18042258997054, 
      0.21267112134122, 0.71515920531078, 0.07216877677613, 
      0.01933381646199, 0.11919354020663, 0.95022692228971
   };
   
   Matrix3x3Multiply(matrix, r, g, b, x, y, z);
}

void XYZtoRGB(real_t x, real_t y, real_t z, real_t &r, real_t &g, real_t &b) {
   static const real_t matrix[] = {
      3.240479, -1.537150, -0.498535, 
      -0.969256, 1.875992, 0.041556, 
      0.055648, -0.204043, 1.057311, 
   };
   
   Matrix3x3Multiply(matrix, x, y, z, r, g, b);
}

void XYZtoxyY(real_t x, real_t y, real_t z, real_t &xP, real_t &yP, real_t &Y) {
   real_t inv_norm = (x + y + z);
   if (inv_norm != 0)
      inv_norm = 1.0 / inv_norm;
   
   xP = x * inv_norm;
   yP = y * inv_norm;
   Y  = y;
}

void xyYtoXYZ(real_t xP, real_t yP, real_t Y, real_t &x, real_t &y, real_t &z) {
   const real_t inv_y = (y == 0 ? 0 : 1.0 / y);
   
   x = Y * x * inv_y;
   y = Y;
   z = Y * (1 - x - y) * inv_y;
}

}

