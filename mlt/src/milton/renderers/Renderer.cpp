/**<!-------------------------------------------------------------------->
   @file   Renderer.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Abstract rendering engine which attempts to evaluate a rendering 
   equation over a given 2D domain (the film plane) and pipes its output 
   to an equally abstract RenderOutput (generally an image wrapper, but 
   allowing for distributed rendering)
   <!-------------------------------------------------------------------->**/

#include "Renderer.h"

#include <DirectIllumination.h>
#include <Camera.h>
#include <Scene.h>
#include <sstream>

namespace milton {

Renderer::~Renderer() {
   safeDelete(m_scene);
   safeDelete(m_camera);
   safeDelete(m_directIllumination);
}

void Renderer::init() {
   ASSERT(m_scene && m_camera);
   
   if (!m_initted) {
      //cerr << "Renderer: " << *((PropertyMap *)this) << endl;
      m_initted = true;
      
      cout << "initializing renderer" << endl;
      m_scene->init();
      
      Material *m = new Material();
      m->init();
      
      m_scene->getMaterials().push_back(m);
      m_camera->setMaterial(m);
      m_camera->init(m_scene);
      
      // TODO: make this customizable via loader / scenefile
      m_directIllumination = new DirectIllumination(this);
      
      m_directIllumination->inherit(*this);
      m_directIllumination->init();
   } else {
      cerr << "warning: Renderers may only be initialized once" << endl;
   }
}

std::string Renderer::getElapsedTime() const {
   // TODO: bug in boost::timer -- reports ~2.5x time on my system
   
   // 'elapsed' is measured in seconds since the timer was constructed
   const double elapsed = m_timer.elapsed();
   std::stringstream s;
   
   s << ((unsigned long) floor(elapsed)) << " seconds";
   
   if (elapsed >= 60) {
      unsigned minutes = (unsigned) floor(elapsed / 60);
      unsigned seconds = (unsigned) floor(elapsed - (60 * minutes));
      
      if (minutes >= 60) {
         unsigned hours = (minutes / 60);
         minutes       %= 60;
         
         s << " (" << hours << "h:" << minutes << "m:" << seconds << "s)";
      } else {
         s << " (" << minutes << "m:" << seconds << "s)";
      }
   }
   
   return s.str();
}

}

