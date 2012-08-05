/**<!-------------------------------------------------------------------->
   @file   MLTAggregatePathMutation.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Spring 2009
   
   @brief
      Aggregation of one or more sub-MLTPathMutations under one coherent 
   interface. Used to choose between bidirectional, lens subpath, and 
   perturbation mutations during Metropolis transitions in MLTMarkovProcess.
   <!-------------------------------------------------------------------->**/

#include "MLTAggregatePathMutation.h"
#include <ResourceManager.h>
#include <Random.h>
#include <QtCore/QtCore>
#include <mlt.h>

namespace milton {

MLTAggregatePathMutation::~MLTAggregatePathMutation() {
   for(unsigned i = m_mutations.size(); i--;)
      safeDelete(m_mutations[i]);
}

void MLTAggregatePathMutation::init() {
   ASSERT(m_renderer);
   
   { // cleanup any possible old mutations
      for(unsigned i = m_mutations.size(); i--;)
         safeDelete(m_mutations[i]);
      
      m_mutations.clear();
      m_weights.clear();
   }
   
   { // TODO: add more mutations and make customizable
      real_t mltBidirPathMutationProb = 
         m_renderer->getValue<real_t>("mltBidirPathMutationProb", 1.0);
      real_t mltLensSubpathMutationProb = 
         m_renderer->getValue<real_t>("mltLensSubpathMutationProb", 1.0);
      real_t mltPerturbationPathMutationProb = 
         m_renderer->getValue<real_t>("mltPerturbationPathMutationProb", 1.0);
      
      mltBidirPathMutationProb = ABS(mltBidirPathMutationProb);
      mltLensSubpathMutationProb = ABS(mltLensSubpathMutationProb);
      mltPerturbationPathMutationProb = ABS(mltPerturbationPathMutationProb);
      
      if (mltBidirPathMutationProb + 
          mltLensSubpathMutationProb + 
          mltPerturbationPathMutationProb <= EPSILON)
      {
         mltBidirPathMutationProb = 1;
         mltLensSubpathMutationProb = 1;
         mltPerturbationPathMutationProb = 1;
      }
      
      /*ResourceManager::log.info 
         << "mltBidirPathMutationProb:        " << mltBidirPathMutationProb << endl 
         << "mltLensSubpathMutationProb:      " << mltLensSubpathMutationProb << endl 
         << "mltPerturbationPathMutationProb: " << mltPerturbationPathMutationProb << endl;*/
      
      if (mltBidirPathMutationProb > 0) {
         m_mutations.push_back(new MLTBidirPathMutation(m_renderer));
         m_weights.push_back(mltBidirPathMutationProb);
      }
      
      if (mltLensSubpathMutationProb > 0) {
         m_mutations.push_back(new MLTLensSubpathMutation(m_renderer));
         m_weights.push_back(mltLensSubpathMutationProb);
      }
      
      if (mltPerturbationPathMutationProb > 0) {
         m_mutations.push_back(new MLTPerturbationPathMutation(m_renderer));
         m_weights.push_back(mltPerturbationPathMutationProb);
      }
   }
   
   { // initialize all mutations
      ASSERT(m_mutations.size() == m_weights.size());
      
      for(unsigned i = m_mutations.size(); i--;)
         m_mutations[i]->init();
   }
   
   { // normalize cdf weights
      real_t sum = 0;
      for(unsigned i = m_weights.size(); i--;)
         sum += m_weights[i];
      
      ASSERT(sum > 0);
      for(unsigned i = m_weights.size(); i--;)
         m_weights[i] /= sum;
   }
}

unsigned s_ind = 0;
real_t MLTAggregatePathMutation::mutate(const Path &X, Path &Y) {
   ASSERT(m_mutations.size() == m_weights.size());
   
   unsigned index = 
      Random::sampleNormalizedCDF(&m_weights[0], m_weights.size());
   
/*#warning "TESTING... REMOVE"
   if (X.length() == 5 && X[1].bsdf->isSpecular()&&!X[2].bsdf->isSpecular()&&X[3].bsdf->isSpecular())
      index = 1;
#warning "TESTING... REMOVE"*/
   
   s_ind = index;
   return m_mutations[index]->mutate(X, Y);
}

#if 0
real_t MLTAggregatePathMutation::getPd(const Event &path) {
   real_t pd = 0;
   
   const unsigned index = path.getMetadata<unsigned>();
   ASSERT(index < m_mutations.size());
   
   return m_mutations[index]->getPd(path);
   
   //for(unsigned i = m_mutations.size(); i--;)
      //pd += m_weights[i] * m_mutations[i]->getPd(path);
   
   return pd;
}

void MLTAggregatePathMutation::setPath(const Path &path) {
   MLTPathMutation::setPath(path);
   
   for(unsigned i = m_mutations.size(); i--;)
      m_mutations[i]->setPath(path);
}

#endif

}

