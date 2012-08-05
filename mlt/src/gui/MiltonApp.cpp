/**<!-------------------------------------------------------------------->
   @file   MiltonApp.cpp
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

#include "MiltonApp.h"
#include "RenderThread.h"
#include <milton.h>

#include <QtGui/QApplication>
#include <GL/glut.h>
#include <typeinfo>

#define DEFAULT_SCENEFILE   "default.js"

#ifdef MILTON_ARCH_WINDOWS
#  include "MiltonAppWindows.inl"
#endif

namespace milton {

bool MiltonApp::s_instance = false;

MiltonApp::MiltonApp(int &argc, char **argv, const std::string &title, 
                     bool enableGui)
   : m_title(title), m_qApplication(NULL), m_gui(NULL)
{
#if (defined(MILTON_ARCH_WINDOWS) && defined(DEBUG))
   s_redirect_windows_io_to_console();
#endif
   
   if (s_instance) {
      ASSERT(0 && "error: only one MiltonApp instance allowed per application");
      abort();
   }
   
   s_instance         = true;
   m_params.enableGui = enableGui;
   
   // initialize commandline parameters
   if (!_initializeParams(argc, argv, m_params))
      printUsage(argc, argv, "error parsing commandline input");
   
   // initialize Qt
   m_qApplication = new QApplication(argc, argv, m_params.enableGui);
   
   _init();
}

MiltonApp::~MiltonApp() {
   safeDelete(m_gui);
   safeDelete(m_qApplication);
   
   FOREACH(RendererListIter, m_renderers, iter) {
      Renderer *renderer = *iter;
      
      safeDelete(renderer);
   }
}

void MiltonApp::_init() {
   // initialize the stats package
   /*m_params.seed = 1239694988;
   cerr << "WARNING: using hard-coded seed: " << m_params.seed << endl;
   
   #warning ""
   #warning ""
   #warning ""
   #warning ""
   #warning "hard-coded seed"
   #warning ""
   #warning ""
   #warning ""
   #warning ""
   #warning ""*/
   Random::init(m_params.seed);
   
   m_gui = (m_params.enableGui ? new Gui(m_title) : NULL);
}

bool MiltonApp::loadCommandlineParams() {
   ResourceManager::insert<bool>("enableGui",    m_params.enableGui);
   ResourceManager::insert<bool>("forcePreview", m_params.forcePreview);
   
   // load each scenefile specified
   FOREACH(StringListIter, m_params.scenefiles, iter) {
      const std::string &sceneFile = *iter;
      MiltonJSONSceneLoader loader;
      ParseData data(true);
      
      if (!loader.parse(sceneFile, data)) {
         cerr << "parse error: " << data.error << endl << endl;
         printUsage();
         return false;
      }
      
      ASSERT(data.renderer);
      ASSERT(data.camera);
      Canvas *canvas = NULL;
      
      bool createOutput = (NULL == data.output);
      if (!createOutput && m_params.enableGui && 
          !data.renderer->supportsOpenGL()) 
      {
         ReconstructionRenderOutput *output = NULL;
         
         try {
            output = dynamic_cast<ReconstructionRenderOutput*>(data.output);
         } catch(std::bad_cast&) { }
         
         createOutput = (output != NULL);
      }
      
      if (createOutput) {
         if (!m_params.enableGui) {
            // no output specified and gui is disabled, so use default output
            ResourceManager::log.info << 
               "no output specified; defaulting to file '" << 
               m_params.output << "'" << endl;
            
            data.output = _getOutput(m_params);
            if (NULL == data.output) {
               cerr << "error loading output" << endl;
               return false;
            }
            
            FileRenderOutput *output = 
               dynamic_cast<FileRenderOutput*>(data.output);
            output->setImage(new RgbaImage(data.outputProperties.getValue<unsigned>("width"),  
                                           data.outputProperties.getValue<unsigned>("height")));
         } else {
            // no output specified, so default to gui
            
            if (data.renderer->supportsOpenGL()) {
               canvas  = new OpenGLCanvas(m_gui, 
                                          data.outputProperties.getValue<unsigned>("width"),  
                                          data.outputProperties.getValue<unsigned>("height"));
            } else {
               ImageCanvas *icanvas = NULL;
               
               icanvas = new ImageCanvas(m_gui, 
                                         data.outputProperties.getValue<unsigned>("width"),  
                                         data.outputProperties.getValue<unsigned>("height"));
               
               canvas = icanvas;
               ReconstructionRenderOutput *oldOutput = 
                  (ReconstructionRenderOutput*)data.output;
               
               data.output = icanvas;
               data.output->inherit(data.outputProperties);
               
               if (oldOutput) {
                  icanvas->inherit(*oldOutput);
                  icanvas->setFilter(oldOutput->getFilter());
                  
                  oldOutput->setFilter(NULL);
                  safeDelete(oldOutput);
               }
               
               data.output->init();
            }
            
            canvas->setRenderer(data.renderer);
         }
         
         if (data.output) {
            PointSampleRenderer *renderer = NULL;
            
            try {
               renderer = dynamic_cast<PointSampleRenderer*>(data.renderer);
            } catch(std::bad_cast&) { }
            
            if (renderer)
               renderer->setOutput(data.output);
         }
      }
      
      if (canvas)
         addCanvas(canvas, data.source);
      else 
         addRenderer(data.renderer);
   }
   
   return true;
}

