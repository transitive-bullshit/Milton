/**<!-------------------------------------------------------------------->
   @file   RenderThread.h
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Intermediary for communicating/blocking between Gui and non-Gui threads 
   in Qt
   <!-------------------------------------------------------------------->**/

#include "RenderThread.h"
#include "ImageCanvas.h"

#include <renderers/Renderer.h>
#include <QtGui/QApplication>

namespace milton {

RenderThread::RenderThread(Renderer *renderer)
   : QThread(), m_renderer(renderer)
{ }

RenderThread::~RenderThread()
{ }

void RenderThread::run() {
   if (m_renderer)
      m_renderer->render();
}

/* Implementation of LocalSyscallProxy */
LocalSyscallProxy::LocalSyscallProxy(ImageCanvas *canvas, QObject *mainProxy)
   : m_canvas(canvas)
{
   if (mainProxy) // auto-connect to mainProxy
      connectTo(mainProxy);
}

void LocalSyscallProxy::repaint(int x1, int y1, int w, int h) {
   emit sig_repaint((void*)m_canvas, x1, y1, w, h);
}

void LocalSyscallProxy::connectTo(QObject *mainProxy) {
   Qt::ConnectionType connectionKind;
   
   if (this->thread() != mainProxy->thread()) {
      connectionKind = Qt::BlockingQueuedConnection;
   } else {
      connectionKind = Qt::DirectConnection;
   }
   
   // make the connections between this and the main thread proxy
   connect(this, SIGNAL(sig_repaint(void*,int, int, int, int)),
           mainProxy, SLOT(slot_repaint(void*,int, int, int, int)),
           connectionKind);
}

MainSyscallProxy::MainSyscallProxy() {
   // push myself to the main thread
   moveToThread(qApp->thread());
}

// the following method just relays the call to the handler
void MainSyscallProxy::slot_repaint(void *t, int x1, int y1, int w, int h) {
   ImageCanvas *target = (ImageCanvas *) t;
   target->_repaintPrivate(x1, y1, w, h);
}

}

