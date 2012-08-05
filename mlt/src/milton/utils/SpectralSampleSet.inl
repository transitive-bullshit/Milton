/**<!-------------------------------------------------------------------->
   @file   SpectralSampleSet.inl
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

#ifndef SPECTRAL_SAMPLE_SET_INL_
#define SPECTRAL_SAMPLE_SET_INL_

#include <iostream>

namespace milton {

// Constructors
// ------------

// Constructs a SpectralSampleSet sampled at the three canonical RGB 
// wavelengths, with the sample values specified in the given 
// 3-element array
inline SpectralSampleSet::SpectralSampleSet(const real_t *data)
   : m_data(new SpectralSample[NO_DEFAULT_WAVELENGTHS]), 
     m_n(NO_DEFAULT_WAVELENGTHS)
{
   for(unsigned i = m_n; i--;) {
      m_data[i].value      = data[i];
      m_data[i].wavelength = SpectralSampleSet::s_defaultWavelengths[i];
   }
}

// Constructs a SpectralSampleSet sampled at the three canonical RGB 
// wavelengths, with zero values
inline SpectralSampleSet::SpectralSampleSet() 
   : m_data(new SpectralSample[NO_DEFAULT_WAVELENGTHS]), 
     m_n(NO_DEFAULT_WAVELENGTHS)
{
   for(unsigned i = m_n; i--;) {
      m_data[i].value      = 0;
      m_data[i].wavelength = SpectralSampleSet::s_defaultWavelengths[i];
   }
}

// Constructs a SpectralSampleSet sampled at the three canonical RGB 
// wavelengths, with the sample values given
inline SpectralSampleSet::SpectralSampleSet(const real_t &r, 
                                            const real_t &g, 
                                            const real_t &b)
   : m_data(new SpectralSample[3]), m_n(3)
{
   m_data[0].value      = r;
   m_data[0].wavelength = SpectralSampleSet::s_defaultWavelengths[0];
   
   m_data[1].value      = g;
   m_data[1].wavelength = SpectralSampleSet::s_defaultWavelengths[1];
   
   m_data[2].value      = b;
   m_data[2].wavelength = SpectralSampleSet::s_defaultWavelengths[2];
}

// Constructs a SpectralSampleSet sampled at the three canonical RGB 
// wavelengths, with the sample values specified in the given 3-
// element vector
inline SpectralSampleSet::SpectralSampleSet(const Vector3 &v)
   : m_data(new SpectralSample[NO_DEFAULT_WAVELENGTHS]), 
     m_n(NO_DEFAULT_WAVELENGTHS)
{
   for(unsigned i = m_n; i--;) {
      m_data[i].value      = v[i];
      m_data[i].wavelength = SpectralSampleSet::s_defaultWavelengths[i];
   }
}

// Constructs a SpectralSampleSet sampled at the three canonical RGB 
// wavelengths, with the sample values specified by the given RgbaHDR
inline SpectralSampleSet::SpectralSampleSet(const RgbaHDR &rgba)
   : m_data(new SpectralSample[NO_DEFAULT_WAVELENGTHS]), 
     m_n(NO_DEFAULT_WAVELENGTHS)
{
   m_data[0].value      = rgba.r;
   m_data[0].wavelength = SpectralSampleSet::s_defaultWavelengths[0];
   
   m_data[1].value      = rgba.g;
   m_data[1].wavelength = SpectralSampleSet::s_defaultWavelengths[1];
   
   m_data[2].value      = rgba.b;
   m_data[2].wavelength = SpectralSampleSet::s_defaultWavelengths[2];
}

// Copy Constructor
inline SpectralSampleSet::SpectralSampleSet(const SpectralSampleSet &v) 
   : m_data(new SpectralSample[v.m_n]), m_n(v.m_n)
{
   for(unsigned i = m_n; i--;)
      m_data[i] = v[i];
}


// Accessor Operators
// ------------------

// @returns a reference to the element at the given index
inline const SpectralSample &SpectralSampleSet::operator[](
   const unsigned index) const
{
   ASSERT(index >= 0 && index < m_n);
   
   return m_data[index];
}

// @returns a reference to the element at the given index
// @note changes to the returned element will affect this spectrum
inline       SpectralSample &SpectralSampleSet::operator[](
   const unsigned index)
{
   ASSERT(index >= 0 && index < m_n);
   
   return m_data[index];
}

// @returns the number of spectral samples contained in this set
inline unsigned SpectralSampleSet::getN() const {
   return m_n;
}


// Equality Operators
// ------------------
inline       bool       SpectralSampleSet::operator==(
   const SpectralSampleSet &v) const
{
   if (m_n != v.m_n)
      return false;
   
   for(unsigned i = m_n; i--;) {
      if (m_data[i] != v[i])
         return false;
   }
   
   return true;
}

inline       bool       SpectralSampleSet::operator!=(
   const SpectralSampleSet &v) const 
{
   return !((*this) == v);
}


// Relational Operators
// ------------------
inline       bool       SpectralSampleSet::operator>=(
   const SpectralSampleSet &v) const
{
   ASSERT(m_n == v.m_n);
   
   for(unsigned i = m_n; i--;) {
      ASSERT(m_data[i].wavelength == v[i].wavelength);
      
      if (!(m_data[i].value >= v[i].value))
         return false;
   }
   
   return true;
}

inline       bool       SpectralSampleSet::operator<=(
   const SpectralSampleSet &v) const 
{
   ASSERT(m_n == v.m_n);
   
   for(unsigned i = m_n; i--;) {
      ASSERT(m_data[i].wavelength == v[i].wavelength);
      
      if (!(m_data[i].value <= v[i].value))
         return false;
   }
   
   return true;
}


// Mutator Operators
// -----------------
inline       SpectralSampleSet &SpectralSampleSet::operator =(
   const SpectralSampleSet &v)
{
   SpectralSample *old = m_data;
   
   m_n    = v.m_n;
   m_data = new SpectralSample[m_n];
   safeDeleteArray(old);
   
   for(unsigned i = m_n; i--;)
      m_data[i] = v[i];
   
   return *this;
}

inline       SpectralSampleSet &SpectralSampleSet::operator+=(
   const SpectralSampleSet &rhs)
{
   ASSERT(m_n == rhs.m_n);
   
   for(unsigned i = m_n; i--;) {
      ASSERT(m_data[i].wavelength == rhs[i].wavelength);
      
      m_data[i].value += rhs.m_data[i].value;
   }
   
   return *this;
}

inline       SpectralSampleSet &SpectralSampleSet::operator-=(
   const SpectralSampleSet &rhs)
{
   ASSERT(m_n == rhs.m_n);
   
   for(unsigned i = m_n; i--;) {
      ASSERT(m_data[i].wavelength == rhs[i].wavelength);
      
      m_data[i].value -= rhs.m_data[i].value;
   }
   
   return *this;
}

inline       SpectralSampleSet &SpectralSampleSet::operator*=(
   const SpectralSampleSet &rhs)
{
   ASSERT(m_n == rhs.m_n);
   
   for(unsigned i = m_n; i--;) {
      ASSERT(m_data[i].wavelength == rhs[i].wavelength);
      
      m_data[i].value *= rhs.m_data[i].value;
   }
   
   return *this;
}


// Scalar Mutator Operators
// ------------------------
inline       SpectralSampleSet &SpectralSampleSet::operator*=(const real_t &scale) {
   for(unsigned i = m_n; i--;)
      m_data[i].value *= scale;
   
   return *this;
}

inline       SpectralSampleSet &SpectralSampleSet::operator/=(const real_t &scale) {
   ASSERT(scale != 0);
   
   for(unsigned i = m_n; i--;)
      m_data[i].value /= scale;
   
   return *this;
}


// Arithmetic Operators
// --------------------

inline       SpectralSampleSet  SpectralSampleSet::operator+ (const SpectralSampleSet &rhs) const
{
   SpectralSampleSet s(*this);
   s += rhs;
   
   return s;
}

inline       SpectralSampleSet  SpectralSampleSet::operator- (const SpectralSampleSet &rhs) const
{
   SpectralSampleSet s(*this);
   s -= rhs;
   
   return s;
}

inline       SpectralSampleSet  SpectralSampleSet::operator/ (const SpectralSampleSet &rhs) const
{
   SpectralSampleSet s(*this);
   ASSERT(m_n == rhs.m_n);
   
   for(unsigned i = m_n; i--;) {
      ASSERT(m_data[i].wavelength == rhs.m_data[i].wavelength);
      ASSERT(rhs.m_data[i].value != 0);
      
      s.m_data[i].value /= rhs.m_data[i].value;
   } 
   
   return s;
}

inline       SpectralSampleSet  SpectralSampleSet::operator* (const SpectralSampleSet &rhs) const
{
   SpectralSampleSet s(*this);
   s *= rhs;
   
   return s;
}


// Scalar Arithmetic Operators
inline       SpectralSampleSet  SpectralSampleSet::operator* (const real_t &scale) const {
   SpectralSampleSet s(*this);
   s *= scale;
   
   return s;
}

inline       SpectralSampleSet  SpectralSampleSet::operator/ (const real_t &scale) const {
   SpectralSampleSet s(*this);
   s /= scale;
   
   return s;
}


// Misc Functionality
// --------------------------

// @returns whether or not all samples in this set are zero
inline bool SpectralSampleSet::isZero() const {
   for(unsigned i = m_n; i--;)
      if (m_data[i].value != 0)
         return false;
   
   return true;
}

// @returns the sum of the components in this SpectralSampleSet
inline real_t SpectralSampleSet::getSum() const {
   real_t sum = 0;
   
   for(unsigned i = m_n; i--;)
      sum += m_data[i].value;
   
   return sum;
}

// @returns the average of the components in this SpectralSampleSet
inline real_t SpectralSampleSet::getAverage() const {
   return getSum() / m_n;
}


// Extra operators where SpectralSampleSet is on right-hand side
// --------------------------------------------------

// @returns the @p v, with each spectral sample scaled by @p scale
inline       SpectralSampleSet  operator* (const real_t &scale, 
                                           const SpectralSampleSet &rhs)
{
   return rhs * scale;
}

// Prints a SpectralSampleSet to an output stream
inline       std::ostream      &operator<<(std::ostream &os, 
                                           const SpectralSampleSet &v)
{
   const unsigned n = v.getN();
   
   os << "{ ";
   for(unsigned i = 0; i < n; ++i) {
      os << v[i].value << " (" << v[i].wavelength << ")" << 
         (i < n - 1 ? ", " : "");
   }
   os << " }";
   
   return os;
}

// @returns dimension (0,1,...,N) of maximum value
inline unsigned SpectralSampleSet::getMaxSample() const {
   real_t maxValue = m_data[0].value;
   unsigned maxIndex = 0;
   
   for(unsigned i = 1; i < m_n; ++i) {
      if (m_data[i].value > maxValue) {
         maxValue = m_data[i].value;
         maxIndex = i;
      }
   }
   
   return maxIndex;
}

// @returns dimension (0,1,...,N) of minimum value
inline unsigned SpectralSampleSet::getMinSample() const {
   real_t minValue = m_data[0].value;
   unsigned minIndex = 0;
   
   for(unsigned i = 1; i < m_n; ++i) {
      if (m_data[i].value < minValue) {
         minValue = m_data[i].value;
         minIndex = i;
      }
   }
   
   return minIndex;
}

// @returns this SpectralSampleSet converted to RGB format
inline RgbaHDR SpectralSampleSet::getRGB() const {
   return RgbaHDR::fromVector(Vector3(m_data[0].value, m_data[1].value, 
                                      m_data[2].value));
}

// @returns this SpectralSampleSet as a HDR floating-point rgba struct (implicit conversion)
inline SpectralSampleSet::operator RgbaHDR() const {
   return RgbaHDR::fromVector(Vector3(m_data[0].value, m_data[1].value, 
                                      m_data[2].value));
}

}

#endif // SPECTRAL_SAMPLE_SET_INL_

