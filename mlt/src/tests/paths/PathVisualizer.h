/**<!-------------------------------------------------------------------->
   @file   PathVisualizer.h
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Visualizer for randomly-generated bidirectional paths
   <!-------------------------------------------------------------------->**/

#ifndef PATH_VISUALIZER_H_
#define PATH_VISUALIZER_H_

#include <Visualization.h>

using namespace milton;

class IPathGenerator;
class Path;

class PathVisualizer : public InteractionListener {
   public:
      PathVisualizer(OpenGLCanvas *parent);
      
      virtual ~PathVisualizer();
      
      // Callbacks upon user interaction
      virtual void mousePressEvent  (InteractionInfo &info);
      virtual void keyPressEvent    (InteractionInfo &info);
      
      virtual void paintGL();
      virtual void init();
      
   protected:
      virtual void _init();
      virtual void _initL();
      virtual void _initE();
      virtual void _reset();
      
   protected:
      Path *m_light;
      Path *m_eye;
      IPathGenerator *m_generator;
};

#endif // PATH_VISUALIZER_H_

