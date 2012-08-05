/**<!-------------------------------------------------------------------->
   @class  FileRenderOutput
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Reconstructs an output image from point samples, writing the resulting 
   image out to a file periodically and upon completion of rendering.
   <!-------------------------------------------------------------------->**/

#ifndef FILE_RENDER_OUTPUT_H_
#define FILE_RENDER_OUTPUT_H_

#include <renderers/outputs/ReconstructionRenderOutput.h>

namespace milton {

class MILTON_DLL_EXPORT FileRenderOutput : public ReconstructionRenderOutput {
   
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      inline FileRenderOutput(Image *output, const std::string &fileName)
         : ReconstructionRenderOutput(output), m_fileName(fileName), 
           m_lastSave(0), m_savePeriod(5)
      { }
      
      virtual ~FileRenderOutput()
      { }
      
      
      //@}-----------------------------------------------------------------
      ///@name Initialization
      //@{-----------------------------------------------------------------
      
      virtual void init();
      virtual void finalize();
      
      
      //@}-----------------------------------------------------------------
      ///@name Main usage interface
      //@{-----------------------------------------------------------------
      
      virtual void addSample(PointSample &sample);
      
      
      //@}-----------------------------------------------------------------
      ///@name Accessors / Mutators
      //@{-----------------------------------------------------------------
      
      inline const std::string &getFileName() const {
         return m_fileName;
      }
      
      inline void setFileName(const std::string &fileName) {
         m_fileName = fileName;
      }
      
      
      //@}-----------------------------------------------------------------
      
   protected:
      virtual bool _save(const std::string &fileName);
      
   protected:
      std::string m_fileName;
      
      unsigned    m_lastSave;
      unsigned    m_savePeriod;
};

}

#endif // FILE_RENDER_OUTPUT_H_

