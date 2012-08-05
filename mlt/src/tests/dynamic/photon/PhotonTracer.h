/**<!-------------------------------------------------------------------->
   @class  PhotonTracer
   @author Travis Fischer (fisch0920@gmail.com)
   @author Matthew Jacobs (jacobs.mh@gmail.com)
   @date   Fall 2008

   @brief
      Photon tracer which traces paths of 'photons' from light sources 
   throughout the scene, terminating via Russian Roulette
   <!-------------------------------------------------------------------->**/

#ifndef PHOTON_TRACER_H_
#define PHOTON_TRACER_H_

//#include <boost/regex.hpp>

class Path;

struct PhotonTracerParams {
   /*const boost::regex pathNotation;
   
   inline PhotonTracerParams(const std::string &pathNotation_)
      : pathNotation(pathNotation_)
   { }*/
   
   bool caustic;
   
   inline PhotonTracerParams(bool caustic_)
      : caustic(caustic_)
   { }
};

class PhotonTracer {
   public:
      ///@name Main usage interface
      //@{-----------------------------------------------------------------
      
      /**
       * @brief
       *    Generates a light subpath which is assumed to start at an emitter, 
       * constrained by the given parameters
       */
      virtual void generateL(Path &light, 
                             const PhotonTracerParams &params) const;
      
      
      //@}-----------------------------------------------------------------
      
   protected:
      //virtual bool _isPotentialMatch(const boost::regex &e, 
      //                               const std::string &s) const;
};

#endif // PHOTON_TRACER_H_

