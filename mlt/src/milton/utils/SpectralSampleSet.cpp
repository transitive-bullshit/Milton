/**<!-------------------------------------------------------------------->
   @file   SpectralSampleSet.cpp
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

#include "SpectralSampleSet.h"

namespace milton {

unsigned SpectralSampleSet::s_defaultWavelengths[3] = {
   680, // red
   555, // green
   480  // blue
};

SpectralSampleSet::~SpectralSampleSet() {
   safeDeleteArray(m_data);
}

}