void MiltonApp::addCanvas(Canvas *canvas, const std::string &name) {
   ASSERT(canvas);
   
   if (NULL == m_gui) {
      cerr << "GUI mode must be enabled to add a canvas to MiltonApp!" << endl;
      ASSERT(0);
      
      return;
   }
   
   canvas->init();
   m_gui->addCanvas(canvas, name.c_str());
   
   m_canvases.push_back(canvas);
}

void MiltonApp::addRenderer(Renderer *renderer) {
   ASSERT(renderer);
   
   renderer->init();
   m_renderers.push_back(renderer);
}

bool MiltonApp::exec() {
   std::vector<RenderThread*> renderThreads;
   bool success = true;
   
   FOREACH(RendererListIter, m_renderers, iter) {
      Renderer *renderer = *iter;
      
      if (renderer) {
         RenderThread *thread = new RenderThread(renderer);
         thread->start();
         
         renderThreads.push_back(thread);
      }
   }
   
   if (m_gui) {
      if (m_params.startRender) {
         QTimer::singleShot(50, m_gui, SLOT(renderPressed()));
      } else {
         ResourceManager::log.info << endl;
         ResourceManager::log.info << "paused -- select Options->Render to "
            "begin rendering (because of '-pause' commandline flag)" << endl;
      }
      
      success = m_qApplication->exec();
   }
   
   for(unsigned i = 0; i < renderThreads.size(); ++i) {
      RenderThread *thread = renderThreads[i];
      
      while(!thread->wait());
      safeDelete(thread);
   }
   
   return success;
}

void MiltonApp::printUsage(int argc, char **argv, const char *errorMsg) {
   const std::string &tab  = " \t";
   const std::string &tab1 = "   ";
   
   if (errorMsg)
      cerr << errorMsg << endl << endl;
   
   cerr << "Usage: " << (argv ? argv[0] : "miltonapp") << " <options> <scenefile>*" << endl 
        << tab1 << "Note: if no scenefile is specified, the default scenefile '" DEFAULT_SCENEFILE "' will be used" << endl << endl
        << "Options: (convenience, shortcut flags)" << endl
        << tab1 << "            " << tab << " (options later in commandline will override those " << endl
        << tab1 << "            " << tab << "  listed earlier)" << endl
        << tab1 << "--help      " << tab << ": prints out this usage information" << endl
        << tab1 << "-nox        " << tab << ": disables the Qt gui frontend for environments without" << endl
        << tab1 << "            " << tab << "  X support)" << endl
        << tab1 << "-gui        " << tab << ": enables the Qt gui frontend" << endl
        << tab1 << "-verbose -v " << tab << ": enables extra debugging printouts" << endl
        << tab1 << "-nopreview  " << tab << ": disables forcing of OpenGL preview of loaded scenes" << endl
        << tab1 << "-preview    " << tab << ": overrides renderer specified in scenefiles to default" << endl 
        << tab1 << "            " << tab << "  to an OpenGL preview" << endl
        << tab1 << "-pause      " << tab << ": disable rendering right away upon initialization when" << endl
        << tab1 << "            " << tab << "  Qt gui is enabled" << endl
        << tab1 << "-seed=%d    " << tab << ": sets the seed for the random number generator" << endl
        << tab1 << "-output=%s  " << tab << ": quickly specifies an output file to store rendered" << endl
        << tab1 << "            " << tab << "  results to. if the suffix for the filename given is" << endl
        << tab1 << "            " << tab << "  a valid image format, the results of rendering will" << endl
        << tab1 << "            " << tab << "  be saved as an image in the specified format, " << endl
        << tab1 << "            " << tab << "  otherwise, raw output of film plane positions -> " << endl
        << tab1 << "            " << tab << "  spectral radiance values will be recorded in a simple," << endl
        << tab1 << "            " << tab << "  unordered ASCII format:" << endl
        << tab1 << "            " << tab << "  x y L0 L1 ... LN" << endl
        << tab1 << "            " << tab << "  x y L0 L1 ... LN" << endl;
        /*<< tab1 << "            " << tab << "  ..." << endl << endl
        << "The following options use standard Milton JSON syntax to override possible "
           "equivalent definitions which may be listed in the scenefile(s) given" << endl
        << tab1 << "Example: " << endl
        << tab1 << tab << "-output={\"type\":\"naive\",\"width\":640,\"height\":480}" << endl
        << tab1 << "-output=%s  " << endl
        << tab1 << "-renderer=%s" << endl
        << tab1 << "-camera=%s  " << endl;*/
}

