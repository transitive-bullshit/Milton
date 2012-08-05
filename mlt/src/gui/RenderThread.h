/**<!-------------------------------------------------------------------->
   @class  RenderThread
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Intermediary for communicating/blocking between Gui and non-Gui threads 
   in Qt
   <!-------------------------------------------------------------------->**/

#ifndef RENDER_THREAD_H_
#define RENDER_THREAD_H_

#include <QtCore/QThread>
#include <common/common.h>

namespace milton {

class Renderer;
class ImageCanvas;

class RenderThread : public QThread {
   
   public:
      RenderThread(Renderer *renderer);
      virtual ~RenderThread();
      
   protected:
      virtual void run();
      
   protected:
      Renderer *m_renderer;
};

// this class acts as a signal emitter in the current thread
// its intended to be created, used once, then destroyed
class LocalSyscallProxy : public QObject {
   Q_OBJECT
   
   public:
      LocalSyscallProxy(ImageCanvas *canvas, QObject *mainProxy = NULL);
      void repaint(int x1, int y1, int w, int h);
      void connectTo(QObject *mainProxy);
      
   signals:
      void sig_repaint(void*, int x1, int y1, int w, int h);
      
   private:
      ImageCanvas *m_canvas;
};

// all instances of this class will push themselves into the main thread
class MainSyscallProxy : public QObject {
   Q_OBJECT
   
   public:
      MainSyscallProxy();
      
   public slots:
      void slot_repaint(void *target, int x1, int y1, int w, int h);
};

}

#endif

