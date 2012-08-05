/**<!-------------------------------------------------------------------->
   @class  OpenGLCanvas
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Provides an OpenGL rendering view
   <!-------------------------------------------------------------------->**/

#ifndef OPENGL_CANVAS_H_
#define OPENGL_CANVAS_H_

#include <QtOpenGL/QGLWidget>
#include <QtOpenGL/QtOpenGL>
#include <GL/glut.h>

#include "Canvas.h"

namespace milton {

// setDrawMode constants
enum DRAW_MODE {
   MODE_WIREFRAME = 0, 
   MODE_SOLID = 1
};

class OpenGLCanvas : public QGLWidget, public Canvas {
   
   Q_OBJECT
   
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      /**
       * @param defaultInteractionListeners if true will enable the basic, 
       *        built-in, Maya-style user interactions via CTRL + mouse
       */
      OpenGLCanvas(bool defaultInteractionListeners, 
                   Gui *gui = NULL, 
                   unsigned width  = DEFAULT_WIDTH, 
                   unsigned height = DEFAULT_HEIGHT);
      
      /**
       * @brief
       *    Constructs an OpenGLCanvas with default interaction listeners 
       * (Maya-style camera interactions)
       */
      OpenGLCanvas(Gui *gui = NULL, 
                   unsigned width  = DEFAULT_WIDTH, 
                   unsigned height = DEFAULT_HEIGHT);
      
      virtual ~OpenGLCanvas();
      
      //@}-----------------------------------------------------------------
      ///@name Saving
      //@{-----------------------------------------------------------------
      
      /**
       * Attempts to saver this canvas' image to the specified file
       * @returns whether or not the save was successful
       */
      bool save(const char *fileName);
      
      //@}-----------------------------------------------------------------
      ///@name OpenGL light utility methods (lights specified by integer id)
      //@{-----------------------------------------------------------------
      
      /// Turns the light on
      void lightOn(int light);
      /// Turns the light off
      void lightOff(int light);
      /// Specifies the position of the light
      /// @note Assumes current matrixMode is GL_MODELVIEW
      void lightPos(int light, float x, float y, float z);
      /// Specifies the color of the light
      void lightColor(int light, float r, float g, float b);
      /// Specifies the function of the light
      void lightFunc(int light, float a, float b, float c);
      
      //@}-----------------------------------------------------------------
      
      virtual void init();
      
      virtual bool supportsOpenGL() const;
      
      virtual QWidget *getQWidget() {
         return this;
      }
      
   public slots:
      ///@name Main usage interface
      //@{-----------------------------------------------------------------
      
      void setDrawMode(int style);
      virtual void redraw();
      
      //@}-----------------------------------------------------------------
      
   protected:
      ///@name Overridden from QWidget
      //@{-----------------------------------------------------------------
      
      virtual void mousePressEvent(QMouseEvent *);
      virtual void mouseReleaseEvent(QMouseEvent *);
      virtual void mouseMoveEvent(QMouseEvent *);
      
      virtual void keyPressEvent(QKeyEvent *event);
      virtual void keyReleaseEvent(QKeyEvent *event);
      
      virtual QSize sizeHint() const;
      virtual QSize minimumSize() const;
      virtual QSize maximumSize() const;
      
      virtual void initializeGL();
      virtual void paintEvent(QPaintEvent *e);
      void resizeGL(int width, int height);
      
      //@}-----------------------------------------------------------------
      
   protected:
      int m_drawMode;
      
   private:
      void _init(bool defaultInteractionListeners);
      void _setupQWidget();
};

}

#endif // OPENGL_CANVAS_H_

