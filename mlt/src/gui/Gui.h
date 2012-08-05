#ifndef GUI_H_
#define GUI_H_

#include <QtGui/QMainWindow>
#include <QtGui/QDockWidget>
#include <QtCore/QStack>

#include <common/common.h>

namespace milton {

class  Canvas;
class  CanvasContainer;
struct ParseData;

/// Top level GUI application
class Gui : public QMainWindow {
   Q_OBJECT
   
   public:
      // Expects four characters to be passed in
      inline explicit Gui(const std::string &title = "Milton Renderer")
      {
         _init(title);
         
         show();
      }
      
      virtual ~Gui()
      { }
      
      /// Add a canvas to the GUI with a default title
      void addCanvas(Canvas *canvas);
      /// Add a canvas to the GUI with the provided title
      void addCanvas(Canvas *canvas, const char *title);
      /// Load a scene with the given filename, returns a bool
      /// and SceneData (out argument)
      //bool loadScene(const char *sceneFile, ParseData &outData);
      
   public slots:
      void loadPressed();
      void savePressed();
      void renderPressed();
      
      void drawModeChanged();
      void enableAccelPreviewChanged();
      void enableDefaultLightingChanged();
      void selectComboBox(int);
      
   signals:
      void drawModeChanged(int style);
      void render();
      
   protected:
      void _init(const std::string &title);
      void _redrawAll();
      Canvas *_getActiveCanvas(const char *text);
      
   private:
      QStack<CanvasContainer *> m_canvases;
      QAction *m_wire;
      QAction *m_enableAccelPreview;
      QAction *m_enableDefaultLighting;
      int      m_selectedCombo;
      std::string m_lastSaved;
};

class CanvasContainer : public QDockWidget {
   
   Q_OBJECT
   
   public:
      CanvasContainer(Canvas *canvas, const char* title = NULL);
      virtual ~CanvasContainer();
      
      Canvas *getCanvas() const;
      char   *getTitle()  const;
      
   private slots:
      void synchronizeSize(int, int);
   
   protected:
      void setupContainer();
      
      // @overridden from QWidget
      virtual QSize sizeHint() const;
      virtual QSize minimumSize() const;
      virtual QSize maximumSize() const;
      
      Canvas *m_canvas;
      QString m_title;
};

}

#endif // GUI_H_

