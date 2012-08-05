/**<!-------------------------------------------------------------------->
   @class  UniformOnSphereSampler
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Represents a uniform distribution on the surface of an N-dimensional 
   unit sphere (with parameter N > 0 and radius = 1)
   <!-------------------------------------------------------------------->**/

#ifndef UNIFORM_ON_SPHERE_SAMPLER_H_
#define UNIFORM_ON_SPHERE_SAMPLER_H_

#include <stats/Sampler.h>

namespace milton {

class MILTON_DLL_EXPORT UniformOnSphereSampler : public Sampler {
   
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      inline explicit UniformOnSphereSampler(unsigned dimension = 3)
         : Sampler(), 
           m_sampler(Random::s_generator, 
                     Random::UniformOnSphereDist(dimension)), 
           m_dimension(dimension)
      { }
      
      inline UniformOnSphereSampler(const UniformOnSphereSampler &copy)
         : Sampler(copy), 
           m_sampler(copy.m_sampler), 
           m_dimension(copy.m_dimension)
      { }
      
      virtual ~UniformOnSphereSampler()
      { }
      
      
      //@}-----------------------------------------------------------------
      ///@name Initialization
      //@{-----------------------------------------------------------------
      
      /**
       * @brief
       *    Should perform any initialization of this random variable which 
       * may be necessary to speed or prepare sampling
       * 
       * @note should be called before calling sample or getPd
       * @note default implementation initialiazes this random variable's 
       *    sample space
       */
      virtual void init();
      
      
      //@}-----------------------------------------------------------------
      ///@name Main usage interface
      //@{-----------------------------------------------------------------
      
      /**
       * @returns a randomly chosen event x, sampled from this random 
       *    variable's sample space; returned Event has underlying type of 
       *    Point3
       * @note consecutive calls to sample are expected to return 
       *    independent, identically distributed (IID) samples
       */
      virtual Event sample();
      
      /**
       * @returns the probability density with which the given event would be 
       *    sampled according to the underlying probability density function
       * @note the given event is assumed to lie within this random variable's
       *    sample space
       */
      virtual real_t getPd(const Event &event);
      
      
      //@}-----------------------------------------------------------------
      ///@name Accessors/Mutators
      //@{-----------------------------------------------------------------
      
      /**
       * @returns the dimension of the sphere, whose surface 
       *    defines the sample space of this random variable
       */
      inline real_t getDimension() const {
         return m_dimension;
      }
      
      /**
       * @returns the radius of the N-dimensional sphere, whose surface 
       *    defines the sample space of this random variable
       */
      inline real_t getRadius() const {
         return 1.0;
      }
      
      //@}-----------------------------------------------------------------
      
   protected:
      unsigned _factorial(unsigned n);
      
   protected:
      Random::BoostUniformOnSphereSampler m_sampler;
      
      unsigned m_dimension;
};

}

#endif // UNIFORM_ON_SPHERE_SAMPLER_H_

