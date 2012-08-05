/**<!-------------------------------------------------------------------->
   @class  ImageCanvas
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Provides a Qt canvas for displaying 2D raster data
   <!-------------------------------------------------------------------->**/

#ifndef IMAGE_CANVAS_H_
#define IMAGE_CANVAS_H_

#include <renderers/outputs/ReconstructionRenderOutput.h>
#include <common/image/RgbaImage.h>
#include "Canvas.h"
#include <QtGui/QWidget>

class QMouseEvent;
class QPaintEvent;
class QImage;
class QTimerEvent;

namespace milton {

struct PointSample;
class  MainSyscallProxy;

class ImageCanvas : public QWidget, 
                    public Canvas, 
                    public ReconstructionRenderOutput
{
   Q_OBJECT
   
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      ImageCanvas(Gui *gui, 
                  unsigned width  = DEFAULT_WIDTH, 
                  unsigned height = DEFAULT_HEIGHT);
      ImageCanvas(Gui *gui, 
                  const QImage &background);
      
      virtual ~ImageCanvas();
      
      
      //@}-----------------------------------------------------------------
      ///@name Saving/Loading
      //@{-----------------------------------------------------------------
      
      /**
       * Attempts to saver this canvas' image to the specified file
       * @returns whether or not the save was successful
       */
      bool save(const char *fileName);
      
      /**
       * Attempts to load this canvas' image from the specified file
       * @returns whether or not the load was successful
       */
      bool load(const char *fileName);
      
      
      //@}-----------------------------------------------------------------
      ///@name Pixel data accessors/mutators
      //@{-----------------------------------------------------------------
      
      /**
       * Sets an individual pixel's data on this canvas
       */
      void setPixel(unsigned x, unsigned y, const Rgba32 &val);
      
      /**
       * @returns an individual pixel's data
       */
      Rgba32 getPixel(unsigned x, unsigned y) const;
      
      /**
       * Sets all of the pixel data on this canvas
       */
      void setData(const Rgba32 *data);
      
      /**
       * Sets all of the pixel data and resizes this canvas, all without 
       * allocating any temporary memory inbetween
       */
      void setData(const Rgba32 *data, unsigned newWidth, unsigned newHeight);
      
      /**
       * @returns the underlying image
       */
      inline Image *getImage() {
         ASSERT(m_output);
         
         return m_output;
      }
      
      
      //@}-----------------------------------------------------------------
      ///@name Canvas dimensions
      //@{-----------------------------------------------------------------
      
      void setWidth(unsigned width);
      void setHeight(unsigned height);
      void setSize(unsigned width, unsigned height);
      
      
      //@}-----------------------------------------------------------------
      
      virtual bool supportsOpenGL() const;
      
      virtual QWidget *getQWidget() {
         return this;
      }
      
      
   public:
      // for synchronous gui updates during concurrent rendering
      friend class MainSyscallProxy;
      
   public slots:
      ///@name Main usage interface
      //@{-----------------------------------------------------------------
      
      /// Attempts to render the entire canvas
      void render();
      
      /// Attempts to render the specified rectangular subregion on the canvas
      void render(unsigned x0, unsigned y0, 
                  unsigned width, unsigned height);
      
      /// Updates the entire canvas (same as update)
      virtual void redraw();
      
      /// Updates the entire canvas
      virtual void update();
      
      /// Updates the specified rectangular subregion on the canvas
      virtual void update(int x, int y, int width, int height);
      
      
      //@}-----------------------------------------------------------------
      
   protected:
      ///@name Overridden from QWidget
      //@{-----------------------------------------------------------------
      
      virtual void mousePressEvent(QMouseEvent *);
      virtual void mouseReleaseEvent(QMouseEvent *);
      virtual void mouseMoveEvent(QMouseEvent *);
      
      virtual void keyPressEvent(QKeyEvent *event);
      virtual void keyReleaseEvent(QKeyEvent *event);
      
      virtual QSize sizeHint()    const;
      virtual QSize minimumSize() const;
      virtual QSize maximumSize() const;
      
      virtual void paintEvent(QPaintEvent *e);
      
      
      //@}-----------------------------------------------------------------
      ///@name Overridden from RenderOutput
      //@{-----------------------------------------------------------------
      
      virtual void _addSample(const unsigned row, const unsigned col, 
                              PointSample &value, const real_t weight, 
                              bool lock = false);
      
      
      //@}-----------------------------------------------------------------
      
   protected:
      QImage           *m_qimage;
      QImage           *m_qfinalized;
      RgbaImage        *m_finalized;
      
      MainSyscallProxy *m_mainProxy;
      unsigned         *m_renderCount;
      QMutex            m_mltMutex;
      
   private:
      void _init();
      void _setupQWidget();
      void _resizeImage(bool modifyImage = true);
      void _resetFinalized();
      
      void _repaintPrivate(unsigned x, unsigned y, unsigned w, unsigned h);
};

}

#endif // IMAGE_CANVAS_H_

