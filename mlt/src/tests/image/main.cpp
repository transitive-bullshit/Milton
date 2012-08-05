/**<!-------------------------------------------------------------------->
   @file   main.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   <!-------------------------------------------------------------------->**/

#include <MiltonApp.h>
#include <milton.h>
//#include <third-party/median/ctmf.h>
#include "ctmf.h"
using namespace std;

void printUsage(char **argv) {
   cerr << "usage: " << argv[0] << " <path-to-image>" << endl;
}

int main(int argc, char **argv) {
   if (argc < 2) {
      printUsage(argv);
      return 1;
   }
   
   MiltonApp app(argc, argv, "Image Visualizer");
   Image *image = Image::load(argv[1]);
   if (NULL == image) {
      cerr << "image failed to load!" << endl;
      printUsage(argv);
      return 1;
   }
   
   RgbaImage *rgbaImage = NULL;
   
   if (image->isHDR())
      rgbaImage = HDRUtils::toRgbaImage((HDRImage *)image);
   else 
      rgbaImage = (RgbaImage *)image;
   
   // median-filter luminance
   
   Rgba32 *inData = rgbaImage->getData();
   for(unsigned i = image->getSize(); i--;) {
      Rgba32 &p0 = inData[i];
      
      RgbaHDR o;
      RGBtoXYZ(p0.r, p0.g, p0.b, o.r, o.g, o.b);
      
      p0.r = (unsigned char) CLAMP(o.r, 0, 255);
      p0.g = (unsigned char) CLAMP(o.g, 0, 255);
      p0.b = (unsigned char) CLAMP(o.b, 0, 255);
      
      //const RgbaHDR temp = p;
      //p.r = (unsigned char)(((real_t)temp.r) / (temp.r + temp.g + temp.b));
      //p.b = (unsigned char)(((real_t)temp.g) / (temp.r + temp.g + temp.b));
   }
   
   RgbaImage *filtered = new RgbaImage(*rgbaImage);
   Rgba32 *outData = filtered->getData();
   ctmf((unsigned char*)(&inData[0].g), (unsigned char*)(&outData[0].g), 
        image->getWidth(), image->getHeight(), 
        sizeof(Rgba32) * image->getWidth(), sizeof(Rgba32) * image->getWidth(), 
        2, 4, 512 * 1024);
   
   for(unsigned i = image->getSize(); i--;) {
      Rgba32 &p0 = inData[i];
      Rgba32 &p = outData[i];
      
      RgbaHDR temp;
      temp.r = p0.r * (((real_t)p.g) / p0.g);
      temp.g = p.g;
      temp.b = p0.b * (((real_t)p.g) / p0.g);
      //p.r = ((unsigned char)CLAMP((p.r * (real_t)p.g) / p0.g, 0, 255));
      //p.b = ((unsigned char)CLAMP((p.b * (real_t)p.g) / p0.g, 0, 255));
      
      XYZtoRGB(temp.r, temp.g, temp.b, temp.r, temp.g, temp.b);
      p.r = (unsigned char) CLAMP(temp.r, 0, 255);
      p.g = (unsigned char) CLAMP(temp.g, 0, 255);
      p.b = (unsigned char) CLAMP(temp.b, 0, 255);
   }
   
   // median filter r, g, and b channels
   /*for(unsigned i = 3; i--;) {
      ctmf(((unsigned char*)&rgbaImage->getData()[0].data[i]), ((unsigned char*)&filtered->getData()[0][i]), 
           image->getWidth(), image->getHeight(), 
           sizeof(Rgba32) * image->getWidth(), sizeof(Rgba32) * image->getWidth(), 
           1, 4, 512 * 1024);
   }*/
   
   filtered->save("out.png");
   //rgbaImage->save("out.png");
   
   ImageCanvas *canvas = 
      new ImageCanvas(app.getGui(), image->getWidth(), image->getHeight());
   
   canvas->setData(filtered->getData());
   //canvas->setData(rgbaImage->getData());
   app.addCanvas(canvas);
   
   return app.exec();
}

