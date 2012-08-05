/**<!-------------------------------------------------------------------->
   @file   Visualization.h
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Extracts out non-essential functionality (visualizations, user 
   interaction) from Canvases
   <!-------------------------------------------------------------------->**/

#include "Visualization.h"
#include "OpenGLCanvas.h"
#include "ImageCanvas.h"
#include "Gui.h"

#include <milton.h>
#include <QtGui/QtGui>

namespace milton {

InteractionListener::InteractionListener(Canvas *parent) 
   : m_parent(parent)
{
   ASSERT(parent);
   
   // register with parent
   m_parent->registerInteractionListener(this);
}

InteractionListener::~InteractionListener() 
{ }

// Callbacks upon user interaction (empty by default)
void InteractionListener::mousePressEvent  (InteractionInfo &info)
{ }
void InteractionListener::mouseReleaseEvent(InteractionInfo &info)
{ }
void InteractionListener::mouseMoveEvent   (InteractionInfo &info)
{ }

void InteractionListener::keyPressEvent    (InteractionInfo &info)
{ }
void InteractionListener::keyReleaseEvent  (InteractionInfo &info)
{ }

// Called for OpenGL drawing during paintEvent in parent Canvas
// (empty by default)
void InteractionListener::paintGL()
{ }

// Called during paintEvent in parent Canvas (empty by default)
void InteractionListener::paint(QPainter *e)
{ }

 
CameraUIListener::CameraUIListener(OpenGLCanvas *parent)
   : InteractionListener(parent), m_visualizeAccel(false)
{ }

CameraUIListener::~CameraUIListener()
{ }

// Callbacks upon user interaction
void CameraUIListener::mousePressEvent  (InteractionInfo &info) {
   QMouseEvent *event = static_cast<QMouseEvent *>(info.event);
   
   m_lastMousePos = m_mouseDownPos = Point2(event->x(), event->y());
   
   ThinLensCamera *camera = dynamic_cast<ThinLensCamera *>(m_parent->getCamera());
   if (camera != NULL) {
      m_origEye  = camera->getEye();
      m_origUp   = camera->getUp();
      m_origLook = camera->getLook();
      m_origU    = camera->getU();
   }
}

void CameraUIListener::mouseReleaseEvent(InteractionInfo &info) 
{ }

void CameraUIListener::mouseMoveEvent   (InteractionInfo &info) {
   QMouseEvent *event = static_cast<QMouseEvent *>(info.event);
   
   if (event->modifiers() & Qt::ControlModifier) {
      Qt::MouseButtons buttons = event->buttons();
      const Vector2 &diff = Vector2(0,0);
      //Point2(event->x(), event->y()) - m_lastMousePos;
      
      ThinLensCamera *camera = dynamic_cast<ThinLensCamera *>(m_parent->getCamera());
      if (camera != NULL) {
         if (buttons & Qt::LeftButton) {
            // Camera trackball rotation
            // --------------------------------------------
            const Vector2 &diff = 
               Point2(event->x(), event->y()) - m_mouseDownPos;
            Point3 origin;
            
            const real_t scaleFactor   = -0.015f;
            const Matrix4x4 &leftRight = getRotMat(origin, m_origUp, 
                                                   scaleFactor * diff[0]);
            const Matrix4x4 &upDown    = getRotMat(origin, m_origU, 
                                                   scaleFactor * diff[1]);
            
            // Calculate new camera parameters
            const Point3 &eyeNew   = upDown * leftRight * m_origEye;
            const Vector3 &upNew   = upDown * leftRight * m_origUp;
            const Vector3 &lookNew = upDown * leftRight * m_origLook;
            
            camera->setOrientation(eyeNew, lookNew, upNew);
         } else if (buttons & Qt::RightButton) {
            // Camera dolly (translation along look vector)
            // --------------------------------------------
            const real_t   amount = diff[0] / 200.0;
            const Vector3 &offset = camera->getLook() * amount;
            
            const Point3 &eyeNew  = camera->getEye() + offset;
            camera->setOrientation(eyeNew, camera->getLook(), camera->getUp());
         } else if (buttons & Qt::MidButton) {
            // Camera pan (translation along uv film plane)
            // --------------------------------------------
            const Point3 &eyeNew  = camera->getEye() + 
               (camera->getU() * (-diff[0] / 64))    + 
               (camera->getV() * (diff[1] / 64));
            
            camera->setOrientation(eyeNew, camera->getLook(), camera->getUp());
         }
         
         m_parent->redraw();
      }
   }
   
   m_lastMousePos = Point2(event->x(), event->y());
}

void CameraUIListener::keyPressEvent    (InteractionInfo &info) {
   QKeyEvent *event = static_cast<QKeyEvent *>(info.event);
   
   m_visualizeAccel = (event->key() == Qt::Key_A);
   
   if (m_visualizeAccel) {
      //Scene *scene = m_parent->getScene();
      //ASSERT(scene);
      
      // TODO
      //scene->
   }
}

void CameraUIListener::keyReleaseEvent  (InteractionInfo &info) {
   QKeyEvent *event = static_cast<QKeyEvent *>(info.event);
   
   if (m_visualizeAccel)
      m_visualizeAccel = !(event->key() == Qt::Key_A);
}

void CameraUIListener::paintGL() {
   if (m_visualizeAccel) {
      
   }
}


IntersectDebugTester::IntersectDebugTester(Canvas *parent)
   : InteractionListener(parent), m_index(0)
{
   ResourceManager::insert<unsigned>("kdTreeIndex",  m_index);
}

IntersectDebugTester::~IntersectDebugTester()
{ }

void IntersectDebugTester::mousePressEvent  (InteractionInfo &info) {
   QMouseEvent *event = static_cast<QMouseEvent *>(info.event);
   
   if (event->modifiers() & Qt::ControlModifier)
      return;
   
   Camera *camera = m_parent->getCamera();
   ASSERT(camera);
   Scene  *scene  = m_parent->getScene();
   ASSERT(scene);
   
   const real_t width = m_parent->getWidth(), height = m_parent->getHeight();
   
   SurfacePoint pt;
   Ray ray(camera->getWorldRay(Point2(event->x() / width, event->y() / height)));
   
   m_index = 0;
   //ResourceManager::insert<bool>("flushPreview", true);
   const real_t t = scene->getIntersection(ray, pt);
   
   cerr << event->x() << ", " << event->y() << ": ";
   if (pt.init(ray, t)) { // intersection found
      cerr << pt << endl;
      
      DirectIllumination *d = m_parent->getRenderer()->getDirectIllumination();
      ASSERT(d);
      
      const SpectralSampleSet &direct = d->evaluate(pt);
      
      cerr << "direct illum = " << direct << endl;
   } else {
      //cerr << "no intersection" << endl;
   }
   
   //m_parent->redraw();
}

void IntersectDebugTester::keyPressEvent(InteractionInfo &info) {
   QKeyEvent *event = static_cast<QKeyEvent *>(info.event);
   
   if (event->modifiers() & Qt::ControlModifier)
      return;
   
   if (event->key() == Qt::Key_Plus)
      ++m_index;
   else if (event->key() == Qt::Key_Minus && m_index > 0)
      --m_index;
   else 
      return;
   
   //cerr << "kdTreeIndex: " << m_index << endl;
   //ResourceManager::insert<unsigned>("kdTreeIndex",  m_index);
   //ResourceManager::insert<bool>("flushPreview", true);
   
   m_parent->redraw();
}

}

