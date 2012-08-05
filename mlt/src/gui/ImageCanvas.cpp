/**<!-------------------------------------------------------------------->
   @file   ImageCanvas.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   <!-------------------------------------------------------------------->**/

#include "ImageCanvas.h"
#include "RenderThread.h"
#include "Gui.h"

#include <renderers/PointSample.h>
#include <utils/SpectralSampleSet.h>

namespace milton {

ImageCanvas::ImageCanvas(Gui *gui, unsigned width, unsigned height)
   : Canvas(gui, width, height), 
     ReconstructionRenderOutput(), 
     m_qimage(NULL), m_qfinalized(NULL), m_finalized(NULL)
{
   m_qimage = new QImage(m_width, m_height, QImage::Format_RGB32);
   m_mainProxy = new MainSyscallProxy();
   
   _init();
}

ImageCanvas::ImageCanvas(Gui *gui, const QImage &background)
   : Canvas(gui, background.width(), background.height()), 
     ReconstructionRenderOutput(), 
     m_qimage(new QImage(background)), m_qfinalized(NULL), m_finalized(NULL)
{
   _init();
}

ImageCanvas::~ImageCanvas() {
   // note: m_qfinalized must be deleted before m_finalized
   safeDelete(m_qimage);
   safeDelete(m_qfinalized);
   safeDelete(m_finalized);
}


bool ImageCanvas::save(const char *fileName) {
   if (m_qfinalized)
      return m_qfinalized->save(QString(fileName), NULL, -1);
   
   if (m_qimage == NULL)
      return false;
   
   return m_qimage->save(QString(fileName), NULL, -1);
}

bool ImageCanvas::load(const char *fileName) {
   QImage *temp = new QImage;
   if (!temp->load(QString(fileName)))
      return false;
   
   if (temp->format() != QImage::Format_RGB32) {
      QImage *old = temp;
      
      temp = new QImage(old->convertToFormat(QImage::Format_RGB32));
      safeDelete(old);
   }
   
   setSize(temp->width(), temp->height());
   
   RgbaImage temp32(*temp);
   m_output->copyData(&temp32);
   //memcpy(getData(), temp->bits(), temp->numBytes());
   update();
   
   safeDelete(temp);
   return true;
}


void ImageCanvas::setPixel(unsigned x, unsigned y, const Rgba32 &color) {
   ASSERT(x >= 0 && x < m_width);
   ASSERT(y >= 0 && y < m_height);
   ASSERT(m_qimage);
   
   m_qimage->setPixel(x, y, qRgb(color.r, color.g, color.b));
}

Rgba32 ImageCanvas::getPixel(unsigned x, unsigned y) const {
   ASSERT(x >= 0 && x < m_width);
   ASSERT(y >= 0 && y < m_height);
   ASSERT(m_qimage);
   
   Rgba32 c;
   QRgb rgb = m_qimage->pixel(x, y);
   
   c.r = (unsigned char)qRed(rgb);
   c.g = (unsigned char)qGreen(rgb);
   c.b = (unsigned char)qBlue(rgb);
   
   return c;
}

void ImageCanvas::setData(const Rgba32 *data) {
   ASSERT(data);
   
   safeDelete(m_qimage);
   
   m_qimage = new QImage((unsigned char*) data, m_width, m_height, QImage::Format_RGB32);
   ReconstructionRenderOutput::setImage(new RgbaImage(*m_qimage));
   
   update();
}

void ImageCanvas::setData(const Rgba32 *data, 
                          unsigned newWidth, 
                          unsigned newHeight)
{
   if (data == NULL)
      return;
   
   m_width  = newWidth;
   m_height = newHeight;
   
   if ((void*) data != (void*) m_qimage->bits()) {
      safeDelete(m_qimage);
      safeDelete(m_qfinalized);
      safeDelete(m_finalized);
      
      m_qimage = new QImage((unsigned char*) data, m_width, m_height, QImage::Format_RGB32);
      ReconstructionRenderOutput::setImage(new RgbaImage(*m_qimage));
   }
   
   _resizeImage(false);
}


void ImageCanvas::setWidth(unsigned width) {
   if (m_width != width) {
      m_width = width;
      
      _resizeImage();
   }
}

void ImageCanvas::setHeight(unsigned height) {
   if (m_height != height) {
      m_height = height;
      
      _resizeImage();
   }
}

void ImageCanvas::setSize(unsigned width, unsigned height) {
   if ((m_width != width || m_height != height) && width > 0 && height > 0) {
      m_width  = width;
      m_height = height;
      
      _resizeImage();
   }
}


bool ImageCanvas::supportsOpenGL() const {
   return false;
}


void ImageCanvas::render() {
   ASSERT(m_renderer);
   
   m_renderer->render();
}

void ImageCanvas::render(unsigned x0, unsigned y0, 
                         unsigned width, unsigned height)
{
   ASSERT(m_renderer);
   
   // TODO: handle subrendering
   m_renderer->render();
}

void ImageCanvas::redraw() {
   update();
}

void ImageCanvas::update() {
   if (QThread::currentThread() == qApp->thread()) {
      QWidget::update();
   } else {
      LocalSyscallProxy(this, m_mainProxy).repaint(0, 0, m_width, m_height);
   }
}

void ImageCanvas::update(int x, int y, int width, int height) {
   if (QThread::currentThread() == qApp->thread()) {
      QWidget::update(x, y, width, height);
   } else {
      LocalSyscallProxy(this, m_mainProxy).repaint(x, y, width, height);
   }
}

void ImageCanvas::_addSample(const unsigned row, const unsigned col, 
                             PointSample &sample, const real_t weight, 
                             bool lock)
{
   ReconstructionRenderOutput::_addSample(row, col, sample, weight, false);
   
   if (sample.update || sample.save) {
      if (m_renderer->isMLT()) {
         cout << "   tonemapping output" << endl;
         
         _resetFinalized();
      }
      
      if (sample.save)
         update(0, 0, m_width, m_height);
      else
         update(0, row, m_width, 1);
   }
}

void ImageCanvas::_resetFinalized() {
   QMutexLocker lock(&m_mltMutex);
   
   // TODO: more efficiently handling of resources, especially when updating 
   // frequently...
   RgbaImage *finalized = getFinalizedOutput();
   QImage *qfinalized   = new QImage((unsigned char*) finalized->getData(), 
                                     m_width, m_height, QImage::Format_RGB32);
   
   // note: m_qfinalized must be deleted before m_finalized
   safeDelete(m_qfinalized);
   safeDelete(m_finalized);
   
   m_finalized  = finalized;
   m_qfinalized = qfinalized;
}

void ImageCanvas::mousePressEvent(QMouseEvent *e) {
   mousePressed(e);
}

void ImageCanvas::mouseReleaseEvent(QMouseEvent *e) {
   mouseReleased(e);
}

void ImageCanvas::mouseMoveEvent(QMouseEvent *e) {
   mouseMoved(e);
}

void ImageCanvas::keyPressEvent(QKeyEvent *e) {
   keyPressed(e);
}

void ImageCanvas::keyReleaseEvent(QKeyEvent *e) {
   keyReleased(e);
}


QSize ImageCanvas::sizeHint() const {
   return QSize(getWidth(), getHeight());
}

QSize ImageCanvas::minimumSize() const {
   return sizeHint();
}

QSize ImageCanvas::maximumSize() const {
   return sizeHint();
}


void ImageCanvas::paintEvent(QPaintEvent *e) {
   ASSERT(m_qimage);
   
   if (NULL == m_qimage || m_qimage->isNull()) {
      cerr << "ImageCanvas::paintEvent invalid QImage" << endl;
      ASSERT(0);
      abort();
   }
   
   QPainter painter(this);
   /*const QRect clip(0, 0, m_width, m_height);
   p.setClipRect(clip);
   p.drawImage(clip, *m_qimage, clip);*/
   
   if (m_qfinalized)
      painter.drawImage(0, 0, *m_qfinalized);
   else 
      painter.drawImage(0, 0, *m_qimage);
   
   /* Notify all listeners of the paint event */
   ListenerListIter iter;
   painter.setRenderHint(QPainter::Antialiasing);
   
   for(iter = m_interactionListeners.begin(); iter != m_interactionListeners.end(); iter++) {
      (*iter)->paint(&painter);
   }
}


void ImageCanvas::_init() {
   ReconstructionRenderOutput::setImage(new RgbaImage(*m_qimage));
   
   _setupQWidget();
}

void ImageCanvas::_setupQWidget() {
   // minor optimization: tell Qt that we'll be painting our own background
   setAutoFillBackground(false);
   setAttribute(Qt::WA_NoSystemBackground);
   setFocusPolicy(Qt::StrongFocus);
   
   if (layout() != NULL)
      layout()->setSizeConstraint(QLayout::SetFixedSize);
   
   setMouseTracking(true);
   setFixedSize(m_width, m_height);
}

void ImageCanvas::_resizeImage(bool modifyImage) {
   if (modifyImage) {
      safeDelete(m_qimage);
      safeDelete(m_qfinalized);
      safeDelete(m_finalized);
      
      m_qimage = new QImage(m_width, m_height, QImage::Format_RGB32);
      ReconstructionRenderOutput::setImage(new RgbaImage(*m_qimage));
   }
   
   setFixedSize(m_width, m_height);
   //sizeChanged (m_width, m_height);
   
   if (modifyImage)
      update();
}

void ImageCanvas::_repaintPrivate(unsigned x, unsigned y, 
                                  unsigned w, unsigned h)
{
   QWidget::repaint(x, y, w, h);
}

}

