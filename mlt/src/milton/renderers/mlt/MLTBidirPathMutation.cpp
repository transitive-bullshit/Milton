/**<!-------------------------------------------------------------------->
   @file   MLTBidirPathMutation.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Spring 2009
   
   @brief
      Bidirectional mutations are the foundation of the MLT algorithm and are 
   responsible for handling large mutations, such as modifying a path's length.
   This is achieved by deleting a (possibly empty) subpath of the current path 
   and replacing it by a (possibly empty) different subpath.
   
   @see p. 345-350 of Veach's thesis (section 11.4.2)
   <!-------------------------------------------------------------------->**/

#include "MLTBidirPathMutation.h"
#include <QtCore/QtCore>
#include <mlt.h>

namespace milton {

#define PD1_0 (0.25)
#define PD1_1 (0.5)
#define PA1_0 (0.5)
#define PA1_1 (0.2)

//#define TEST_CASE_4_5


// follows p. 345-350 of Veach's thesis fairly closely
real_t MLTBidirPathMutation::mutate(const Path &X, Path &Y) {
   const unsigned n = X.length();
   unsigned kd = 0, ka = 0; // lengths of subpaths to delete and add
   real_t pd1 = 1, pd2 = 1; // deletion probabilities
   real_t pa1 = 1, pa2 = 1; // addition probabilities
   unsigned lP, mP;         // new light and eye subpath lengths
   int l, m;                // exclusive indices marking subpath to delete
   
   { // handle subpath deletion
      { // choose a subpath length to delete
         real_t e1 = Random::sample(0, 1);
         const real_t p0 = PD1_0, p1 = PD1_1;
         
         if (e1 < p0) {
            kd  = 0;
            pd1 = p0;
         } else if (e1 < p0 + p1) {
            kd  = 1;
            pd1 = p1;
         } else {
            real_t p = p0 + p1;
            kd  = 1;
            
            do {
               ++kd;
               
               pd1 = (1.0 / (1UL << (kd + 1)));
               p  += pd1;
            } while(e1 - p > 0 && kd < n);
            
            if (kd == n && p < 1)
               pd1 += (1 - p);
         }
         
         ASSERT(kd <= n);
      }
      
#ifdef TEST_CASE_4_5
      kd  = 0;
      pd1 = PD1_0;
#endif
      
      { // choose a specific subpath of length kd to delete
         // l ranges from [-1, n - kd - 1]
         l = Random::sampleInt(-1, n - kd);
         m = l + kd + 1;
         
#ifdef TEST_CASE_4_5
         l = 1;
         m = 2;
#endif
         
         ASSERT(l >= -1 && l <= (signed) (n - 1) && l <= (signed) (n - kd - 1));
         ASSERT(m >= 0  && m <= (signed) (n));
         
         pd2 = _getPd2(X, kd, l);
      }
      
#ifdef DEBUG
      const real_t _pd1 = _getPd1(X, kd);
      const real_t _pd2 = _getPd2(X, kd, l);
      ASSERT(_pd1 == pd1 && _pd2 == pd2);
#endif
   }
   
   { // handle new subpath addition
      { // choose a subpath length to add
         real_t e2 = Random::sample(0, 1);
         const real_t p0 = PA1_0, p1 = PA1_1;
         
         if (e2 < p0) {
            ka  = kd;
            pa1 = p0;
         } else {
            e2 -= p0;
            
            if (e2 < p1) {
               ka  = kd + 1;
               pa1 = p1;
            } else {
               e2 -= p1;
               bool found = false;
               
               if (kd >= 1) {
                  if (e2 < p1) {
                     ka  = kd - 1;
                     pa1 = p1;
                     
                     found = true;
                  } else {
                     e2 -= p1;
                  }
               }
               
               if (!found) {
                  unsigned k = 1;
                  
                  do {
                     ++k;
                     
                     ka  = kd + k;
                     pa1 = 0.2 * (1.0 / (1UL << (k)));
                     
                     if (kd >= k) {
                        if (e2 < pa1)
                           ka = kd - k;
                        
                        e2 -= pa1;
                     }
                     
                     e2 -= pa1;
                  } while(e2 > 0);
               }
            }
         }
         
         ASSERT(ka >= 0);
      }
      
#ifdef TEST_CASE_4_5
      ka  = 1;
      pa1 = PA1_1;
#endif
      
      { // choose lengths of light and eye subpaths to add
         lP = Random::sampleInt(0, ka + 1);
         mP = ka - lP;
         
#ifdef TEST_CASE_4_5
         lP = 1;
         mP = 0;
#endif
         
         pa2 = _getPa2(X, kd, ka, lP);
      }
      
#ifdef DEBUG
      const real_t _pa1 = _getPa1(X, kd, ka);
      const real_t _pa2 = _getPa2(X, kd, ka, lP);
      ASSERT(_pa1 == pa1 && _pa2 == pa2);
#endif
   }
   
   if (kd >= n && ka == 0) // empty path
      return 0;         // mutation failed
   
   ASSERT(pd1 > 0 && pd2 > 0);
   ASSERT(pa1 > 0 && pa2 > 0);
   
   Y          = X.left (l + 1);
   Path right = X.right(n - m);
   ASSERT(n - (Y.length() + right.length()) == kd);
   
   { // add light and eye subpaths and join together
      for(unsigned i = 0; i < lP; ++i) {
         if (!Y.append(false))
            return 0;   // mutation failed
      }
      
      /*while(!Y.empty() && Y.back().bsdf->isSpecular()&&ka < 40) {
         ++ka;
         ++lP;
         
         if (!Y.append(false))
            return 0;   // mutation failed
      }*/
      
      for(unsigned i = 0; i < mP; ++i) {
         if (!right.prepend(false))
            return 0;   // mutation failed
      }
      
      /*while(!right.empty() && right.front().bsdf->isSpecular()&&ka < 40) {
         ++ka;
         ++mP;
         
         if (!right.prepend(false))
            return 0;   // mutation failed
      }*/
      
      if (!Y.append(right))
         return 0;      // mutation failed
   }
   
   ASSERT(Y.length() == n - kd + ka);
   
   if (!Y.front().isEmitter() || !Y.back().isSensor())
      return 0;         // mutation failed
   
   real_t pd  = pd1 * pd2;
   real_t Rxy = _getMutationR(X, Y, kd, ka, pd, pa1, l, m);
   real_t Ryx = 1;
   
   {
      m = (m + ((int)ka)) - ((int)kd);
      unsigned ktemp = kd;
      kd = ka;
      ka = ktemp;
      
      pd1 = _getPd1(Y, kd);
      pd2 = _getPd2(Y, kd, l);
      pd  = pd1 * pd2;
      
      pa1 = _getPa1(Y, kd, ka);
      Ryx = _getMutationR(Y, X, kd, ka, pd, pa1, l, m);
   }
   
// TODO:  #warning "TESTING"
   const real_t Fx = X.getRadiance().getRGB().luminance();
   if (Fx == 0||Rxy==0)
      return 0;
   const real_t a = ((Y.getRadiance()/X.getRadiance()).getRGB()).luminance() * (Ryx/Rxy);
   return MIN(1.0, a);
// TODO:  #warning "TESTING"
   
   return (Ryx <= 0 ? 0 : MIN(1.0, Rxy / Ryx));
}

#ifdef TEST_CASE_4_5
real_t s_alpha = 0;
#endif

real_t MLTBidirPathMutation::_getMutationR(const Path &X, 
                                           const Path &Y, 
                                           const unsigned kd, 
                                           const unsigned ka, 
                                           const real_t pd, 
                                           const real_t pa1, 
                                           const int l, 
                                           const int m) 
{
   const unsigned n2 = Y.length();
   
#ifdef TEST_CASE_4_5
   if (X.length() == 4 && Y.length() == 5) {
      cerr << X << endl << endl;
      cerr << Y << endl << endl;
      
      cerr << "kd: " << kd << ", ka: " << ka << ", pd: " << pd << ", pa1: " << pa1 << ", l: " << l << ", m: " << m << endl;
      
      SpectralSampleSet Qxy = SpectralSampleSet::identity();
      real_t Txy = pd * (pa1 * _getPa2(X, kd, ka, 0) * Y[3].pdfE * Y[3].GE + pa1 * _getPa2(X, kd, ka, 1) * Y[1].pdfL * Y[1].GL);
      
      real_t f_y = (Y[1].fs * Y[1].GL * Y[2].fs * Y[2].GL * Y[3].fs).getRGB().luminance();
      real_t f_x = (X[1].fs * X[1].GL * X[2].fs).getRGB().luminance();
      
      const real_t div0 = Y.getRadiance().getRGB().luminance() / 
         const_cast<Path&>(X).getRadiance().getRGB().luminance();
      const real_t div1 = f_y / f_x;
      
      cerr << "f(y) / f(x): " << div0 << " vs " << div1 << endl;
      
      Path temp = X;
      X = Y;
      real_t Tyx = _getPd1(X, 1) * _getPd2(X, 1, 1) * _getPa1(X, 1, 0);
      X = temp;
      
      cerr << "T(y|x): " << Txy << ", T(x|y): " << Tyx << ", f_y: " << f_y << ", f_x: " << f_x << endl;
      cerr << "Q(y|x): " << Txy / f_y << endl;
      cerr << "Q(x|y): " << Tyx / f_x << endl;
      cerr << "alpha: " << Tyx * f_y / (Txy * f_x) << endl;
      cerr << endl;
      
      s_alpha = MIN(1, Tyx * f_y / (Txy * f_x));
   }
#endif
   
#if 0
   real_t p = 0;
   for(unsigned lP = 0; lP <= ka; ++lP) {
      const real_t pa = pa1 * _getPa2(X, kd, ka, lP);
      
      const unsigned s = l + lP + 1;
      const unsigned t = n2 - s;
      
      p += pa * 
         (l + ((int)lP) >= 0 ? Y[l + lP].pL : 1) * 
         (m + lP - ka < n2&&m+lP-ka>=0 ? Y[m + lP - ka].pE : 1);
   }
   
   p *= pd / ((l >= 0 &&l<n2? Y[l].pL : 1) * (m < n2&&m>=0 ? Y[m].pE : 1));
   const real_t luminance = Y.getRadiance().getRGB().luminance();
   
   if (luminance == 0)
      return 0;
   return p / luminance;
#endif
   
   const real_t luminance = Y.getRadiance().getRGB().luminance();
   real_t Txy = 0;
   
   if (luminance <= 0)
      return 0;
   
   Path &pathY = const_cast<Path&>(Y);
   
   // handle all possible ways new subpath could've been added
   for(unsigned i = 0; i <= ka; ++i) {
      const real_t pa2 = _getPa2(X, kd, ka, i);
      
      const unsigned s = l + i + 1;
      const unsigned t = n2 - s;
      
      Txy += pa2 * pathY.getPd(s, t);
//TODO:  #warning "TESTING"
//#warning "TESTING"
//#warning "TESTING"
      //Txy += pa2 * (s <= 0 ? 1 : pathY[s - 1].pL) * (s >= n2 ? 1 : pathY[n2 - t].pE);
//#warning "TESTING"
//#warning "TESTING"
//#warning "TESTING"
      
//#warning "TESTING"
//#warning "TESTING"
//#warning "TESTING"
      // NOTE: definitely shouldn't cut out if equal to zero
      //    * only if when i == lP evaluates to zero
      //    * which should only happen if luminance is zero
      //if (pathY.getPd(s, t) <= 0)
      //   return 0;
//#warning "TESTING"
//#warning "TESTING"
//#warning "TESTING"
      
      /*
      const SpectralSampleSet &radiance = const_cast<Path&>(Y).getContribution(s, t);
      const real_t luminance = radiance.getRGB().luminance();
      if (luminance == 0)
         return 0; // invalid mutation
      mutationR += pa2 / luminance;*/
   }
   
   Txy *= pa1 * pd;
   if (Txy <= 0)
      return 0;
   
// #warning "TESTING"
   return Txy;//(luminance / Txy);
}

real_t MLTBidirPathMutation::_getPd1(const Path &X, const unsigned kd) const {
   const unsigned n = X.length();
   ASSERT(kd <= n);
   
   if (kd == 0)
      return PD1_0;
   
   if (kd == 1)
      return PD1_1;
   
   if (kd < n)
      return (1.0 / (1UL << (kd + 1)));
   
   real_t p = 0;
   
   for(unsigned i = 2; i < n; ++i)
      p += (1.0 / (1UL << (i + 1)));
   
   p = 1.0 - (p + PD1_0 + PD1_1);
   if (p < 0)
      return (1.0 / (1UL << (kd + 1)));
   
   ASSERT(p < 1);
   return p;
}

real_t MLTBidirPathMutation::_getPd2(const Path &X, const unsigned kd, 
                                     const signed l) const
{
   const unsigned n = X.length();
   ASSERT(kd <= n);
   ASSERT(l  <= ((signed) n) - 1);
   
   // TODO: incorporate importance sampling
   
   return 1.0 / (n - kd + 1);
}

real_t MLTBidirPathMutation::_getPa1(const Path &X, const unsigned kd, 
                                     const unsigned ka) const
{
   const unsigned diff = ABS(((signed) kd) - ((signed) ka));
   
   if (diff == 0)
      return PA1_0;
   
   if (diff == 1)
      return PA1_1;
   
   return 0.2 * (1.0 / (1UL << (diff)));
}

real_t MLTBidirPathMutation::_getPa2(const Path &, 
                                     const unsigned, const unsigned ka, 
                                     const unsigned) const
{
   return 1.0 / (ka + 1);
}

#if 0
real_t MLTBidirPathMutation::getPd(const Event &event) {
   Path pathX = m_path;
   Path pathY = event.getValue<const Path&>();
   const unsigned n1 = pathX.length();
   const unsigned n2 = pathY.length();
   unsigned kd = 0, ka = 0; // lengths of deleted and added subpaths
   real_t pd1 = 1, pd2 = 1; // deletion probabilities
   real_t pa1 = 1;          // addition probabilities
   real_t pd  = 1;          // cumulative probabilities
   real_t Qxy = 0;          // Q(xy) = Q(y|x) = T(y|x) / f(y) and vice-versa
   int l = 0, m = 0;        // exclusive indices marking deleted subpath
   
   if (n1 < 2 || n2 < 2)
      return 0;
   
   { // handle subpath deletion
      // determine which subpath was deleted
      while(l < (signed) n1 && l < (signed) n2 && 
            pathX[l] == pathY[l])
      {
         ++l;
      }
      
      while(m < (signed) n1 && m < (signed) n2 && 
            pathX[n1 - m - 1] == pathY[n2 - m - 1])
      {
         ++m;
      }
      
      //cerr << "\tl: " << l << ", m: " << m << endl;
      
      l -= 1;
      m  = n1 - m;
      
      if (l >= m) {
         l  = -1;
         m  = 0;
         kd = 0;
      } else {
         kd = m - l - 1;
      }
      
      if (n1 - kd > n2) {
         m  = n2;
         kd = n1 - n2;
      }
      
      ASSERT(n1 - kd <= n2);
      
      pd1 = _getPd1(X, kd);
      pd2 = _getPd2(X, kd, l);
      pd  = pd1 * pd2;
      
      ASSERT(pd1 > 0 && pd2 > 0);
   }
   
   ka  = n2 + kd - n1;
   pa1 = _getPa1(X, kd, ka);
   ASSERT(ka <= n2);
   
   Qxy = _getMutationQ(pathX, pathY, kd, ka, pd, pa1, l, m);
   
#if 1
   return Qxy;
#else
   if (Qxy <= 0)
      return 0;
   
   {
      m_path = pathY;
      
      pd1 = _getPd1(X, ka);
      pd2 = _getPd1(X, ka, l);
      pd  = pd1 * pd2;
      
      pa1 = _getPa1(X, ka, kd);
      Qyx = _getMutationQ(pathY, pathX, ka, kd, pd, pa1, l, m);
      
      m_path = pathX;
   }
   
   return (Qxy <= Qyx ? 1 : Qyx / Qxy);
#endif
}

#endif

}

