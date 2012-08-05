/**<!-------------------------------------------------------------------->
   @file   PhotonTracer.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @author Matthew Jacobs (jacobs.mh@gmail.com)
   @date   Fall 2008

   @brief
      Photon tracer which traces paths of 'photons' from light sources 
   throughout the scene, terminating via Russian Roulette
   <!-------------------------------------------------------------------->**/

#include "PhotonTracer.h"
#include <renderers/utils/Path.h>
#include <materials/BSDF.h>
#include <QtCore>

void PhotonTracer::generateL(Path &light, 
                             const PhotonTracerParams &params) const
{
   light.clear();
   
   do {
      const bool roulette = (light.length() >= 3);
      
      if (!light.append(roulette))
         break;
      
      if (light.length() == 1)
         continue;
      
      const PathVertex &v = light.back();
      if (params.caustic) {
         if (!v.bsdf->isSpecular())
            break;
      } else {
         if (v.bsdf->isSpecular())
            break;
      }
      
      /*const std::string &pathNotation = light.toHeckbertNotation();
      if (!_isPotentialMatch(params.pathNotation, pathNotation)) {
         light.pop_back();
         break;
      }*/
   } while(1);
}

/*bool PhotonTracer::_isPotentialMatch(const boost::regex &e, 
                                     const std::string &s) const
{
   boost::match_results<std::string::const_iterator> what;
   
   return (boost::regex_match(s, what, e, 
                              boost::match_default | 
                              boost::match_partial));
}*/

