/**<!-------------------------------------------------------------------->
   @file   Random.cpp
   @author travis fischer (fisch0920@gmail.com)
   @author matthew jacobs (jacobs.mh@gmail.com)
   @date   fall 2008
   
   @brief
      Provides static functionality for generating base random numbers, 
   wrapping around boost::random, shared by all Samplers
   <!-------------------------------------------------------------------->**/

#include "Random.h"
#include "ContUniformSampler.h"
#include "DiscreteUniformSampler.h"
#include <ResourceManager.h>
#include <QtCore/QtCore>
#include <sstream>

namespace milton {

// initialize static members of class Random
Random::Generator  Random::s_generator;
//Random::NormalDist Random::s_normalDist;
//Random::NormalGen  Random::s_normalGen(Random::s_generator, Random::s_normalDist);
unsigned Random::s_seed = static_cast<unsigned>(std::time(0));

Sampler *Random::s_contUniformSampler = new ContUniformSampler(0, 1);

real_t Random::sample(real_t min, real_t max) {
#if 0
   ostringstream oss;
   oss << "random::" << QThread::currentThreadId();
   const std::string &key = oss.str(); 
   
   Sampler *sampler = 
      ResourceManager::getValueThreadLocal<Sampler*>(key, NULL);
   
   if (NULL == sampler) {
      sampler = new ContUniformSampler(0, 1);
      sampler->init();
      ResourceManager::insertThreadLocal<Sampler*>(key, sampler);
   }
   
   const real_t x = sampler->sample();
#else
   const real_t x = s_contUniformSampler->sample();
#endif
   
   ASSERT(x >= 0.0 && x < 1.0);
   
   const real_t r = (x * (max - min)) + min;
   if (r < min || r >= max)
      abort();
   
   return r;
}

}

