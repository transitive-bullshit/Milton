#include "Gui.h"
#include "OpenGLCanvas.h"
#include "ImageCanvas.h"
#include "RenderThread.h"

#include <milton.h>
#include <QtGui/QtGui>

#include <fstream>
#include <typeinfo>
#include <iostream>
using namespace std;

namespace milton {

void Gui::_init(const std::string &title) {
   QPalette plastique;
   QApplication::setStyle("plastique");
   
   QColor brightText, base, highlight, button, mid,
          text, windowText, buttonText, window, 
          highlightedText;

   highlightedText.setRgb(255,255,255);
   base.setRgb(255,255,255);
   highlight.setRgb(157, 187, 227);
   button.setRgb(240,240,240);
   mid.setRgb(75,75,75);
   brightText.setRgb(0,0,0);
   text.setRgb(0,0,0);
   windowText.setRgb(0,0,0);
   buttonText.setRgb(0,0,0);
   window.setRgb(255,255,255);
   
   plastique.setBrush(QPalette::HighlightedText, highlightedText);
   plastique.setBrush(QPalette::BrightText, brightText);
   plastique.setBrush(QPalette::Base, base);
   plastique.setBrush(QPalette::Highlight, highlight);
   plastique.setBrush(QPalette::Button, button);
   plastique.setBrush(QPalette::Mid, mid);
   plastique.setBrush(QPalette::Text, text);
   plastique.setBrush(QPalette::WindowText, windowText);
   plastique.setBrush(QPalette::ButtonText, buttonText);
   plastique.setBrush(QPalette::Window, window);

   QApplication::setPalette(plastique);
   
   setWindowTitle(QString(title.c_str()));
   setDockOptions(QMainWindow::AnimatedDocks | QMainWindow::AllowNestedDocks);
   
   QMenu *file = menuBar()->addMenu(QString("&File"));
   QMenu *options = menuBar()->addMenu(QString("&Options"));
   
   m_lastSaved = "/map/gfx0/users/tfischer/mlt/data/renders";
   
   /* FILE */
   file->addAction(QString("&Load Scene"), this, SLOT(loadPressed()));
   file->addAction(QString("&Save Render"), this, SLOT(savePressed()));
   file->addSeparator();
   
   file->addAction(QString("&Quit"), qApp, SLOT(quit()));   
   
   /* OPTIONS */
   options->addAction(QString("Render"), this, SLOT(renderPressed()));
   options->addSeparator();
   
   m_enableAccelPreview = options->addAction(QString("Preview Mesh kd-Trees"), this, SLOT(enableAccelPreviewChanged()));
   m_enableDefaultLighting = options->addAction(QString("Enable Default Lighting"), this, SLOT(enableDefaultLightingChanged()));
   
   options->addSeparator();
   
   m_enableAccelPreview->setCheckable(true);
   m_enableDefaultLighting->setCheckable(true);
   
   m_enableAccelPreview->setChecked(ResourceManager::getValue<bool>("enableAccelPreview", false));
   m_enableDefaultLighting->setChecked(ResourceManager::getValue<bool>("enableDefaultLighting", true));
   
   
   ASSERT(m_enableDefaultLighting->isChecked() == ResourceManager::getValue<bool>("enableDefaultLighting"));
   
   m_wire = options->addAction(QString("Wireframe"), this, SLOT(drawModeChanged()));
   QAction *action = options->addAction(QString("Solid"), this, SLOT(drawModeChanged()));
   m_wire->setCheckable(true);
   action->setCheckable(true);
   
   QActionGroup *actionGroup = new QActionGroup(this);
   actionGroup->setExclusive(true);
   actionGroup->addAction(m_wire);
   actionGroup->addAction(action);
   
   m_wire->setChecked(false);
   action->setChecked(true);
    
   layout()->setSizeConstraint(QLayout::SetFixedSize);
}

/*bool Gui::loadScene(const char *sceneFile, ParseData &data) {
   MiltonJSONSceneLoader loader;
   
   if (!loader.parse(sceneFile, data)) {
      cerr << "Parse error: " << data.error << endl;
      QMessageBox::warning(this, "Warning", 
                           QString("Error loading scenefile (%1): %2").arg(
                              sceneFile, QString(data.error.c_str())));
      return false;
   }
   
   return true;
}*/

void Gui::addCanvas(Canvas *canvas, const char* title) {
   // All dockwidgets must go into same area if you want them to be tabbed
   Qt::DockWidgetArea area = Qt::RightDockWidgetArea;
   
   setUpdatesEnabled(false);
   
   CanvasContainer *container = new CanvasContainer(canvas, title);
   addDockWidget(area, container);
   if (!m_canvases.isEmpty())
      tabifyDockWidget(m_canvases.top(), container);
   
   m_canvases.push(container);
   
   setUpdatesEnabled(true);
   
   // Notify canvas when mesh properties are modified
   
   if (canvas->supportsOpenGL()) {
      OpenGLCanvas *openGLCanvas = NULL;
      
      try {
         openGLCanvas = dynamic_cast<OpenGLCanvas*>(canvas);
      } catch(std::bad_cast &) { }
      
      if (openGLCanvas) {
         connect(this, SIGNAL(drawModeChanged(int)), openGLCanvas, 
                 SLOT(setDrawMode(int)));
      }
   } else {
      ImageCanvas *imageCanvas = NULL;
      
      try {
         imageCanvas = dynamic_cast<ImageCanvas*>(canvas);
      } catch(std::bad_cast &) { }
      
      if (imageCanvas) {
         connect(this, SIGNAL(render()), imageCanvas, SLOT(render()));
      }
   }
}

void Gui::addCanvas(Canvas* canvas) {
   addCanvas(canvas, "Canvas");
}

void Gui::drawModeChanged() {
   emit drawModeChanged(!((int)m_wire->isChecked()));
}

void Gui::loadPressed() {
   QMessageBox::warning(this, "Warning", 
                        QString("scene loading currently unsupported"));
   
   return;
   
#if 0
   const QString &fileName = QFileDialog::getOpenFileName(
         this, 
         "Choose a scenefile to open",
         "/map/gfx0/users/mjacobs/cs197/src/gui/scenes");
   
   if (fileName.isEmpty())
      return;
   
   const char *sceneFile = fileName.toAscii().data();
   
   /* Set the busy cursor for the app */
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   
   ParseData data(true);
   
   loadScene(sceneFile, data);
   
   /* Set it back to the normal cursor */
   QApplication::restoreOverrideCursor();
#endif
}

void Gui::savePressed() {
   const QString &fileName = QFileDialog::getSaveFileName(
      this,
      "Choose a filename to save to", 
      m_lastSaved.c_str(), 
      "Images (*.png *.jpg *.tiff)");
   
   if (fileName.isEmpty())
      return;
   
   // TODO: on Windows, use '\' as separator
   m_lastSaved = fileName.left(fileName.lastIndexOf('/')).toAscii().data();
   
   Canvas *active = _getActiveCanvas("Select the canvas you would like to save:");
   
   if (active) {
      bool success = active->save(fileName.toAscii().data());
      
      if (!success)
         QMessageBox::warning(this, "Error", QString("Could not save to file '%1'").arg(fileName));
   }
}

void Gui::renderPressed() {
   foreach(CanvasContainer *container, m_canvases) {
      if (container != NULL) {
         Canvas *canvas = container->getCanvas();
         ASSERT(canvas);
         
         if (!canvas->supportsOpenGL()) {
            ImageCanvas *imageCanvas = dynamic_cast<ImageCanvas*>(canvas);
            ASSERT(imageCanvas);
            
            Renderer *renderer = imageCanvas->getRenderer();
            
            if (renderer) {
               Image *image = imageCanvas->getImage();
               Rgba32 black(0, 0, 0, 255);
               ASSERT(image);
               
               for(unsigned i = image->getHeight(); i--;)
                  for(unsigned j = image->getWidth(); j--;)
                     image->setPixel(i, j, black);
               
               /*memset(imageCanvas->getData(), 0, 
                      imageCanvas->getWidth() * imageCanvas->getHeight() * 
                      sizeof(struct Rgba32));*/
               imageCanvas->update();
               
               // TODO: take care of memory leak here
               RenderThread *thread = new RenderThread(renderer);
               thread->start();
            }
         }
      }
   }
}

void Gui::enableAccelPreviewChanged() {
   bool enabled = m_enableAccelPreview->isChecked();
   ResourceManager::insert<bool>("enableAccelPreview", enabled);
   ASSERT(ResourceManager::getValue<bool>("enableAccelPreview") == enabled);
   
   _redrawAll();
}

void Gui::enableDefaultLightingChanged() {
   bool enabled = m_enableDefaultLighting->isChecked();
   ResourceManager::insert<bool>("enableDefaultLighting", enabled);
   ASSERT(ResourceManager::getValue<bool>("enableDefaultLighting") == enabled);
   
   _redrawAll();
}

void Gui::_redrawAll() {
   foreach(CanvasContainer *container, m_canvases) {
      if (container) {
         Canvas *canvas = container->getCanvas();
         
         if (canvas)
            canvas->redraw();
      }
   }
}

void Gui::selectComboBox(int index) {
   m_selectedCombo = index;
}

Canvas *Gui::_getActiveCanvas(const char *text) {
   Canvas *visible = NULL;

   QComboBox *available = new QComboBox(this);
   foreach(CanvasContainer *container, m_canvases) {
      if (container != NULL) {
         available->addItem(container->getTitle());
         visible = container->getCanvas();
      }
   }
   
   if (available->count() <= 1 && visible)
      return visible;
   
   connect(available, SIGNAL(currentIndexChanged(int)), this, SLOT(selectComboBox(int)));

   QDialog *d = new QDialog(this);
   d->setModal(true);
   QGridLayout *l = new QGridLayout(d);
   QLabel *label = new QLabel(QString(text), this);
   l->addWidget(label, 0, 0);
   l->addWidget(available, 1, 0);
   QPushButton *okay = new QPushButton(tr("Okay"), this);
   QPushButton *cancel = new QPushButton(tr("Cancel"), this);

   l->addWidget(okay, 2, 0);
   l->addWidget(cancel, 2, 1);

   d->setLayout(l);

   connect(okay, SIGNAL(released()), d, SLOT(accept()));
   connect(cancel, SIGNAL(released()), d, SLOT(reject()));
   
   m_selectedCombo = 0;
   int res = d->exec();
   
   if (res == QDialog::Rejected)
      return NULL;
   
   int i = 0;
   foreach(CanvasContainer *container, m_canvases) {
      if (container != NULL) {
         if (i++ == m_selectedCombo) {
            visible = container->getCanvas();
            break;
         }
      }
   }

   return visible;
}


// --------------------------
// CanvasContainer
// --------------------------

CanvasContainer::CanvasContainer(Canvas* canvas, const char* title)
   : QDockWidget(tr(title)), m_canvas(canvas), 
     m_title(title ? title : "Canvas")
{
   setupContainer();
}

CanvasContainer::~CanvasContainer() 
{ }

void CanvasContainer::setupContainer() {
   QWidget *widget = m_canvas->getQWidget();
   assert(widget);
   
   setWidget(widget);
   
   if (layout())
      layout()->setSizeConstraint(QLayout::SetFixedSize);
   
   setFixedSize(sizeHint());
   setContentsMargins(0, 0, 0, 0);
}

void CanvasContainer::synchronizeSize(int, int) {
   bool floating = isFloating();
   setFloating(!floating);
   setFloating(floating);
   
   if (layout())
      layout()->setSizeConstraint(QLayout::SetFixedSize);
   
   setFixedSize(sizeHint());
   setContentsMargins(0, 0, 0, 0);
}

QSize CanvasContainer::sizeHint() const {
   return QSize(m_canvas->getWidth(), m_canvas->getHeight());
}

QSize CanvasContainer::minimumSize() const {
   return sizeHint();
}

QSize CanvasContainer::maximumSize() const {
   return sizeHint();
}

Canvas *CanvasContainer::getCanvas() const {
   return m_canvas;
}

char *CanvasContainer::getTitle() const {
   return m_title.toAscii().data();
}

}

