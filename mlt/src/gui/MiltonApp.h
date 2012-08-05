/**<!-------------------------------------------------------------------->
   @class  MiltonApp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Main entrypoint for Qt-based frontend which comes bundled with Milton 
   (including a non-graphical 'nox' mode for remote / batch rendering).
   
   @note 
      MiltonApp constructs an underlying QApplication, which must be 
   initialized before any other Qt user-interface objects are created.
   For this reason, initializing MiltonApp should generally be at the 
   very beginning of your program's mainline.

   @note
      MiltonApp is a singleton class and is only meant to be instantiated 
   once per program (generally the first line of main)
   <!-------------------------------------------------------------------->**/

#ifndef MILTON_APP_H_
#define MILTON_APP_H_

#include "MiltonParams.h"
#include "OpenGLCanvas.h"
#include "ImageCanvas.h"
#include "Gui.h"

class QApplication;

namespace milton {

class RenderOutput;

DECLARE_STL_TYPEDEF(std::vector<Renderer*>, RendererList);
DECLARE_STL_TYPEDEF(std::vector<Canvas*>,   CanvasList);

class MiltonApp {
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      /**
       * @brief   
       *    Constructs a MiltonApp from the commandline parameters given
       * 
       * @param title denotes the main titlebar of the Gui when it is enabled
       * @param enableGui denotes whether or not to default to having a Qt Gui
       *    (enableGui may be overridden via the '-nox' commandline flag which 
       *    indicates a strict non-graphical mode)
       * 
       * @note
       *    Gui mode may be disabled implicitly if the current environment 
       * or platform does not support QtGui or graphics in general (eg. no 
       * X-server)
       */
      MiltonApp(int &argc, char **argv, 
                const std::string &title = "Milton Renderer", 
                bool enableGui = true);
      
      virtual ~MiltonApp();
      
      
      //@}-----------------------------------------------------------------
      ///@name Initialization
      //@{-----------------------------------------------------------------
      
      /**
       * @brief
       *    Loads / initializes canvases and renderer jobs specified on the 
       * commandline (which was given to MiltonApp's constructor)
       * 
       * @note
       *    Invoking this method is completely optional and only provided 
       * for convenience; a MiltonApp can disregard its commandline input 
       * and construct a single OpenGLCanvas, for example, and still 
       * utilize the underlying Milton framework without actually rendering 
       * anything.
       * 
       * @returns whether or not initialization was successful (will fail 
       *    if commandline contained invalid arguments or if a scenefile 
       *    that was specified fails to load)
       */
      virtual bool loadCommandlineParams();
      
      /**
       * @brief
       *    Adds the given canvas to this MiltonApp, optionally specifying 
       * its subwindow's title @p name
       */
      virtual void addCanvas  (Canvas *canvas, const std::string &name = "");
      
      /**
       * @brief
       *    Adds a non-graphical render job to this MiltonApp, whose render 
       * method will be called in a new thread upon calling MiltonApp's exec
       */
      virtual void addRenderer(Renderer *renderer);
      
      
      //@}-----------------------------------------------------------------
      ///@name Main usage interface
      //@{-----------------------------------------------------------------
      
      /**
       * @brief
       *    Enters Milton's main rendering loop, blocking the calling thread 
       * until it completes. If Gui mode is enabled, this will initiate any 
       * non-deferred renders and enter the underlying QApplication's main 
       * loop (renders may be deferred via the -pause commandline flag, in 
       * which case you must hit 'render' in the Gui to initiate rendering).
       * If Gui mode is disabled, this will begin rendering all batch jobs 
       * which have been added to this app, either implicitly via the 
       * commandline or explicitly via addRenderer
       * 
       * @note
       *    If Gui mode is disabled, the -pause commandline flag has no effect,
       * and all renders are initiated right away upon entering exec
       * 
       * @note
       *    exec blocks the calling thread until execution completes
       * 
       * @returns whether or not execution was successful
       * @see isGuiEnabled
       */
      virtual bool exec();
      
      
      //@}-----------------------------------------------------------------
      ///@name Accessors
      //@{-----------------------------------------------------------------
      
