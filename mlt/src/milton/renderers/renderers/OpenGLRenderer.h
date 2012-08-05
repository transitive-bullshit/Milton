/**<!-------------------------------------------------------------------->
   @class  OpenGLRenderer
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      OpenGL preview renderer
   <!-------------------------------------------------------------------->**/

#ifndef OPENGL_RENDERER_H_
#define OPENGL_RENDERER_H_

#include <renderers/Renderer.h>

namespace milton {

class MILTON_DLL_EXPORT OpenGLRenderer : public Renderer {
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      inline OpenGLRenderer(Camera *camera = NULL, Scene *scene = NULL)
         : Renderer(camera, scene)
      { }
      
      virtual ~OpenGLRenderer()
      { }
      
      
      //@}-----------------------------------------------------------------
      ///@name Main usage interface
      //@{-----------------------------------------------------------------
      
      /**
       * Renders an OpenGL preview of the underlying scene
       * 
       * @note assumes caller is within a valid OpenGL drawing context
       */
      virtual void render();
      
      
      //@}-----------------------------------------------------------------
      ///@name Accessors / Mutators
      //@{-----------------------------------------------------------------
      
      virtual bool supportsOpenGL() {
         return true;
      }
      
      
      //@}-----------------------------------------------------------------
};

}

#endif // OPENGL_RENDERER_H_