bool MiltonApp::_initializeParams(int argc, char **argv, MiltonParams &params) {
   int index = 0;
   
   /* Parse optional arguments */
   while(++index < argc) {
      const char firstChar = argv[index][0];
      
      if (firstChar == '\0') {
         return false;
      }
      if (firstChar != '-' && firstChar != '/') {
         params.scenefiles.push_back(std::string(argv[index]));
      } else {
         const char *str = argv[index] + 1;
         
         if (!strcmp(str, "nox")) {
            params.enableGui    = false;
         } else if (!strcmp(str, "gui")) {
            params.enableGui    = true;
         } else if (!strcmp(str, "v") || !strcmp(str, "verbose")) {
            params.verbose      = true;
         } else if (!strcmp(str, "nopreview")) {
            params.forcePreview = false;
         } else if (!strcmp(str, "preview")) {
            params.forcePreview = true;
         } else if (!strncmp(str, "seed=", strlen("seed="))) {
            params.seed         = atol(str + strlen("seed="));
         } else if (!strcmp(str, "pause")) {
            params.startRender  = false;
         } else if (!strncmp(str, "output=", strlen("output="))) {
            params.output       = (str + strlen("output="));
         } else if (!strcmp(str, "-help") || !strcmp(str, "help") || !strcmp(str, "?")) {
            printUsage(argc, argv);
            
#ifdef MILTON_ARCH_WINDOWS
            // pause before exiting on windows
            std::cerr << std::endl << "Press enter to exit" << std::endl;
            char dummy[256];
            std::cin.getline(dummy, 256);
#endif
            
            exit(0);
         } else {
            ResourceManager::log.error << "invalid option '" << 
               argv[index] << endl;
            
            return false;
         }
      }
   }
   
   ResourceManager::log.setDebug(params.verbose);
   
#ifdef Q_WS_X11
   if (NULL == getenv("DISPLAY")) {
      if (params.enableGui) {
         ResourceManager::log.error << 
            "error: environment doesn't support Qt gui \"DISPLAY\"" << endl;
         
         params.enableGui = false;
      }
   }
#endif
   
   if (!params.enableGui) {
      if (!params.startRender) {
         ResourceManager::log.info << 
            "rendering will start immediately in 'nox' mode" << endl;
         
         params.startRender = true;
      }
   }
   
   if (0 == params.scenefiles.size()) {
      ResourceManager::log.info << 
         "no scenefile specified; loading default '" << 
         DEFAULT_SCENEFILE << "'" << endl;
      
      params.scenefiles.push_back(DEFAULT_SCENEFILE);
   }
   
   ResourceManager::insert<bool>("enableGui",    params.enableGui);
   ResourceManager::insert<bool>("forcePreview", params.forcePreview);
   
   return true;
}

RenderOutput *MiltonApp::_getOutput(MiltonParams &params) {
   const std::string &output = params.output;
   const std::string &suffix = output.substr(output.rfind('.') + 1);
   
   if (suffix == "png"  || suffix == "jpg" || suffix == "gif" || 
       suffix == "jpeg" || suffix == "tiff")
   {
      return new FileRenderOutput(NULL, output);
   } else {
      ResourceManager::log.error << 
         "error: unrecognized outputfile suffix '" << suffix << "'" <<
         "for output file '" << output << "'" << endl;
      
      return NULL;
   }
}

}