      /**
       * @returns whether or not the Qt frontend and connection to the 
       *    underlying X-server is enabled
       * 
       * @note
       *    Gui mode will be disabled if the underlying runtime environment 
       * doesn't support Qt graphics or if the '-nox' flag was passed to 
       * MiltonApp via the commandline
       */
      inline bool isGuiEnabled() const {
         return m_params.enableGui;
      }
      
      /**
       * @returns the underlying parameters used by loadCommandlineParams
       *    to parse and load scenefiles
       */
      virtual MiltonParams &getParams() {
         return m_params;
      }
      
      /**
       * @returns the underlying parameters used by loadCommandlineParams
       *    to parse and load scenefiles
       */
      virtual const MiltonParams &getParams() const {
         return m_params;
      }
      
      /**
       * @returns the underlying Qt Gui (for convenience only)
       */
      virtual Gui *getGui() {
         return m_gui;
      }
      
      /**
       * @returns the underlying Qt Gui (for convenience only)
       */
      virtual const Gui *getGui() const {
         return m_gui;
      }
      
      /**
       * @returns the underlying Qt QApplication (for convenience only)
       */
      virtual QApplication *getQApplication() {
         return m_qApplication;
      }
      
      /**
       * @returns the underlying Qt QApplication (for convenience only)
       */
      virtual const QApplication *getQApplication() const {
         return m_qApplication;
      }
      
      /**
       * @returns an std::vector of non-graphical renderer jobs which have 
       *    been added to this MiltonApp via addRenderer
       */
      virtual RendererList &getRenderers() {
         return m_renderers;
      }
      
      /**
       * @returns an std::vector of non-graphical renderer jobs which have 
       *    been added to this MiltonApp via addRenderer
       */
      virtual const RendererList &getRenderers() const {
         return m_renderers;
      }
      
      /**
       * @returns an std::vector of Canvases which have been added to this 
       *    MiltonApp via addCanvas
       */
      virtual CanvasList &getCanvases() {
         return m_canvases;
      }
      
      /**
       * @returns an std::vector of Canvases which have been added to this 
       *    MiltonApp via addCanvas
       */
      virtual const CanvasList &getCanvases() const {
         return m_canvases;
      }
      
      
      //@}-----------------------------------------------------------------
      ///@name Miscellaneous
      //@{-----------------------------------------------------------------
      
      /**
       * @brief
       *    Prints an explanation of MiltonApp commandline usage to stderr
       * via std::cerr, including an optional error message
       */
      virtual void printUsage(int argc = 0, char **argv = NULL, 
                              const char *errorMsg = NULL);
      
   protected:
      /**
       * @brief
       *    Fills in params by parsing commandline arguments
       * 
       * @returns whether or not parsing was successful
       */
      virtual bool _initializeParams(int argc, char **argv, 
                                     MiltonParams &params);
      
      /**
       * @returns RenderOutput parsed from params.output
       */
      virtual RenderOutput *_getOutput(MiltonParams &params);
      
      
      //@}-----------------------------------------------------------------
      
   protected:
      MiltonParams  m_params;
      std::string   m_title;
      
      RendererList  m_renderers;
      CanvasList    m_canvases;
      
      QApplication *m_qApplication;
      Gui          *m_gui;
      
   private:
      /**
       * @brief
       *    Initializes the Milton stats package (see Random::init) and 
       * instantiates a Qt Gui if Gui mode is enabled
       */
      void _init();
      
   private:
      /**
       * @brief
       *    boolean denoting whether or not a MiltonApp has previously been 
       * constructed, in order to safeguard against undefined functionality 
       * in the event of multiple MiltonApps being instantiated
       */
      static bool s_instance;
};

}

#endif // MILTON_APP_H_

