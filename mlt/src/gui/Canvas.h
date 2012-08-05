/**<!-------------------------------------------------------------------->
   @class  Canvas
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Provides a generic Canvas interface, shared between OpenGL canvases 
   and pixel-based canvases
   <!-------------------------------------------------------------------->**/

#ifndef CANVAS_H_
#define CANVAS_H_

#include "Visualization.h"
#include <renderers/Renderer.h>
#include <core/Ray.h>

#define DEFAULT_WIDTH      (480)
#define DEFAULT_HEIGHT     (480)

class QWidget;

namespace milton {

DECLARE_STL_TYPEDEF(std::vector<InteractionListener*>, ListenerList);

class Camera;
class Scene;
class Gui;

class Canvas {
   friend class InteractionListener;
   
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      Canvas(Gui *gui, 
             unsigned width = DEFAULT_WIDTH, 
             unsigned height = DEFAULT_HEIGHT);
      
      virtual ~Canvas();
      
      
      //@}-----------------------------------------------------------------
      ///@name Saving
      //@{-----------------------------------------------------------------
      
      /**
       * Attempts to saver this canvas' image to the specified file
       * @returns whether or not the save was successful
       */
      virtual bool save(const char *fileName) = 0;
      
      
      //@}-----------------------------------------------------------------
      ///@name Interaction/Visualization interface
      //@{-----------------------------------------------------------------
      
      virtual void init();
      
      virtual void registerInteractionListener(InteractionListener *listener);
      virtual void unregisterInteractionListener(InteractionListener *listener);
      
      
      //@}-----------------------------------------------------------------
      ///@name Accessors
      //@{-----------------------------------------------------------------
      
      /**
       * @returns true iff this Canvas supports OpenGL
       * 
       * @note Gui assumes that if a Canvas supports OpenGL, it also 
       *    derives from QGLWidget
       * @see also OpenGLCanvas
       */
      virtual bool supportsOpenGL() const = 0;
      
      /**
       * @returns the QWidget associated with this Canvas
       */
      virtual QWidget *getQWidget() = 0;
      
      inline Gui *getGui() const {
         return m_gui;
      }
      
      inline Renderer *getRenderer() {
         return m_renderer;
      }
      
      inline Camera *getCamera() {
         ASSERT(m_renderer);
         return m_renderer->getCamera();
      }
      
      inline Scene *getScene() {
         ASSERT(m_renderer);
         return m_renderer->getScene();
      }
      
      //@}-----------------------------------------------------------------
      ///@name Mutators
      //@{-----------------------------------------------------------------
      
      virtual void setRenderer(Renderer *renderer);
      
      inline void setGui(Gui *gui) {
         m_gui = gui;
      }
      
      
      //@}-----------------------------------------------------------------
      ///@name Canvas dimensions
      //@{-----------------------------------------------------------------
      
      inline unsigned getWidth() const {
         return m_width;
      }
      
      inline unsigned getHeight() const {
         return m_height;
      }
      
      
      //@}-----------------------------------------------------------------
      
      virtual void redraw() = 0;
      
   protected:
      ///@name Interaction/Visualization interface to children
      //@{-----------------------------------------------------------------
      
      virtual void mousePressed (QMouseEvent *event);
      virtual void mouseReleased(QMouseEvent *event);
      virtual void mouseMoved   (QMouseEvent *event);
       
      virtual void keyPressed (QKeyEvent *event);
      virtual void keyReleased(QKeyEvent *event);
      
      
      //@}-----------------------------------------------------------------
      
   protected:
      Gui         *m_gui;
      Renderer    *m_renderer;
      
      unsigned     m_width;
      unsigned     m_height;
      bool         m_initted;
      
      ListenerList m_interactionListeners;
      
   private:
      void _init();
};

}

#endif // CANVAS_H_

