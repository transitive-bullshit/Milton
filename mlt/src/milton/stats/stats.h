/**<!-------------------------------------------------------------------->
   @file   stats.h
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Convenience header which includes all statistics package headers
   <!-------------------------------------------------------------------->**/

#ifndef MILTON_STATS_H_
#define MILTON_STATS_H_

// only including joint headers because each one includes the more abstract 
// single variable header respectively
#include <stats/JointEvent.h>

// static wrapper around boost awkward distribution/generator interface
#include <stats/Random.h>

// concrete distributions
#include <stats/samplers/ContUniformSampler.h>
#include <stats/samplers/DiscreteUniformSampler.h>
#include <stats/samplers/EmitterSampler.h>
#include <stats/samplers/ExponentialSampler.h>
#include <stats/samplers/JointSampler.h>
#include <stats/samplers/NormalSampler.h>
#include <stats/samplers/UniformOnSphereSampler.h>
#include <stats/samplers/UniformSampler.h>

#endif // MILTON_STATS_H_

