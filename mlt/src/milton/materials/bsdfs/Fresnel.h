/**<!-------------------------------------------------------------------->
   @class  Fresnel
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Utility class encapsulating Fresnel's Law which computes the fraction of 
   incident power that is reflected when light travels across an interface 
   defined by the boundary between two media with different indices of 
   refraction.
   
   @note
      The index of refraction (aka refractive index) of a medium is a measure 
   for how much the speed of light (or other waves such as sound waves) is 
   reduced inside the medium (source: Wikipedia).
   <!-------------------------------------------------------------------->**/

#ifndef FRESNEL_H_
#define FRESNEL_H_

#include <common/math/Vector.h>

namespace milton {

class MILTON_DLL_EXPORT Fresnel {
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      /**
       * @brief
       *    Uses Fresnel's Law to compute the fraction of incident power that 
       * is reflected from the given interface in the perfectly specular
       * reflected direction defined by incident vector wi and the interface's 
       * local normal, N.
       * 
       * @param wi is incident vector
       * @param N  is local surface normal
       * @param n1 is index of refraction of medium on incident side of surface
       * @param n2 is index of refraction of medium on opposite side of surface
       * 
       * @returns the reflection coefficient, Fr, of this interaction
       * @note  the transmission coefficient, Fs, is assumed to be (1 - Fr)
       */
      static inline real_t getFr(const Vector3 &wi, const Vector3 &N, 
                                 real_t n1, real_t n2)
      {
         real_t cosTheta = N.dot(wi);
         
         if (cosTheta < 0) // wi incident on front-side of surface
            return Fresnel::getFr(-cosTheta, n1, n2);
         else              // wi incident on back-side  of surface
            return Fresnel::getFr( cosTheta, n2, n1);
      }
      
      /**
       * @brief
       *    Uses Fresnel's Law to compute the fraction of incident power that 
       * is reflected from the given interface at incident angle theta_i with 
       * respect to the interface's local normal.
       * 
       * @param cos_theta_i is cosine of angle theta_i btwn reversed incident 
       *    vector and normal
       * @param n1 is index of refraction of medium on incident side of surface
       * @param n2 is index of refraction of medium on opposite side of surface
       * 
       * @returns the reflection coefficient, Fr, of this interaction
       * @note  the transmission coefficient, Fs, is assumed to be (1 - Fr)
       */
      static inline real_t getFr(real_t cos_theta_i, 
                                 real_t n1, real_t n2)
      {
         real_t snell   = (n1 / n2) * sin(acos(cos_theta_i));
         if (ABS(snell) >= 1)
            return 1; // total internal reflection
         
         const real_t cos_theta_t = cos(asin(snell));
         const real_t n1cosi = n1 * cos_theta_i, n1cost = n1 * cos_theta_t;
         const real_t n2cosi = n2 * cos_theta_i, n2cost = n2 * cos_theta_t;
         
         // s-polarized
         const real_t Rs = (n1cosi - n2cost) / (n1cosi + n2cost);
         
         // p-polarized
         const real_t Rp = (n1cost - n2cosi) / (n1cost + n2cosi);
         
         // assumes incident light is unpolarized (containing an equal mixture 
         // of s- and p-polarizations)
         return (Rs * Rs + Rp * Rp) / 2.0; // Fr
      }
      
      /**
       * @brief
       *    Uses Fresnel's Law to compute the fraction of incident power that 
       * is refracted from the given interface in the perfect refractive
       * direction defined by incident vector wi and the interface's local 
       * normal, N.
       * 
       * @param wi is incident vector
       * @param N  is local surface normal
       * @param n1 is index of refraction of medium on incident side of surface
       * @param n2 is index of refraction of medium on opposite side of surface
       * 
       * @returns the transmission coefficient, Fs, of this interaction
       * @note  the reflection coefficient, Fr, is assumed to be (1 - Fs)
       */
      static inline real_t getFs(const Vector3 &wi, const Vector3 &N, 
                                 real_t n1, real_t n2)
      {
         return (1 - Fresnel::getFr(wi, N, n1, n2));
      }

      //@}-----------------------------------------------------------------
};

}

#endif // FRESNEL_H_

