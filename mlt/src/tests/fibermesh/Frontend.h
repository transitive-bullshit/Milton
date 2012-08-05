/**<!-------------------------------------------------------------------->
   @file   Frontend.h
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Spring 2009
   
   @brief
      Frontend for FiberMesh project
   <!-------------------------------------------------------------------->**/

#ifndef FIBERMESH_FRONTEND_H_
#define FIBERMESH_FRONTEND_H_

#include <Visualization.h>
#include <DifferentialGeometry.h>
#include <OpenGLCanvas.h>

class FiberMesh;
class Camera;

enum InteractionMode {
   INTERACTION_MODE_DRAW, 
   INTERACTION_MODE_DRAG, 
};

class Frontend : public InteractionListener {
   public:
      Frontend(OpenGLCanvas *parent);
      
      virtual ~Frontend();
      
      // Callbacks upon user interaction
      virtual void mousePressEvent  (InteractionInfo &info);
      virtual void mouseMoveEvent   (InteractionInfo &info);
      virtual void mouseReleaseEvent(InteractionInfo &info);
      virtual void keyPressEvent    (InteractionInfo &info);
      
      /// Called during OpenGL painting in parent Canvas
      virtual void paintGL();
      
      /// Called non-OpenGL painting in parent Canvas
      virtual void paint(QPainter *p);
      
      /// Called upon parent Canvas initialization
      virtual void init();
      
      inline unsigned getWidth() const {
         return m_parent->getWidth();
      }
      
      inline unsigned getHeight() const {
         return m_parent->getHeight();
      }
      
      inline Camera *getCamera() {
         return m_parent->getCamera();
      }
      
      inline InteractionMode getInteractionMode() {
         return m_mode;
      }
      
   protected:
      virtual void _handleCameraMousePressEvent(QMouseEvent *event);
      virtual void _handleCameraMouseMoveEvent (QMouseEvent *event);
      
   protected:
      Vector2List     m_active;
      FiberMesh      *m_engine;
      InteractionMode m_mode;
      unsigned        m_noIterations;
      unsigned        m_delay;
      
      Point3          m_handle;
      
      // user interaction members
      Point2          m_lastMousePos;
      Point2          m_mouseDownPos;
      
      Point3          m_origin;
      Point3          m_origEye;
      Vector3         m_origUp;
      Vector3         m_origLook;
      Vector3         m_origU;
};

#endif // FIBERMESH_FRONTEND_H_

