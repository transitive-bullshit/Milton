/**<!-------------------------------------------------------------------->
   @class  SpectralSampleSet
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Templated set of spectral values (wavelength-dependeht), sampled at N 
   distinct wavelengths. Specific SpectralSampleSet instances include 
   SpectralRadianceSet, in which each sample represents the radiance at a single
   wavelength with units Watts / (m^2 sr), and SpectralScalarSet, where each 
   wavelength-dependent sample represents a unitless scalar that can be used 
   to attenuate SpectralRadianceSet.
   
   @note
      Throughout Milton, currently only RGB spectra are used, but this 
   abstraction is in place and used ubiquitously s.t. we can eventually come 
   back and implement spectrally-aware rendering with the majority of the 
   changes taking place in this one class.
   <!-------------------------------------------------------------------->**/

#ifndef SPECTRAL_SAMPLE_SET_H_
#define SPECTRAL_SAMPLE_SET_H_

#include <common/common.h>
#include <common/image/Rgba.h>

#include <ostream>
#include <limits>

#define NO_DEFAULT_WAVELENGTHS   3

namespace milton {

struct MILTON_DLL_EXPORT SpectralSample {
   /// point-sampled, wavelength-dependent value, with generic units that are 
   /// dependent on the user's interpretation
   real_t   value;
   
   /// wavelength of this spectral sample in nm
   /// note: we're restricting simulation to integer wavelengths to allow 
   /// array indexing by wavelength (for efficiency reasons)
   unsigned wavelength;
   
   inline SpectralSample(real_t value_, unsigned wavelength_)
      : value(value_), wavelength(wavelength_)
   { }
   
   inline SpectralSample(const SpectralSample &r)
      : value(r.value), wavelength(r.wavelength)
   { }
   
   inline SpectralSample()
   { }
   
   inline bool operator==(const SpectralSample &r) const {
      return (wavelength == r.wavelength && EQ(value, r.value));
   }
   
   inline bool operator!=(const SpectralSample &r) const {
      return (wavelength != r.wavelength || NEQ(value, r.value));
   }
   
   inline operator real_t() const {
      return value;
   }
};

class MILTON_DLL_EXPORT SpectralSampleSet {
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      /// Constructs a SpectralSampleSet sampled at the three canonical RGB 
      /// wavelengths, with the sample values specified in the given 3-
      /// element array
      inline explicit SpectralSampleSet(const real_t *data);
      
      /// Constructs a SpectralSampleSet sampled at the three canonical RGB 
      /// wavelengths, with zero values
      inline explicit SpectralSampleSet();
      
      /// Constructs a SpectralSampleSet sampled at the three canonical RGB 
      /// wavelengths, with the sample values given
      inline explicit SpectralSampleSet(const real_t &r, const real_t &g, 
                                        const real_t &b);
      
      /// Constructs a SpectralSampleSet sampled at the three canonical RGB 
      /// wavelengths, with the sample values specified in the given 3-
      /// element vector
      inline SpectralSampleSet(const Vector3 &v);
      
      /// Constructs a SpectralSampleSet sampled at the three canonical RGB 
      /// wavelengths, with the sample values specified by the given RgbaHDR
      inline SpectralSampleSet(const RgbaHDR &rgba);
      
      /// Copy Constructor
      inline SpectralSampleSet(const SpectralSampleSet &v);
      
      ~SpectralSampleSet();
      
      
      //@}-----------------------------------------------------------------
      ///@name Static convenience constructors to generate common spectra
      //@{-----------------------------------------------------------------
      
      /// Generates a SpectralSampleSet sampled at the three canonical RGB 
      /// wavelengths, filled with zeros
      static inline SpectralSampleSet black() {
         return SpectralSampleSet();
      }
      
      /// Generates a SpectralSampleSet sampled at the three canonical RGB 
      /// wavelengths, filled with ones (multiplicative identity)
      static inline SpectralSampleSet identity() {
         return SpectralSampleSet::fill(1);
      }
      
      /// Generates a SpectralSampleSet sampled at the three canonical RGB 
      /// wavelengths, filled with the specified value
      static inline SpectralSampleSet fill(const real_t &value) {
         SpectralSampleSet ret;
         
         for(unsigned i = NO_DEFAULT_WAVELENGTHS; i--;)
            ret[i].value = value;
         
         return ret;
      }
      
      
      //@}-----------------------------------------------------------------
      ///@name Accessor Operators
      //@{-----------------------------------------------------------------
      
      /// @returns a reference to the element at the given index
      inline const SpectralSample &operator[](const unsigned index) const;
      
