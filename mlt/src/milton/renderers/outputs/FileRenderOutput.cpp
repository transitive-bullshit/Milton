/**<!-------------------------------------------------------------------->
   @file   FileRenderOutput.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Reconstructs an output image from point samples, writing the resulting 
   image out to a file periodically and upon completion of rendering.
   <!-------------------------------------------------------------------->**/

#include "FileRenderOutput.h"
#include "PointSample.h"

#include <ResourceManager.h>
#include <SpectralSampleSet.h>
#include <Renderer.h>
#include <QtCore/QtCore>
using namespace std;

namespace milton {

void FileRenderOutput::init() {
   ReconstructionRenderOutput::init();
   
   m_lastSave   = 0;
   m_savePeriod = getValue<unsigned>("savePeriod", 5u);
}

void FileRenderOutput::addSample(PointSample &sample) {
   ReconstructionRenderOutput::addSample(sample);
   bool save = sample.save;
   
   const unsigned seconds = m_parent->getTimer().elapsed();
   save |= (m_isMLT && seconds >= m_lastSave + m_savePeriod);
   
   // save an intermediate, temporary render
   if (sample.save) {
      const std::string &fileName = std::string(".temp") + m_fileName;
      //cout << "saving temp render to '" << fileName << "'" << endl;
      
      _save(fileName);
   }
}

void FileRenderOutput::finalize() {
   ReconstructionRenderOutput::finalize();
   
   if (!_save(m_fileName)) {
      ResourceManager::log.error << "error saving rendered results to file '" 
         << m_fileName << "'" << endl;
   } else {
      ResourceManager::log.info  << "saved rendered results to file '"
         << m_fileName << "'" << endl;
   }
}

bool FileRenderOutput::_save(const std::string &fileName) {
   RgbaImage *finalizedOutput = getFinalizedOutput();
   const unsigned seconds     = m_parent->getTimer().elapsed();
   bool success = false;
   
   m_lastSave = seconds;
   
   if (finalizedOutput) {
      success = finalizedOutput->save(fileName);
      
      safeDelete(finalizedOutput);
   } else if (m_output) {
      success = m_output->save(fileName);
   }
   
   if (!success)
      cerr << "\terror saving temp render to '" << fileName << "'" << endl;
   
   return success;
}

}

