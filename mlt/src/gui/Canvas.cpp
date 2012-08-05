/**<!-------------------------------------------------------------------->
   @class  Canvas.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Provides a generic Canvas interface, shared between OpenGL canvases 
   and pixel-based canvases
   <!-------------------------------------------------------------------->**/

#include "Canvas.h"
#include "Gui.h"

#include <cameras/Camera.h>
#include <core/Scene.h>

namespace milton {

Canvas::Canvas(Gui *gui, unsigned width, unsigned height)
   : m_gui(gui), m_renderer(NULL), m_width(width), m_height(height), 
     m_initted(false)
{
   _init();
}

Canvas::~Canvas() {
   safeDelete(m_renderer);
}

void Canvas::_init() {
   //ASSERT(m_gui != NULL);
}

void Canvas::init() {
   if (!m_initted) {
      m_initted = true;
      
      if (m_renderer)
         m_renderer->init();
      
      ListenerListIter iter;
      
      for(iter = m_interactionListeners.begin(); iter != m_interactionListeners.end(); iter++) {
         (*iter)->init();
      }
   }
}

void Canvas::registerInteractionListener(InteractionListener *listener) {
   ListenerListIter iter;
   ASSERT(listener);
   
   for(iter = m_interactionListeners.begin(); iter != m_interactionListeners.end(); iter++) {
      if (*iter == listener)
         return;
   }
   
   // listener is not already registered
   m_interactionListeners.push_back(listener);
}

void Canvas::unregisterInteractionListener(InteractionListener *listener) {
   ListenerListIter iter;
   ASSERT(listener);
   
   for(iter = m_interactionListeners.begin(); iter != m_interactionListeners.end(); iter++) {
      if (*iter == listener)
         break;
   }
   
   if (iter != m_interactionListeners.end()) {
      ASSERT(*iter == listener);
      m_interactionListeners.erase(iter);
      
      safeDelete(listener);
   }
}

void Canvas::mousePressed(QMouseEvent *event) {
   /* Notify all listeners of the event */
   InteractionInfo info(event);
   ListenerListIter iter;
   
   for(iter = m_interactionListeners.begin(); iter != m_interactionListeners.end(); iter++) {
      if (info.propogateEvent)
         (*iter)->mousePressEvent(info);
   }
}

void Canvas::mouseReleased(QMouseEvent *event) {
   /* Notify all listeners of the event */
   InteractionInfo info(event);
   ListenerListIter iter;
   
   for(iter = m_interactionListeners.begin(); iter != m_interactionListeners.end(); iter++) {
      if (info.propogateEvent)
         (*iter)->mouseReleaseEvent(info);
   }
}

void Canvas::mouseMoved(QMouseEvent *event) {
   /* Notify all listeners of the event */
   InteractionInfo info(event);
   ListenerListIter iter;
   
   for(iter = m_interactionListeners.begin(); iter != m_interactionListeners.end(); iter++) {
      if (info.propogateEvent)
         (*iter)->mouseMoveEvent(info);
   }
}

void Canvas::keyPressed(QKeyEvent *event) {
   /* Notify all listeners of the event */
   InteractionInfo info(event);
   ListenerListIter iter;
   
   for(iter = m_interactionListeners.begin(); iter != m_interactionListeners.end(); iter++) {
      if (info.propogateEvent)
         (*iter)->keyPressEvent(info);
   }
}

void Canvas::keyReleased(QKeyEvent *event) {
   /* Notify all listeners of the event */
   InteractionInfo info(event);
   ListenerListIter iter;
   
   for(iter = m_interactionListeners.begin(); iter != m_interactionListeners.end(); iter++) {
      if (info.propogateEvent)
         (*iter)->keyReleaseEvent(info);
   }
}

void Canvas::setRenderer(Renderer *renderer) {
   m_renderer = renderer;
}

}