      /// @returns a reference to the element at the given index
      /// @note changes to the returned element will affect this spectrum
      inline       SpectralSample &operator[](const unsigned index);
      
      /// @returns the number of spectral samples contained in this set
      inline unsigned getN() const;
      
      
      //@}-----------------------------------------------------------------
      ///@name Equality Operators
      //@{-----------------------------------------------------------------
      
      inline       bool       operator==(const SpectralSampleSet &v) const;
      inline       bool       operator!=(const SpectralSampleSet &v) const;
      
      
      //@}-----------------------------------------------------------------
      ///@name Relational Operators
      //@{-----------------------------------------------------------------
      
      inline       bool       operator>=(const SpectralSampleSet &v) const;
      inline       bool       operator<=(const SpectralSampleSet &v) const;
      
      
      //@}-----------------------------------------------------------------
      ///@name Mutator Operators
      //@{-----------------------------------------------------------------
      
      inline       SpectralSampleSet &operator =(const SpectralSampleSet &v);
      inline       SpectralSampleSet &operator+=(const SpectralSampleSet &rhs);
      inline       SpectralSampleSet &operator-=(const SpectralSampleSet &rhs);
      inline       SpectralSampleSet &operator*=(const SpectralSampleSet &rhs);
      
      
      //@}-----------------------------------------------------------------
      ///@name Scalar Mutator Operators
      //@{-----------------------------------------------------------------
      
      inline       SpectralSampleSet &operator*=(const real_t &scale);
      inline       SpectralSampleSet &operator/=(const real_t &scale);
      
      
      //@}-----------------------------------------------------------------
      ///@name Arithmetic Operators
      //@{-----------------------------------------------------------------
      
      inline       SpectralSampleSet  operator+ (const SpectralSampleSet &rhs) const;
      inline       SpectralSampleSet  operator- (const SpectralSampleSet &rhs) const;
      
      /// @returns element-wise multiplication
      inline       SpectralSampleSet  operator* (const SpectralSampleSet &rhs) const;
      /// @returns element-wise division
      inline       SpectralSampleSet  operator/ (const SpectralSampleSet &rhs) const;
      
      
      
      //@}-----------------------------------------------------------------
      ///@name Scalar Arithmetic Operators
      //@{-----------------------------------------------------------------
      
      inline       SpectralSampleSet  operator* (const real_t &scale) const;
      inline       SpectralSampleSet  operator/ (const real_t &scale) const;
      
      
      //@}-----------------------------------------------------------------
      ///@name Misc Functionality
      //@{-----------------------------------------------------------------
      
      /// @returns whether or not all samples in this set are zero
      inline bool isZero() const;
      
      /// @returns the sum of the components in this SpectralSampleSet
      inline real_t getSum() const;
      
      /// @returns the average of the components in this SpectralSampleSet
      inline real_t getAverage() const;
      
      /// @returns dimension (0,1,...,N) of maximum value
      inline unsigned getMaxSample() const;
      
      /// @returns dimension (0,1,...,N) of minimum value
      inline unsigned getMinSample() const;
      
      
      //@}-----------------------------------------------------------------
      ///@name Conversions
      //@{-----------------------------------------------------------------
      
      /// @returns this SpectralSampleSet converted to RGB format
      inline RgbaHDR getRGB() const;
      
      /// @returns this SpectralSampleSet as a HDR floating-point rgba 
      ///    struct (implicit conversion)
      inline operator RgbaHDR() const;
      
      
      //@}-----------------------------------------------------------------
      
   private:
      SpectralSample *m_data;
      unsigned        m_n;
      
      static unsigned s_defaultWavelengths[3];
};


///@name Extra operators where SpectralSampleSet is on right-hand side
//@{---------------------------------------------------------------------

/// @returns the @p v, with each spectral sample scaled by @p scale
MILTON_DLL_EXPORT
   inline SpectralSampleSet operator* (const real_t &scale, 
                                          const SpectralSampleSet &v);

/// Prints a SpectralSampleSet to an output stream
MILTON_DLL_EXPORT
   inline std::ostream &operator<<(std::ostream &os, 
                                   const SpectralSampleSet &v);

//@}---------------------------------------------------------------------

}

/* include inline implementations */
#include <utils/SpectralSampleSet.inl>

namespace milton {

/// spectral radiance with units Watts / (m^2 sr)
typedef SpectralSample SpectralRadiance;

/// set of spectral radiance samples, where each sample represents the radiance 
/// at a single wavelength with units Watts / (m^2 sr)
typedef SpectralSampleSet SpectralRadianceSet;

}

#endif // SPECTRAL_SAMPLE_SET_H_

