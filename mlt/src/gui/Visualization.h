/**<!-------------------------------------------------------------------->
   @file   Visualization.h
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Extracts out non-essential functionality (visualizations, user 
   interaction) from Canvases
   <!-------------------------------------------------------------------->**/

#ifndef VISUALIZATION_H_
#define VISUALIZATION_H_

#include <shapes/Triangle.h>
#include <core/Ray.h>

#include <QtGui/QtGui>
#include <QtOpenGL/QtOpenGL>
#include <GL/glut.h>

class QInputEvent;

namespace milton {

class OpenGLCanvas;
class ImageCanvas;
class Canvas;

struct InteractionInfo {
   QInputEvent *event;
   
   // InteractionListeners should set this to false if they don't 
   // want other InteractionListeners to also be notified of the event
   bool propogateEvent;
   
   // InteractionListeners can use this to communicate with each other
   int  data;
   
   InteractionInfo(QInputEvent *e = NULL) 
      : event(e), propogateEvent(true), data(0)
   { }
};

/**
 * @brief
 *    Extracts out non-essential functionality (visualizations, user 
 * interaction, etc.) from Canvases
 */
class InteractionListener {
   public:
      /**
       * Registers this listener with the parent Canvas
       */
      InteractionListener(Canvas *parent);
      
      virtual ~InteractionListener();
      
      inline Canvas *getParent() const {
         return m_parent;
      }
      
      /// initializes this InteractionListener after parent has been initialized
      /// @note: default implementation is empty
      virtual void init()
      { }
      
      // Callbacks upon user interaction (empty by default)
      virtual void mousePressEvent  (InteractionInfo &info);
      virtual void mouseReleaseEvent(InteractionInfo &info);
      virtual void mouseMoveEvent   (InteractionInfo &info);
      
      virtual void keyPressEvent    (InteractionInfo &info);
      virtual void keyReleaseEvent  (InteractionInfo &info);
      
      /// Called for OpenGL drawing during paintEvent in parent Canvas
      /// (empty by default)
      virtual void paintGL();
      
      /// Called during paintEvent in parent Canvas (empty by default)
      virtual void paint(QPainter *e);
      
   protected:
      Canvas *m_parent;
};

/**
 * @brief
 *    Provides basic Maya-inspired camera UI modifiers for trackball rotation, 
 * dollying (translation along look vector), and panning (translation along 
 * uv film plane)
 */
class CameraUIListener : public InteractionListener {
   public:
      CameraUIListener(OpenGLCanvas *parent);
      
      virtual ~CameraUIListener();
      
      // Callbacks upon user interaction
      virtual void mousePressEvent  (InteractionInfo &info);
      virtual void mouseReleaseEvent(InteractionInfo &info);
      virtual void mouseMoveEvent   (InteractionInfo &info);
      
      virtual void keyPressEvent    (InteractionInfo &info);
      virtual void keyReleaseEvent  (InteractionInfo &info);
      
      virtual void paintGL();
      
   protected:
      Point2  m_lastMousePos;
      Point2  m_mouseDownPos;
      
      Point3  m_origEye;
      Vector3 m_origUp;
      Vector3 m_origLook;
      Vector3 m_origU;
      
      bool    m_visualizeAccel;
};

/**
 * @brief
 *    Debugging info when a pixel is clicked on an ImageCanvas
 */
class IntersectDebugTester : public InteractionListener {
   public:
      IntersectDebugTester(Canvas *parent);
      
      virtual ~IntersectDebugTester();
      
      // Callbacks upon user interaction
      virtual void mousePressEvent  (InteractionInfo &info);
      virtual void keyPressEvent    (InteractionInfo &info);
      
   protected:
      unsigned m_index;
};

}

#endif // VISUALIZATION_H_

