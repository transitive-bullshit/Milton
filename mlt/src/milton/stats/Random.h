/**<!-------------------------------------------------------------------->
   @class  Random
   @author travis fischer (fisch0920@gmail.com)
   @author matthew jacobs (jacobs.mh@gmail.com)
   @date   fall 2008
   
   @brief
      Provides static functionality for generating base random numbers, 
   wrapping around boost::random, shared by all Samplers
   <!-------------------------------------------------------------------->**/

#ifndef RANDOM_H_
#define RANDOM_H_

#include <boost/random.hpp>
#include <ctime>

#include <stats/Event.h>
#include <stats/Sampler.h>

namespace milton {

struct MILTON_DLL_EXPORT Random {
   
   ///@name Typedefs for boost's generator/distribution interface
   //@{-----------------------------------------------------------------
   
   typedef boost::mt19937 Generator;
   
   typedef boost::normal_distribution<real_t> NormalDist;
   typedef boost::variate_generator<Generator&, NormalDist > 
      BoostNormalSampler;
   
   typedef boost::uniform_real<real_t> ContUniformDist;
   typedef boost::variate_generator<Generator&, ContUniformDist > 
      BoostContUniformSampler;
   
   typedef boost::uniform_int<int> DiscreteUniformDist;
   typedef boost::variate_generator<Generator&, DiscreteUniformDist > 
      BoostDiscreteUniformSampler;
   
   typedef boost::exponential_distribution<real_t> ExponentialDist;
   typedef boost::variate_generator<Generator&, ExponentialDist > 
      BoostExponentialSampler;
   
   typedef boost::uniform_on_sphere<real_t> UniformOnSphereDist;
   typedef boost::variate_generator<Generator&, UniformOnSphereDist > 
      BoostUniformOnSphereSampler;
   
   //@}-----------------------------------------------------------------
   ///@name Static utility methods
   //@{-----------------------------------------------------------------
   
   /**
    * @brief
    *    Initializes the Random library routines; must be called before 
    * using any functionality of the Milton random number generators 
    */
   static void init(unsigned seed = 0) {
      if (seed > 0)
         s_seed = seed;
      
      s_generator.seed(s_seed);
      srand(s_seed);
      
      ASSERT(s_contUniformSampler);
      s_contUniformSampler->init();
   }
   
   /**
    * @returns a random floating point number inbetween the specified 
    *    bounds [@p min, @p max)
    */
   static real_t sample(real_t min = 0, real_t max = 1);
   
   /**
    * @returns a random floating point number inbetween the specified 
    *    bounds [@p min, @p max)
    */
   static inline real_t uniform(real_t min = 0, real_t max = 1) {
      return sample(min, max);
   }
   
   /**
    * @returns a random integer inbetween the specified bounds [@p min, @p max)
    */
   static int sampleInt(int min, int max) {
      return (int) floor(Random::sample(min, max));
   }
   
   /**
    * @returns a random index from the cdf given according to the cumulative 
    *    distribution function
    */
   static unsigned sampleCDF(const real_t *cdf, unsigned n) {
      ASSERT(cdf);
      ASSERT(n > 0);
      
      const real_t x = Random::sample(0.0, 1.0);
      real_t total   = 0;
      real_t sum     = 0;
      
      for(unsigned i = 0; i < n; ++i) {
         ASSERT(cdf[i] >= 0);
         total += cdf[i];
      }
      
      ASSERT(total > 0);
      for(unsigned i = 0; i < n - 1; ++i) {
         sum += cdf[i] / total;
         
         if (x <= sum)
            return i;
      }
      
      return (n - 1);
   }
   
   /**
    * @returns a random index from the cdf given according to the cumulative 
    *    distribution function
    * 
    * @note assumes the given cdf is already normalized
    */
   static unsigned sampleNormalizedCDF(const real_t *cdf, unsigned n) {
      ASSERT(cdf);
      ASSERT(n > 0);
      
      const real_t x = Random::sample(0.0, 1.0);
      real_t sum     = 0;
      
      for(unsigned i = 0; i < n - 1; ++i) {
         sum += cdf[i];
         
         if (x <= sum)
            return i;
      }
      
      return (n - 1);
   }
   
   //@}-----------------------------------------------------------------
   ///@name Static Utilities
   //@{-----------------------------------------------------------------
   
   static Generator s_generator;
   static unsigned  s_seed;
   
   //@}-----------------------------------------------------------------
   
   private:
      static Sampler *s_contUniformSampler;
};

}

#endif // RANDOM_H_

