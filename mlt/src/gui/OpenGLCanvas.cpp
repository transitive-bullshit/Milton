/**<!-------------------------------------------------------------------->
   @file   OpenGLCanvas.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Provides an OpenGL rendering view
   <!-------------------------------------------------------------------->**/

#include "OpenGLCanvas.h"
#include "Gui.h"

#include <milton.h>
#include <QtGui/QtGui>

namespace milton {

OpenGLCanvas::OpenGLCanvas(bool defaultInteractionListeners, 
                           Gui *gui, unsigned width, unsigned height)
   : QGLWidget(QGLFormat(QGL::SampleBuffers)), Canvas(gui, width, height), m_drawMode(MODE_SOLID)
{
   _init(defaultInteractionListeners);
}

OpenGLCanvas::OpenGLCanvas(Gui *gui, unsigned width, unsigned height)
   : QGLWidget(QGLFormat(QGL::SampleBuffers)), Canvas(gui, width, height), m_drawMode(MODE_SOLID)
{
   _init(true);
}

OpenGLCanvas::~OpenGLCanvas()
{ }

void OpenGLCanvas::init() {
   if (!m_initted) {
      if (NULL == m_renderer) {
         Scene *scene = new Scene(MaterialList(), new ShapeSet(false));
         m_renderer   = new OpenGLRenderer(new ThinLensCamera(), scene);
      }
      
      Canvas::init();
   }
}

bool OpenGLCanvas::save(const char *fileName) {
   // technical Qt note:
   //    renderPixmap creates a new, temporary GL context and renders this 
   // OpenGLCanvas in that new context. this 'flushPreview' flag denotes to 
   // Milton shapes / objects that prior display lists, textures, etc. have 
   // been invalidated (because they don't cross over between GL contexts), 
   // and therefore need to be recreated before using them; otherwise, 
   // attempting to use a previously-compiled display list, for example, 
   // would silently fail within renderPixmap.
   ResourceManager::insert<bool>("flushPreview", true);
   const QPixmap &temp = renderPixmap();
   ResourceManager::insert<bool>("flushPreview", false);
   
   return temp.save(QString(fileName), NULL, -1);
}


void OpenGLCanvas::lightOn(int light) {
   ASSERT(light >= 0 && light < 8);
   glEnable(GL_LIGHT0 + light);
}

void OpenGLCanvas::lightOff(int light) {
   ASSERT(light >= 0 && light < 8);
   glDisable(GL_LIGHT0 + light);
}

void OpenGLCanvas::lightPos(int light, float x, float y, float z) {
   ASSERT(light >= 0 && light < 8);
   GLfloat pos[4] = {x, y, z, 1};
   
   glPushMatrix();
   glLoadIdentity();
   glLightfv(GL_LIGHT0+light, GL_POSITION, pos);
   glPopMatrix();
}

void OpenGLCanvas::lightColor(int light, float r, float g, float b) {
   ASSERT(light >= 0 && light < 8);
   GLfloat color[4] = { r, g, b, 1};
   
   glLightfv(GL_LIGHT0+light, GL_DIFFUSE, color);
   glLightfv(GL_LIGHT0+light, GL_SPECULAR, color);
}

void OpenGLCanvas::lightFunc(int light, float a, float b, float c) {
   ASSERT(light >= 0 && light < 8);
   
   glLightf(GL_LIGHT0 + light, GL_CONSTANT_ATTENUATION, a);
   glLightf(GL_LIGHT0 + light, GL_LINEAR_ATTENUATION, b);
   glLightf(GL_LIGHT0 + light, GL_QUADRATIC_ATTENUATION, c);
}


bool OpenGLCanvas::supportsOpenGL() const {
   return true;
}


void OpenGLCanvas::mousePressEvent(QMouseEvent *e) {
   mousePressed(e);
}

void OpenGLCanvas::mouseReleaseEvent(QMouseEvent *e) {
   mouseReleased(e);
}

void OpenGLCanvas::mouseMoveEvent(QMouseEvent *e) {
   mouseMoved(e);
}

void OpenGLCanvas::keyPressEvent(QKeyEvent *e) {
   keyPressed(e);
}

void OpenGLCanvas::keyReleaseEvent(QKeyEvent *e) {
   keyReleased(e);
}


QSize OpenGLCanvas::sizeHint() const {
   return QSize(m_width, m_height);
}

QSize OpenGLCanvas::minimumSize() const {
   return sizeHint();
}

QSize OpenGLCanvas::maximumSize() const {
   return sizeHint();
}


void OpenGLCanvas::initializeGL() {
   qglClearColor(Qt::black);
   
   glShadeModel(GL_SMOOTH);
   glDisable(GL_DITHER);
   glDisable(GL_NORMALIZE);
   glEnable(GL_DEPTH_TEST);
   
   glEnable(GL_LIGHTING);
   for(int i = 8; i--;) {
      glDisable(GL_LIGHT0+i);
      lightFunc(i, 0, .5, .2);
   }
   
   glColorMaterial(GL_FRONT, GL_DIFFUSE);
   glEnable(GL_CULL_FACE);
   
#ifdef GL_VERSION_1_3  // multisampling was added in OpenGL 1.3
   glEnable(GL_MULTISAMPLE);
#endif
   
   setDrawMode(m_drawMode);
}

void OpenGLCanvas::redraw() {
   update();
}

void OpenGLCanvas::paintEvent(QPaintEvent *e) {
   int err;
   if ((err = glGetError()) != GL_NO_ERROR) {
      cerr << "GL is in an error state before call to paintGL()" << endl;
      cerr << "(Error: " << gluErrorString(err) << ")\n";
      assert(0);
   }
   
   glMatrixMode(GL_MODELVIEW);
   glPushMatrix();
   
   { // reset lighting
      glEnable(GL_LIGHTING);
      for(int i = 8; i--;) {
         glDisable(GL_LIGHT0 + i);
         lightFunc(i, .5, .2, 0);
      }
      
      glColorMaterial(GL_FRONT, GL_DIFFUSE);
      glEnable(GL_COLOR_MATERIAL);
      
      if (ResourceManager::getValue<bool>("enableDefaultLighting", true)) {
         const real_t p = 4.5;
         unsigned index = 0;
         
         for(int dim = 3; dim--;) {
            real_t pos[3] = { 1, 1, 1 };
            
            for(real_t sign = -1; sign <= 1; sign += 2) {
               pos[dim] = sign * p;
               pos[(dim + 1) % 3] *= (dim + 1);
               pos[(dim + 2) % 3] *= (dim + 1);
               
               lightOn(index);
               lightPos(index, pos[0], pos[1], pos[2]);
               lightFunc(index, .5, (2-sign)/4.0, 0);
               
               const real_t power = 1;
               lightColor(index, power, power, power);
               
               ++index;
            }
         }
         
         lightOn(index);
         lightPos(index, 2, 2, 2);
         lightFunc(index, 1, .3, 0);
         lightColor(index, 1, 1, 1);
         ++index;
         
         // attempt to place a light at the center of the scene's bounding box
         if (m_renderer) {
            Scene *scene = m_renderer->getScene();
            
            if (scene) {
               ShapeSet *shapes = scene->getShapes();
               
               if (shapes) {
                  const Point3 &p = shapes->getAABB().getCenter();
                  
                  lightOn(index);
                  lightPos(index, p[0], p[1], p[2]);
                  lightFunc(index, .5, 0, 0);
                  lightColor(index, 1, 1, 1);
               }
            }
         }
      }
   }
   
   int oldDrawMode = m_drawMode;
   if (m_renderer && m_renderer->supportsOpenGL())
      m_renderer->render();
   
   /* Notify all listeners of the paintGL event */
   ListenerListIter iter;
   
   for(iter = m_interactionListeners.begin(); iter != m_interactionListeners.end(); iter++) {
      (*iter)->paintGL();
   }
   
   glMatrixMode(GL_MODELVIEW);
   glPopMatrix();
   
   setDrawMode(oldDrawMode);
   
   /* Notify all listeners of the paint event */
   QPainter painter(this);
   painter.setRenderHint(QPainter::Antialiasing);
   
   for(iter = m_interactionListeners.begin(); iter != m_interactionListeners.end(); iter++) {
      (*iter)->paint(&painter);
   }
   
   if ((err = glGetError()) != GL_NO_ERROR) {
      cerr << "GL is in an error state after call to paintGL()" << endl;
      cerr << "(Error: " << gluErrorString(err) << ")\n";
      assert(0);
   }
}

void OpenGLCanvas::resizeGL(int width, int height) {
   glViewport(0, 0, width, height);
}

void OpenGLCanvas::setDrawMode(int style) {
   makeCurrent();
   
   if (style == MODE_WIREFRAME)
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
   else // MODE_SOLID
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
   
   m_drawMode = style;
}


void OpenGLCanvas::_init(bool defaultInteractionListeners) {
   char *dummy_argv = "dummy";
   int dummy_argc   = 1;
   glutInit(&dummy_argc, &dummy_argv);
   
   _setupQWidget();
   
   if (defaultInteractionListeners) {
      registerInteractionListener(new CameraUIListener(this));
   }
}

void OpenGLCanvas::_setupQWidget() {
   // minor optimization: tell Qt that we'll be painting our own background
   setAutoFillBackground(false);
   setAttribute(Qt::WA_NoSystemBackground);
   setFocusPolicy(Qt::StrongFocus);
   
   if (layout() != NULL)
      layout()->setSizeConstraint(QLayout::SetFixedSize);
   
   setMouseTracking(true);
   setFixedSize(m_width, m_height);
   
   // display a message if OpenGL is not supported
   // (over remote login, for example)
   if (!QGLFormat::hasOpenGL()) {
      dprintf("\nERROR: OpenGL not supported on this device!\n"
              "\t(OpenGLCanvas is invalid)\n\n");
   }
}

}

