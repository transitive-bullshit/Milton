/**<!-------------------------------------------------------------------->
   @file   PathVisualizer.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Visualizer for randomly-generated bidirectional paths
   <!-------------------------------------------------------------------->**/

#include "PathVisualizer.h"
#include <OpenGLCanvas.h>
#include <ImageCanvas.h>
#include <Gui.h>
#include <milton.h>

using namespace milton;

PathVisualizer::PathVisualizer(OpenGLCanvas *parent)
   : InteractionListener(parent), m_light(NULL), m_eye(NULL), m_generator(NULL)
{ }

PathVisualizer::~PathVisualizer() {
   safeDelete(m_generator);
   safeDelete(m_light);
   safeDelete(m_eye);
}

void PathVisualizer::init() {
   InteractionListener::init();
   
   cout << endl;
   cout << "directions:" << endl;
   cout << "\t'shift+click' creates a new eye path starting at the clicked pixel" << endl;
   cout << "\t'space' creates a new path from scratch" << endl;
   cout << endl;
   
   m_generator = new BidirectionalPathTracer(new DefaultRenderOutput(), m_parent->getCamera(), 
                                             m_parent->getScene());
   
   _reset();
}

void PathVisualizer::mousePressEvent  (InteractionInfo &info) {
   QMouseEvent *event = static_cast<QMouseEvent *>(info.event);
   
   if (NULL == m_light || NULL == m_eye)
      return;
   
   const Point2 &pt = Point2(event->x(), event->y());
   if (event->modifiers() & Qt::ShiftModifier) {
      SurfacePoint *p = new SurfacePoint();
      UV uv(pt[0] / m_parent->getWidth(), pt[1] / m_parent->getHeight());
      
      m_parent->getCamera()->getPoint(*p, uv);
      
      m_eye->clear();
      m_eye->prepend(PathVertex(p, 1, 1));
      _initE();
      
      m_parent->redraw();
   }
}

void PathVisualizer::keyPressEvent(InteractionInfo &info) {
   QKeyEvent *event = static_cast<QKeyEvent *>(info.event);
   
   if (event->key() == Qt::Key_Space)
      _reset();
}

void PathVisualizer::paintGL() {
   if (NULL == m_light || NULL == m_eye)
      return;
   
   glDisable(GL_LIGHTING);
   
   glBegin(GL_LINES);
   {
      if (m_eye->length() > 0) {
         glColor3f(1, 0, 0);
         for(unsigned i = m_eye->length() - 1; i--;) {
            glVertex3real_tv((*m_eye)[i + 1].pt->position.data);
            glVertex3real_tv((*m_eye)[i].pt->position.data);
         }
      }
      
      if (m_eye->length() > 0 && m_light->length() > 0) {
         glColor3f(1, 1, 1);
         glVertex3real_tv(m_eye->front().pt->position.data);
         glVertex3real_tv(m_light->back().pt->position.data);
      }
      
      if (m_light->length() > 0) {
         glColor3f(0, 1, 0);
         for(unsigned i = m_light->length() - 1; i--;) {
            glVertex3real_tv((*m_light)[i + 1].pt->position.data);
            glVertex3real_tv((*m_light)[i].pt->position.data);
         }
      }
   }
   glEnd();
}

void PathVisualizer::_init() {
   _initE();
   _initL();
   m_parent->redraw();
}

void PathVisualizer::_initL() {
   if (m_eye->front().pt->emitter->isEmitter()) {
      m_light->clear();
   } else {
      m_generator->generateL(*m_light);
      //while(m_light->append(m_light->length() > 0));
   }
}

void PathVisualizer::_initE() {
   m_generator->generateE(*m_eye);
   //while(m_eye->prepend(m_eye->length() >= 2));
}

void PathVisualizer::_reset() {
   if (NULL == m_light)
      m_light = new Path(m_parent->getRenderer());
   else 
      m_light->clear();
   
   if (NULL == m_eye)
      m_eye   = new Path(m_parent->getRenderer());
   else 
      m_eye->clear();
   
   _init();
   
   Path aggregate = *m_light;
   bool valid = aggregate.append(*m_eye);
   cerr << "eye: " << m_eye->length() << ", light: " << m_light->length() 
        << ", " << (valid ? "valid" : "invalid") << endl;
}

