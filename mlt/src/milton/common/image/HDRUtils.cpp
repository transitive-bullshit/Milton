/**<!-------------------------------------------------------------------->
   @class  HDRUtils.h
   @author Travis Fischer (fisch0920@gmail.com)
   @date   January 2009
   
   @brief
      Utilities for reading and writing HDR, OpenEXR, and PFM high dynamic 
   range image formats
   
   @note
      This code is based on similar code found in exrtools, by Billy Biggs 
   (http://scanline.ca/exrtools/)
   <!-------------------------------------------------------------------->**/

#include "HDRUtils.h"
#include "HDRImage.h"
#include "RgbaImage.h"

// disable warning to use secure stdio.h functions
#ifdef MILTON_ARCH_WINDOWS
#  pragma warning(disable : 4996)
#endif

// third-party
#include "rgbe.h"

#ifdef HAVE_OPENEXR
#  include "exrinput.h"
#  include <ImfRgbaFile.h>
#endif

using namespace std;

namespace milton {

HDRImage *HDRUtils::loadHDR(const string &filename) {
   FILE *f = fopen(filename.c_str(), "r");
   unsigned width = 0, height = 0;
   
   if (NULL == f)
      return NULL;
   
   if (RGBE_ReadHeader(f, (int*)&width, (int*)&height, NULL) || 
       width == 0 || height == 0)
   {
      return NULL;
   }
   
   HDRImage *image = new HDRImage(width, height);
   float   *buffer = new float[3 * width];
   
   for(unsigned i = 0; i < height; ++i) {
      if (RGBE_ReadPixels_RLE(f, buffer, width, 1)) {
         safeDelete(image);
         safeDeleteArray(buffer);
         return NULL;
      }
      
      for(unsigned j = width; j--;) {
         const float *q = buffer + j * 3;
         
         image->setPixel<RgbaHDR>(i, j, RgbaHDR(q[0], q[1], q[2]));
      }
   }
   
   fclose(f);
   safeDeleteArray(buffer);
   
   return image;
}

HDRImage *HDRUtils::loadEXR(const string &filename) {
#ifndef HAVE_OPENEXR
   HDRUtils::_warnMissingEXR(__FUNCTION__, filename);
   
   return NULL;
#else
   unsigned width, height;
   exrinput_t *exrin;
   
   if (!(exrin = exrinput_new(filename.c_str())))
      return NULL;
   
   width  = exrinput_get_width(exrin);
   height = exrinput_get_height(exrin);
   
   HDRImage *image = new HDRImage(width, height);
   if (!image) {
      exrinput_delete(exrin);
      return NULL;
   }
   
   for(unsigned i = 0; i < height; ++i) {
      double *inscanline = exrinput_get_scanline(exrin, i);
      
      for(unsigned j = width; j--;) {
         const double *q = inscanline + 4 * j;
         
         image->setPixel<RgbaHDR>(i, j, RgbaHDR(q[0], q[1], q[2], q[3]));
      }
   }
   
   exrinput_delete(exrin);
   return image;
#endif
}

HDRImage *HDRUtils::loadPFM(const string &filename) {
   NYI(); // TODO
   
   return NULL;
}

bool HDRUtils::saveHDR(const string &filename, const Image *image) {
   const string &filenameExt = HDRUtils::_toFullName(filename, ".hdr");
   const unsigned width  = image->getWidth();
   const unsigned height = image->getHeight();
   FILE *f = fopen(filenameExt.c_str(), "wb");
   
   if (NULL == f)
      return false;
   
   if (RGBE_WriteHeader(f, width, height, NULL))
      return false;
   
   float *buffer = new float[3 * width];
   
   for(unsigned i = 0; i < height; ++i) {
      for(unsigned j = width; j--;) {
         const RgbaHDR &p = image->getPixel<RgbaHDR>(i, j);
         float *q = buffer + j * 3;
         
         q[0] = static_cast<float>(MAX(create_real(0), p.r));
         q[1] = static_cast<float>(MAX(create_real(0), p.g));
         q[2] = static_cast<float>(MAX(create_real(0), p.b));
      }
      
      if (RGBE_WritePixels_RLE(f, buffer, width, 1)) {
         safeDeleteArray(buffer);
         return false;
      }
   }
   
   fclose(f);
   safeDeleteArray(buffer);
   
   return true;
}

bool HDRUtils::saveEXR(const string &filename, const Image *image) {
#ifndef HAVE_OPENEXR
   HDRUtils::_warnMissingEXR(__FUNCTION__, filename);
   
   return false;
#else
   const string &filenameExt = HDRUtils::_toFullName(filename, ".exr");
   const unsigned width  = image->getWidth();
   const unsigned height = image->getHeight();
   vector<Imf::Rgba> rgba(width * height);
   
   for(unsigned i = height; i--;) {
      for(unsigned j = width; j--;) {
         const RgbaHDR &p = image->getPixel<RgbaHDR>(i, j);
         
         Imf::Rgba &q = rgba[i * width + j];
         q.r = p.r;
         q.g = p.g;
         q.b = p.b;
         q.a = p.a;
      }
   }
   
   Imf::RgbaOutputFile file(filename.c_str(), width, height, Imf::WRITE_RGBA);
   file.setFrameBuffer((const Imf::Rgba*)&rgb[0], 1, width);
   file.writePixels(height);
   
   return true;
#endif
}

bool HDRUtils::savePFM(const string &filename, const Image *image) {
   NYI(); // TODO
   
   return false;
}

HDRImage  *HDRUtils::toHDRImage (const RgbaImage *in) {
   ASSERT(in);
   
   HDRImage *out = new HDRImage(in->getWidth(), in->getHeight());
   out->setData(in->getData());
   
   return out;
}

RgbaImage *HDRUtils::toRgbaImage(const HDRImage *in) {
   ASSERT(in);
   
   RgbaImage *out  = new RgbaImage(in->getWidth(), in->getHeight());
   out->setData(in->getData());
   
   return out;
}

string HDRUtils::_toFullName(const string &filename, 
                             const string &ext)
{
   const string::size_type extPos = filename.find_last_of('.');
   
   if (extPos == string::npos)
      return filename + ext;
   
   return filename;
}

void HDRUtils::_warnMissingEXR(const char *function, 
                               const string &filename)
{
   cerr << (function ? function : "") << ": " 
        << "This build of Milton does not support OpenEXR" 
        << endl;
}

}

