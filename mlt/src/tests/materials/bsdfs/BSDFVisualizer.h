/**<!-------------------------------------------------------------------->
   @file   BSDFVisualizer.h
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Visualizer for built-in BSDFs, displaying BSDF distributions, probability 
   density functions for sampling BSDFs, and the corresponding distribution of 
   samples taken from BSDFs.  This visualizer has been extremely helpful in 
   debugging BSDF implementations, allowing one to gain some insight visually 
   into what's happening with different BSDFs.
   <!-------------------------------------------------------------------->**/

#ifndef BSDF_VISUALIZER_H_
#define BSDF_VISUALIZER_H_

#include <Visualization.h>

DECLARE_STL_TYPEDEF(std::vector<Vector3 >,     Vector3List);
DECLARE_STL_TYPEDEF(std::vector<Vector3List >, Vector3ListList);

class BSDFVisualizer : public InteractionListener {
   public:
      BSDFVisualizer(OpenGLCanvas *parent);
      
      virtual ~BSDFVisualizer();
      
      // Callbacks upon user interaction
      virtual void mousePressEvent  (InteractionInfo &info);
      virtual void keyPressEvent    (InteractionInfo &info);
      
      /**
       * Called during paintGL in parent Canvas (defaults to empty)
       */
      virtual void render();
      
      virtual void init();
      
   protected:
      void _init();
      void _init(const Ray &r);
      
      void _regenerate();
      void _regenerateSamples();
      void _regenerateDisplay();
      void _sampleBSDF();
      
   protected:
      Material    *m_material;
      SurfacePoint *m_pt;
      Shape       *m_shape;
      
      GLUquadric  *m_quadric;
      Ray          m_ray;
      Vector3List  m_wos;
      
      unsigned     m_index;
      
      Vector3ListList m_vertices; 
      unsigned        m_display;
      std::string     m_bsdf;
};

#endif // BSDF_VISUALIZER_H_

